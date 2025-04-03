#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/RenderTarget.h>

#include <echo/Maths/EchoMaths.h>
#include <echo/Maths/Matrix.h>
#include <echo/Maths/AxisAlignedBox.h>
#include <echo/Maths/Sphere.h>
#include <boost/foreach.hpp>

namespace Echo
{
	Camera::Camera(const std::string& name, Scene& scene) : mName(name),
		mScene(scene),
		mAutoTrackTarget(),
		mAutoTrackOffset(Vector3::ZERO),
		mWindowSet(false),
		mAutoAspectRatio(true),
		mCullFrustum(0),
		mUseRenderingDistance(true)
	{

		// Reasonable defaults to camera params
		mFOVy = f32(Maths::PI / 4.0f);
		mNearDist = 0.1f;
		mFarDist = 10000.0f;
		mAspect = 1.33333333333333f;
		mProjType = ProjectionTypes::PERSPECTIVE;
		SetFixedYawAxis(true); // Default to fixed yaw, like freelook since most people expect this

		InvalidateFrustum();
		InvalidateView();

		// Init matrices
		mViewMatrix = Matrix4::ZERO;

		// no reflection
		mReflect = false;
	}

	//-----------------------------------------------------------------------

	Camera::~Camera()
	{
	}
	//-----------------------------------------------------------------------

	Scene& Camera::GetScene(void) const
	{
		return mScene;
	}
	//-----------------------------------------------------------------------

	const std::string& Camera::GetName(void) const
	{
		return mName;
	}

	void Camera::SetPosition(f32 x, f32 y, f32 z)
	{
		Node::SetPosition(x,y,z);
		InvalidateView();
	}

	//-----------------------------------------------------------------------

	void Camera::SetPosition(const Vector3& vec)
	{
		Node::SetPosition(vec);
		InvalidateView();
	}

	//-----------------------------------------------------------------------
	void Camera::Move(const Vector3& vec)
	{
		Node::Translate(vec);
		InvalidateView();
	}

	//-----------------------------------------------------------------------

	void Camera::MoveRelative(const Vector3& vec)
	{
		Node::TranslateRelative(vec);
		InvalidateView();
	}

	//-----------------------------------------------------------------------

	void Camera::SetDirection(f32 x, f32 y, f32 z)
	{
		SetDirection(Vector3(x, y, z));
	}

	//-----------------------------------------------------------------------

	void Camera::SetDirection(const Vector3& vec)
	{
		// Do nothing if given a zero vector
		// (Replaced assert since this could happen with auto tracking camera and
		//  camera passes through the lookAt point)
		if(vec == Vector3::ZERO) return;

		// Remember, camera points down -Z of local axes!
		// Therefore reverse direction of direction vector before determining local Z
		Vector3 zAdjustVec = -vec;
		zAdjustVec.Normalise();

		Quaternion targetWorldOrientation;


		if(mYawFixed)
		{
			Vector3 xVec = mYawFixedAxis.Cross(zAdjustVec);
			xVec.Normalise();

			Vector3 yVec = zAdjustVec.Cross(xVec);
			yVec.Normalise();

			targetWorldOrientation.FromAxes(xVec, yVec, zAdjustVec);
		} else
		{

			// Get axes from current quaternion
			Vector3 axes[3];
			UpdateView();
			mRealOrientation.ToAxes(axes);
			Quaternion rotQuat;
			if((axes[2] + zAdjustVec).LengthSquared() < 0.00005f)
			{
				// Oops, a 180 degree turn (infinite possible rotation axes)
				// Default to yaw i.e. use current UP
				rotQuat.FromAngleAxis(Radian(Maths::PI), axes[1]);
			} else
			{
				// Derive shortest arc to new direction
				rotQuat = Maths::GetRotationTo(axes[2],zAdjustVec);

			}
			targetWorldOrientation = rotQuat * mRealOrientation;
		}

		// transform to parent space
		if (GetParent())
		{
			SetOrientation(GetParent()->GetDerivedOrientation().Inverse() * targetWorldOrientation);
		}else
		{
			SetOrientation(targetWorldOrientation);
		}

		// TODO If we have a fixed yaw axis, we mustn't break it by using the
		// shortest arc because this will sometimes cause a relative yaw
		// which will tip the camera

		InvalidateView();

	}

	//-----------------------------------------------------------------------

	Vector3 Camera::GetDirection(void) const
	{
		// Direction points down -Z by default
		return mOrientation * -Vector3::UNIT_Z;
	}

	//-----------------------------------------------------------------------

	Vector3 Camera::GetUp(void) const
	{
		return mOrientation * Vector3::UNIT_Y;
	}

	//-----------------------------------------------------------------------

	Vector3 Camera::GetRight(void) const
	{
		return mOrientation * Vector3::UNIT_X;
	}

	//-----------------------------------------------------------------------

	void Camera::LookAt(const Vector3& targetPoint)
	{
		UpdateView();
		this->SetDirection(targetPoint - mRealPosition);
	}

	//-----------------------------------------------------------------------

	void Camera::LookAt(f32 x, f32 y, f32 z)
	{
		Vector3 vTemp(x, y, z);
		this->LookAt(vTemp);
	}

	//-----------------------------------------------------------------------

	void Camera::Roll(const Radian& angle)
	{
		// Rotate around local Z axis
		Vector3 zAxis = mOrientation * Vector3::UNIT_Z;
		Rotate(zAxis, angle);

		InvalidateView();
	}

	//-----------------------------------------------------------------------

	void Camera::Yaw(const Radian& angle)
	{
		Vector3 yAxis;

		if(mYawFixed)
		{
			// Rotate around fixed yaw axis
			yAxis = mYawFixedAxis;
		} else
		{
			// Rotate around local Y axis
			yAxis = mOrientation * Vector3::UNIT_Y;
		}

		Rotate(yAxis, angle);

		InvalidateView();
	}

	//-----------------------------------------------------------------------

	void Camera::Pitch(const Radian& angle)
	{
		// Rotate around local X axis
		Vector3 xAxis = mOrientation * Vector3::UNIT_X;
		Rotate(xAxis, angle);

		InvalidateView();

	}

	//-----------------------------------------------------------------------

	void Camera::Rotate(const Vector3& axis, const Radian& angle)
	{
		Quaternion q;
		q.FromAngleAxis(angle, axis);
		Rotate(q);
	}

	//-----------------------------------------------------------------------

	void Camera::Rotate(const Quaternion& q)
	{
		// Note the order of the mult, i.e. q comes after

		// Normalise the quat to avoid cumulative problems with precision
		Quaternion qnorm = q;
		qnorm.Normalise();
		Node::SetOrientation(qnorm * GetOrientation());
		InvalidateView();

	}

	//-----------------------------------------------------------------------
	bool Camera::IsViewOutOfDate(void) const
	{
		// Overridden from Frustum to use local orientation / position offsets
		// Attached to node?
		Node* parent = GetParent();
		if (parent != 0)
		{
			if (mRecalcView ||
				parent->GetDerivedOrientation() != mLastParentOrientation ||
				parent->GetDerivedPosition() != mLastParentPosition)
			{
				// Ok, we're out of date with SceneNode we're attached to
				mLastParentOrientation = parent->GetDerivedOrientation();
				mLastParentPosition = parent->GetDerivedPosition();
				mRealOrientation = mLastParentOrientation * GetOrientation();
				mRealPosition = (mLastParentOrientation * GetPosition()) + mLastParentPosition;
				mRecalcView = true;
				mRecalcWindow = true;
			}
		}
		else
		{
			// Rely on own updates
			mRealOrientation = mOrientation;
			mRealPosition = mPosition;
		}

		// Deriving reflected orientation / position
		if(mRecalcView)
		{
			if(mReflect)
			{
				// Calculate reflected orientation, use up-vector as fallback axis.
				Vector3 dir = mRealOrientation * Vector3::NEGATIVE_UNIT_Z;
				Vector3 rdir = dir.Reflect(mReflectPlane.normal);
				Vector3 up = mRealOrientation * Vector3::UNIT_Y;
				mDerivedOrientation = Maths::GetRotationTo(dir,rdir, up) * mRealOrientation;

				// Calculate reflected position.
				mDerivedPosition = mReflectMatrix.TransformAffine(mRealPosition);
			} else
			{
				mDerivedOrientation = mRealOrientation;
				mDerivedPosition = mRealPosition;
			}
		}

		return mRecalcView;

	}

	void Camera::InvalidateView() const
	{
		mRecalcWindow = true;
		Frustum::InvalidateView();
	}

	void Camera::InvalidateFrustum(void) const
	{
		mRecalcWindow = true;
		Frustum::InvalidateFrustum();
	}

	std::ostream& operator<<(std::ostream& o, const Camera& c)
	{
		o << "Camera(Name='" << c.mName << "', pos=" << c.mPosition;
		Vector3 dir(c.mOrientation * Vector3(0, 0, -1));
		o << ", direction=" << dir << ",near=" << c.mNearDist;
		o << ", far=" << c.mFarDist << ", FOVy=" << c.mFOVy.ValueDegrees();
		o << ", aspect=" << c.mAspect << ", ";
		o << ", xoffset=" << c.mFrustumOffset.x << ", yoffset=" << c.mFrustumOffset.y;
		o << ", focalLength=" << c.mFocalLength << ", ";
		o << "NearFrustumPlane=" << c.mFrustumPlanes[FrustumPlanes::NEAR] << ", ";
		o << "FarFrustumPlane=" << c.mFrustumPlanes[FrustumPlanes::FAR] << ", ";
		o << "LeftFrustumPlane=" << c.mFrustumPlanes[FrustumPlanes::LEFT] << ", ";
		o << "RightFrustumPlane=" << c.mFrustumPlanes[FrustumPlanes::RIGHT] << ", ";
		o << "TopFrustumPlane=" << c.mFrustumPlanes[FrustumPlanes::TOP] << ", ";
		o << "BottomFrustumPlane=" << c.mFrustumPlanes[FrustumPlanes::BOTTOM];
		o << ")";

		return o;
	}

	void Camera::SetFixedYawAxis(bool useFixed, const Vector3& fixedAxis)
	{
		mYawFixed = useFixed;
		mYawFixedAxis = fixedAxis;
	}

	void Camera::SetOrientation(const Quaternion& q)
	{
		Node::SetOrientation(q);
		InvalidateView();
	}
	
	void Camera::RenderScene(RenderTarget& renderTarget)
	{
		_AutoTrack();
		renderTarget.SetProjectionMatrix(GetProjectionMatrix(), (GetProjectionType()==ProjectionTypes::ORTHOGRAPHIC));
		renderTarget.SetCurrentCamera(this);
		mScene.Render(renderTarget,*this);
		renderTarget.SetCurrentCamera(nullptr);
	}

	const Quaternion& Camera::GetDerivedOrientation(void) const
	{
		UpdateView();
		return mDerivedOrientation;
	}

	const Vector3& Camera::GetDerivedPosition(void) const
	{
		UpdateView();
		return mDerivedPosition;
	}

	Vector3 Camera::GetDerivedDirection(void) const
	{
		// Direction points down -Z
		UpdateView();
		return mDerivedOrientation * Vector3::NEGATIVE_UNIT_Z;
	}

	Vector3 Camera::GetDerivedUp(void) const
	{
		UpdateView();
		return mDerivedOrientation * Vector3::UNIT_Y;
	}

	Vector3 Camera::GetDerivedRight(void) const
	{
		UpdateView();
		return mDerivedOrientation * Vector3::UNIT_X;
	}

	const Quaternion& Camera::GetRealOrientation(void) const
	{
		UpdateView();
		return mRealOrientation;
	}

	const Vector3& Camera::GetRealPosition(void) const
	{
		UpdateView();
		return mRealPosition;
	}

	Vector3 Camera::GetRealDirection(void) const
	{
		// Direction points down -Z
		UpdateView();
		return mRealOrientation * Vector3::NEGATIVE_UNIT_Z;
	}

	Vector3 Camera::GetRealUp(void) const
	{
		UpdateView();
		return mRealOrientation * Vector3::UNIT_Y;
	}

	Vector3 Camera::GetRealRight(void) const
	{
		UpdateView();
		return mRealOrientation * Vector3::UNIT_X;
	}

	void Camera::SetAutoTracking(bool enabled, shared_ptr<NodeInterface> target, const Vector3& offset)
	{
		if(enabled)
		{
			assert(target && "target cannot be a null pointer if tracking is enabled");
			mAutoTrackTarget = target;
			mAutoTrackOffset = offset;
		} else
		{
			mAutoTrackTarget.reset();
		}
	}
	

	void Camera::_AutoTrack(void)
	{
		// NB assumes that all scene nodes have been updated
		if(mAutoTrackTarget)
		{
			LookAt(mAutoTrackTarget->GetDerivedPosition() + mAutoTrackOffset);
		}
	}
	
	Ray Camera::GetCameraToViewportRay(f32 screenX, f32 screenY) const
	{
		Ray ret;
		GetCameraToViewportRay(screenX, screenY, ret);
		return ret;
	}

	void Camera::GetCameraToViewportRay(f32 screenX, f32 screenY, Ray& outRay) const
	{
		Matrix4 inverseVP = (GetProjectionMatrix() * GetViewMatrix(true)).Inverse();

	#if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
		// We need to convert screen point to our oriented viewport (temp solution)
		f32 tX = screenX;
		f32 a = GetOrientationMode() * Maths::HALF_PI;
		screenX = Maths::Cos(a) * (tX - 0.5f) + Maths::Sin(a) * (screenY - 0.5f) + 0.5f;
		screenY = Maths::Sin(a) * (tX - 0.5f) + Maths::Cos(a) * (screenY - 0.5f) + 0.5f;
		if((int) GetOrientationMode()&1) screenY = 1.f - screenY;
	#endif

		f32 nx = (2.0f * screenX) - 1.0f;
		f32 ny = 1.0f - (2.0f * screenY);
		Vector3 nearPoint(nx, ny, -1.f);
		// Use midPoint rather than far point to avoid issues with infinite projection
		Vector3 midPoint(nx, ny, 0.0f);

		// Get ray origin and ray target on near plane in world space
		Vector3 rayOrigin, rayTarget;

		rayOrigin = inverseVP * nearPoint;
		rayTarget = inverseVP * midPoint;

		Vector3 rayDirection = rayTarget - rayOrigin;
		rayDirection.Normalise();

		outRay.SetOrigin(rayOrigin);
		outRay.SetDirection(rayDirection);
	}
	
	void Camera::UpdateAspectForViewport(const Viewport& viewport, const RenderTarget& renderTarget)
	{
		//Update the camera aspect ratio if needed.
		if(GetAutoAspectRatio())
		{
			if(viewport.GetHasFixedAspectRatio())
			{
				SetAspectRatio(viewport.GetAspectRatio());
			}else
			{
				f32 targetWidth = static_cast<f32>(renderTarget.GetWidth());
				f32 targetHeight = static_cast<f32>(renderTarget.GetHeight());
				assert(targetHeight>=0 && targetWidth>=0 && "Detected a viewport with a 0 dimension.");
				Viewport::Rectangle rectangle = viewport.GetRectangle(targetWidth / targetHeight);
				SetAspectRatio((rectangle.GetWidth() * targetWidth) / (rectangle.GetHeight() * targetHeight));
			}
		}
	}

	void Camera::SetWindow(f32 Left, f32 Top, f32 Right, f32 Bottom)
	{
		mWLeft = Left;
		mWTop = Top;
		mWRight = Right;
		mWBottom = Bottom;

		mWindowSet = true;
		mRecalcWindow = true;
	}

	void Camera::ResetWindow()
	{
		mWindowSet = false;
	}

	void Camera::SetWindowImpl() const
	{
		if(!mWindowSet || !mRecalcWindow)
			return;

		// Calculate general projection parameters
		f32 vpLeft, vpRight, vpBottom, vpTop;
		CalcProjectionParameters(vpLeft, vpRight, vpBottom, vpTop);

		f32 vpWidth = vpRight - vpLeft;
		f32 vpHeight = vpTop - vpBottom;

		f32 wvpLeft = vpLeft + mWLeft * vpWidth;
		f32 wvpRight = vpLeft + mWRight * vpWidth;
		f32 wvpTop = vpTop - mWTop * vpHeight;
		f32 wvpBottom = vpTop - mWBottom * vpHeight;

		Vector3 vp_ul(wvpLeft, wvpTop, -mNearDist);
		Vector3 vp_ur(wvpRight, wvpTop, -mNearDist);
		Vector3 vp_bl(wvpLeft, wvpBottom, -mNearDist);
		Vector3 vp_br(wvpRight, wvpBottom, -mNearDist);

		Matrix4 inv = mViewMatrix.InverseAffine();

		Vector3 vw_ul = inv.TransformAffine(vp_ul);
		Vector3 vw_ur = inv.TransformAffine(vp_ur);
		Vector3 vw_bl = inv.TransformAffine(vp_bl);
		Vector3 vw_br = inv.TransformAffine(vp_br);

		mWindowClipPlanes.clear();
		if(mProjType == ProjectionTypes::PERSPECTIVE)
		{
			Vector3 position = GetPositionForViewUpdate();
			mWindowClipPlanes.push_back(Plane(position, vw_bl, vw_ul));
			mWindowClipPlanes.push_back(Plane(position, vw_ul, vw_ur));
			mWindowClipPlanes.push_back(Plane(position, vw_ur, vw_br));
			mWindowClipPlanes.push_back(Plane(position, vw_br, vw_bl));
		} else
		{
			Vector3 x_axis(inv[0][0], inv[0][1], inv[0][2]);
			Vector3 y_axis(inv[1][0], inv[1][1], inv[1][2]);
			x_axis.Normalise();
			y_axis.Normalise();
			mWindowClipPlanes.push_back(Plane(x_axis, vw_bl));
			mWindowClipPlanes.push_back(Plane(-x_axis, vw_ur));
			mWindowClipPlanes.push_back(Plane(y_axis, vw_bl));
			mWindowClipPlanes.push_back(Plane(-y_axis, vw_ur));
		}

		mRecalcWindow = false;

	}

	const std::vector<Plane>& Camera::GetWindowPlanes(void) const
	{
		UpdateView();
		SetWindowImpl();
		return mWindowClipPlanes;
	}

	f32 Camera::GetBoundingRadius(void) const
	{
		// return a little bigger than the near distance
		// just to keep things just outside
		return mNearDist * 1.5f;

	}

	const Vector3& Camera::GetPositionForViewUpdate(void) const
	{
		// Note no update, because we're calling this from the update!
		return mRealPosition;
	}

	const Quaternion& Camera::GetOrientationForViewUpdate(void) const
	{
		return mRealOrientation;
	}

	bool Camera::GetAutoAspectRatio(void) const
	{
		return mAutoAspectRatio;
	}

	void Camera::SetAutoAspectRatio(bool autoratio)
	{
		mAutoAspectRatio = autoratio;
	}

	bool Camera::IsVisible(const AxisAlignedBox& bound, FrustumPlane* culledBy) const
	{
		if(mCullFrustum)
		{
			return mCullFrustum->IsVisible(bound, culledBy);
		} else
		{
			return Frustum::IsVisible(bound, culledBy);
		}
	}

	bool Camera::IsVisible(const Sphere& bound, FrustumPlane* culledBy) const
	{
		if(mCullFrustum)
		{
			return mCullFrustum->IsVisible(bound, culledBy);
		} else
		{
			return Frustum::IsVisible(bound, culledBy);
		}
	}

	bool Camera::IsVisible(const Vector3& vert, FrustumPlane* culledBy) const
	{
		if(mCullFrustum)
		{
			return mCullFrustum->IsVisible(vert, culledBy);
		} else
		{
			return Frustum::IsVisible(vert, culledBy);
		}
	}

	const Frustum::WorldSpaceCorners& Camera::GetWorldSpaceCorners(void) const
	{
		if(mCullFrustum)
		{
			return mCullFrustum->GetWorldSpaceCorners();
		} else
		{
			return Frustum::GetWorldSpaceCorners();
		}
	}

	const Plane& Camera::GetFrustumPlane(unsigned short plane) const
	{
		if(mCullFrustum)
		{
			return mCullFrustum->GetFrustumPlane(plane);
		} else
		{
			return Frustum::GetFrustumPlane(plane);
		}
	}

	bool Camera::ProjectSphere(const Sphere& sphere,
								f32* left, f32* top, f32* right, f32* bottom) const
	{
		if(mCullFrustum)
		{
			return mCullFrustum->ProjectSphere(sphere, left, top, right, bottom);
		} else
		{
			return Frustum::ProjectSphere(sphere, left, top, right, bottom);
		}
	}

	f32 Camera::GetNearPlane(void) const
	{
		if(mCullFrustum)
		{
			return mCullFrustum->GetNearPlane();
		} else
		{
			return Frustum::GetNearPlane();
		}
	}

	f32 Camera::GetFarPlane(void) const
	{
		if(mCullFrustum)
		{
			return mCullFrustum->GetFarPlane();
		} else
		{
			return Frustum::GetFarPlane();
		}
	}

	const Matrix4& Camera::GetViewMatrix(void) const
	{
		if(mCullFrustum)
		{
			return mCullFrustum->GetViewMatrix();
		} else
		{
			return Frustum::GetViewMatrix();
		}
	}

	const Matrix4& Camera::GetViewMatrix(bool ownFrustumOnly) const
	{
		if(ownFrustumOnly)
		{
			return Frustum::GetViewMatrix();
		} else
		{
			return GetViewMatrix();
		}
	}

	std::vector<Vector4> Camera::GetRayForwardIntersect(const Vector3& anchor, const Vector3 *dir, f32 planeOffset) const
	{
		// Get the intersections of frustum rays with a plane of interest. The plane is
		// assumed to have constant z. If this is not the case, rays should be rotated
		// beforehand to work in a coordinate system in which this is true.

		std::vector<Vector4> res;

		if(!dir)
			return res;

		int infpt[4] = {0, 0, 0, 0}; // 0=finite, 1=infinite, 2=straddles infinity
		Vector3 vec[4];

		// find how much the anchor point must be displaced in the plane's
		// constant variable
		f32 delta = planeOffset - anchor.z;

		// now set the intersection point and note whether it is a 
		// point at infinity or straddles infinity
		unsigned int i;
		for(i = 0; i < 4; i++)
		{
			f32 test = dir[i].z * delta;
			if(test == 0.0)
			{
				vec[i] = dir[i];
				infpt[i] = 1;
			} else
			{
				f32 lambda = delta / dir[i].z;
				vec[i] = anchor + (lambda * dir[i]);
				if(test < 0.0)
					infpt[i] = 2;
			}
		}

		for(i = 0; i < 4; i++)
		{
			// store the finite intersection points
			if(infpt[i] == 0)
				res.push_back(Vector4(vec[i].x, vec[i].y, vec[i].z, 1.0));
			else
			{
				// handle the infinite points of intersection;
				// cases split up into the possible frustum planes 
				// pieces which may contain a finite intersection point
				int nextind = (i + 1) % 4;
				int prevind = (i + 3) % 4;
				if((infpt[prevind] == 0) || (infpt[nextind] == 0))
				{
					if(infpt[i] == 1)
						res.push_back(Vector4(vec[i].x, vec[i].y, vec[i].z, 0.0));
					else
					{
						// handle the intersection points that straddle infinity (back-project)
						if(infpt[prevind] == 0)
						{
							Vector3 temp = vec[prevind] - vec[i];
							res.push_back(Vector4(temp.x, temp.y, temp.z, 0.0));
						}
						if(infpt[nextind] == 0)
						{
							Vector3 temp = vec[nextind] - vec[i];
							res.push_back(Vector4(temp.x, temp.y, temp.z, 0.0));
						}
					}
				} // end if we need to add an intersection point to the list
			} // end if infinite point needs to be considered
		} // end loop over frustun corners

		// we end up with either 0, 3, 4, or 5 intersection points
		return res;
	}

	void Camera::ForwardIntersect(const Plane& worldPlane, std::vector<Vector4>& intersect3d) const
	{
		// Forward intersect the camera's frustum rays with a specified plane of interest. Note
		// that if the frustum rays shoot out and would back project onto the plane, this means
		// the forward intersection of the frustum would occur at the line at infinity.
		
		Vector3 trCorner = GetWorldSpaceCorners()[0];
		Vector3 tlCorner = GetWorldSpaceCorners()[1];
		Vector3 blCorner = GetWorldSpaceCorners()[2];
		Vector3 brCorner = GetWorldSpaceCorners()[3];

		// need some sort of rotation that will bring the plane normal to the z axis
		Plane pval = worldPlane;
		if(pval.normal.z < 0.0)
		{
			pval.normal *= -1.0;
			pval.d *= -1.0;
		}
		Quaternion invPlaneRot = Maths::GetRotationTo(pval.normal,Vector3::UNIT_Z);

		// get rotated light
		Vector3 lPos = invPlaneRot * GetDerivedPosition();
		Vector3 vec[4];
		vec[0] = invPlaneRot * trCorner - lPos;
		vec[1] = invPlaneRot * tlCorner - lPos;
		vec[2] = invPlaneRot * blCorner - lPos;
		vec[3] = invPlaneRot * brCorner - lPos;

		// compute intersection points on plane
		std::vector<Vector4> iPnt = GetRayForwardIntersect(lPos, vec, -pval.d);

		Quaternion planeRot = invPlaneRot.Inverse();
		intersect3d.clear();
		for(unsigned int i = 0; i < iPnt.size(); i++)
		{
			Vector3 intersection = planeRot * Vector3(iPnt[i].x, iPnt[i].y, iPnt[i].z);
			intersect3d.push_back(Vector4(intersection.x, intersection.y, intersection.z, iPnt[i].w));
		}
	}

	void Camera::SynchroniseBaseSettingsWith(const Camera* cam)
	{
		this->SetPosition(cam->GetPosition());
		this->SetProjectionType(cam->GetProjectionType());
		this->SetOrientation(cam->GetOrientation());
		this->SetAspectRatio(cam->GetAspectRatio());
		this->SetNearPlane(cam->GetNearPlane());
		this->SetFarPlane(cam->GetFarPlane());
		this->SetUseRenderingDistance(cam->GetUseRenderingDistance());
		this->SetFOVy(cam->GetFOVy());
		this->SetFocalLength(cam->GetFocalLength());

		// Don't do these, they're not base settings and can cause referencing issues
		//this->setLodCamera(cam->getLodCamera());
		//this->setCullingFrustum(cam->getCullingFrustum());

	}
}
