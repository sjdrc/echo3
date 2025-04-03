#include <echo/Graphics/Frustum.h>
#include <echo/Maths/EchoMaths.h>
#include <echo/Maths/Matrix3.h>
#include <echo/Maths/Sphere.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Graphics/SubMesh.h>

namespace Echo
{
	const f32 Frustum::INFINITE_FAR_PLANE_ADJUST = 0.00001f;

	Frustum::Frustum() : 
		mProjType(ProjectionTypes::PERSPECTIVE), 
		mFOVy(f32(Maths::PI/4.0f)), 
		mFarDist(1000.0f), 
		mNearDist(0.1f), 
		mAspect(1.33333333333333f), 
		mOrthoHeight(1000),
		mFrustumOffset(Vector2::ZERO),
		mFocalLength(1.0f),
		mLastParentOrientation(Quaternion::IDENTITY),
		mLastParentPosition(Vector3::ZERO),
		mRecalcFrustum(true), 
		mRecalcView(true), 
		mRecalcFrustumPlanes(true),
		mRecalcWorldSpaceCorners(true),
		mRecalcVertexData(true),
		mCustomViewMatrix(false),
		mCustomProjMatrix(false),
		mFrustumExtentsManuallySet(false),
		mOrientationMode(OrientationModes::DEGREE_0),
		mReflect(false), 
		mObliqueDepthProjection(false)
	{
		UpdateView();
		UpdateFrustum();
	}

	Frustum::~Frustum()
	{
		// Do nothing
	}

	void Frustum::SetFOVy(const Radian& fov)
	{
		mFOVy = fov;
		InvalidateFrustum();
	}
	
	const Radian& Frustum::GetFOVy(void) const
	{
		return mFOVy;
	}

	void Frustum::SetFOV(const Radian& fov)
	{
		assert(mAspect>0.f);
		if(mAspect>0.f)
		{
			SetFOVy(fov/mAspect);
		}
	}
	
	Radian Frustum::GetFOV() const
	{
		return (mFOVy*mAspect);
	}

	void Frustum::SetFarPlane(f32 farPlane)
	{
		mFarDist = farPlane;
		InvalidateFrustum();
	}

	f32 Frustum::GetFarPlane(void) const
	{
		return mFarDist;
	}

	void Frustum::SetNearPlane(f32 nearPlane)
	{
		assert(nearPlane > 0);
		mNearDist = nearPlane;
		InvalidateFrustum();
	}

	f32 Frustum::GetNearPlane(void) const
	{
		return mNearDist;
	}

	void Frustum::SetFrustumOffset(const Vector2& offset)
	{
		mFrustumOffset = offset;
		InvalidateFrustum();
	}

	void Frustum::SetFrustumOffset(f32 horizontal, f32 vertical)
	{
		SetFrustumOffset(Vector2(horizontal, vertical));
	}

	const Vector2& Frustum::GetFrustumOffset() const
	{
		return mFrustumOffset;
	}

	void Frustum::SetFocalLength(f32 focalLength)
	{
		assert( focalLength > 0);
		mFocalLength = focalLength;
		InvalidateFrustum();
	}

	f32 Frustum::GetFocalLength() const
	{
		return mFocalLength;
	}

	const Matrix4& Frustum::GetProjectionMatrix(void) const
	{
		UpdateFrustum();
		return mProjMatrix;
	}

	const Matrix4& Frustum::GetViewMatrix(void) const
	{
		UpdateView();
		return mViewMatrix;
	}

	const Frustum::FrustumPlaneArray& Frustum::GetFrustumPlanes(void) const
	{
		// Make any pending updates to the calculated frustum planes
		UpdateFrustumPlanes();

		return mFrustumPlanes;
	}

	const Plane& Frustum::GetFrustumPlane(unsigned short plane) const
	{
		// Make any pending updates to the calculated frustum planes
		UpdateFrustumPlanes();

		return mFrustumPlanes[plane];

	}

	bool Frustum::IsVisible(const AxisAlignedBox& bound, FrustumPlane* culledBy) const
	{
		// Null boxes always invisible
		if (bound.IsNull()) return false;

		// Infinite boxes always visible
		if (bound.IsInfinite()) return true;

		// Make any pending updates to the calculated frustum planes
		UpdateFrustumPlanes();

		// Get centre of the box
		Vector3 centre = bound.GetCentre();
		// Get the half-size of the box
		Vector3 halfSize = bound.GetHalfSize();

		// For each plane, see if all points are on the negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (plane == FrustumPlanes::FAR && mFarDist == 0)
				continue;

			Plane::Side side = mFrustumPlanes[plane].GetSide(centre, halfSize);
			if (side == Plane::Sides::NEGATIVE)
			{
				// ALL corners on negative side therefore out of view
				if (culledBy)
					*culledBy = (FrustumPlane)plane;
				return false;
			}

		}

		return true;
	}

	bool Frustum::IsVisible(const Vector3& vert, FrustumPlane* culledBy) const
	{
		// Make any pending updates to the calculated frustum planes
		UpdateFrustumPlanes();

		// For each plane, see if all points are on the negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (plane == FrustumPlanes::FAR && mFarDist == 0)
				continue;

			if (mFrustumPlanes[plane].GetSide(vert) == Plane::Sides::NEGATIVE)
			{
				// ALL corners on negative side therefore out of view
				if (culledBy)
					*culledBy = (FrustumPlane)plane;
				return false;
			}

		}

		return true;
	}

	bool Frustum::IsVisible(const Sphere& sphere, FrustumPlane* culledBy) const
	{
		// Make any pending updates to the calculated frustum planes
		UpdateFrustumPlanes();

		// For each plane, see if sphere is on negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (plane == FrustumPlanes::FAR && mFarDist == 0)
				continue;

			// If the distance from sphere center to plane is negative, and 'more negative' 
			// than the radius of the sphere, sphere is outside frustum
			if (mFrustumPlanes[plane].GetDistance(sphere.GetCenter()) < -sphere.GetRadius())
			{
				// ALL corners on negative side therefore out of view
				if (culledBy)
					*culledBy = (FrustumPlane)plane;
				return false;
			}

		}

		return true;
	}

	void Frustum::CalcProjectionParameters(f32& left, f32& right, f32& bottom, f32& top) const
	{ 
		if (mCustomProjMatrix)
		{
			// Convert clipspace corners to camera space
			Matrix4 invProj = mProjMatrix.Inverse();
			Vector3 topLeft(-0.5f, 0.5f, 0.0f);
			Vector3 bottomRight(0.5f, -0.5f, 0.0f);

			topLeft = invProj * topLeft;
			bottomRight = invProj * bottomRight;

			left = topLeft.x;
			top = topLeft.y;
			right = bottomRight.x;
			bottom = bottomRight.y;

		}
		else
		{
			if (mFrustumExtentsManuallySet)
			{
				left = mLeft;
				right = mRight;
				top = mTop;
				bottom = mBottom;
			}
			// Calculate general projection parameters
			else if (mProjType == ProjectionTypes::PERSPECTIVE)
			{
				Radian thetaY (mFOVy * 0.5f);
				f32 tanThetaY = Maths::Tan<f32>(thetaY);
				f32 tanThetaX = tanThetaY * mAspect;

				f32 nearFocal = mNearDist / mFocalLength;
				f32 nearOffsetX = mFrustumOffset.x * nearFocal;
				f32 nearOffsetY = mFrustumOffset.y * nearFocal;
				f32 half_w = tanThetaX * mNearDist;
				f32 half_h = tanThetaY * mNearDist;

				left   = - half_w + nearOffsetX;
				right  = + half_w + nearOffsetX;
				bottom = - half_h + nearOffsetY;
				top	= + half_h + nearOffsetY;

				mLeft = left;
				mRight = right;
				mTop = top;
				mBottom = bottom;
			}else
			{
				// Unknown how to apply frustum offset to orthographic camera, just ignore here
				f32 half_w = GetOrthoWindowWidth() * 0.5f;
				f32 half_h = GetOrthoWindowHeight() * 0.5f;

				left   = - half_w;
				right  = + half_w;
				bottom = - half_h;
				top	= + half_h;

				mLeft = left;
				mRight = right;
				mTop = top;
				mBottom = bottom;
			}

		}
	}
	//-----------------------------------------------------------------------
	void Frustum::UpdateFrustumImpl(void) const
	{
		// Common calcs
		f32 left, right, bottom, top;
		CalcProjectionParameters(left, right, bottom, top);

		if (!mCustomProjMatrix)
		{

			// The code below will dealing with general projection 
			// parameters, similar glFrustum and glOrtho.
			// Doesn't optimise manually except division operator, so the 
			// code more self-explaining.

			f32 inv_w = 1 / (right - left);
			f32 inv_h = 1 / (top - bottom);
			f32 inv_d = 1 / (mFarDist - mNearDist);

			// Recalc if frustum params changed
			if (mProjType == ProjectionTypes::PERSPECTIVE)
			{
				// Calc matrix elements
				f32 A = 2 * mNearDist * inv_w;
				f32 B = 2 * mNearDist * inv_h;
				f32 C = (right + left) * inv_w;
				f32 D = (top + bottom) * inv_h;
				f32 q, qn;
				if (mFarDist == 0)
				{
					// Infinite far plane
					q = Frustum::INFINITE_FAR_PLANE_ADJUST - 1;
					qn = mNearDist * (Frustum::INFINITE_FAR_PLANE_ADJUST - 2);
				}else
				{
					q = - (mNearDist) * inv_d;
					// This used to be:
					//	q = - (mFarDist + mNearDist) * inv_d;
					// but the result was a matrix that GX (Wii) didn't like. I compared the matrix generation code in libogc
					// with this and found that this was the difference that prevented it from working. I don't see any
					// negative effects with the GLRenderTarget.
					
					qn = -2 * (mFarDist * mNearDist) * inv_d;
				}

				// NB: This creates 'uniform' perspective projection matrix,
				// which depth range [-1,1], right-handed rules
				//
				// [ A   0   C   0  ]
				// [ 0   B   D   0  ]
				// [ 0   0   q   qn ]
				// [ 0   0   -1  0  ]
				//
				// A = 2 * near / (right - left)
				// B = 2 * near / (top - bottom)
				// C = (right + left) / (right - left)
				// D = (top + bottom) / (top - bottom)
				// q = - (far + near) / (far - near)
				// qn = - 2 * (far * near) / (far - near)

				mProjMatrix = Matrix4::ZERO;
				mProjMatrix[0][0] = A;
				mProjMatrix[0][2] = C;
				mProjMatrix[1][1] = B;
				mProjMatrix[1][2] = D;
				mProjMatrix[2][2] = q;
				mProjMatrix[2][3] = qn;
				mProjMatrix[3][2] = -1;

				if (mObliqueDepthProjection)
				{
					// Translate the plane into view space

					// Don't use getViewMatrix here, incase overrided by 
					// camera and return a cull frustum view matrix
					UpdateView();
					Plane plane = mViewMatrix * mObliqueProjPlane;

					// Thanks to Eric Lenyel for posting this calculation 
					// at www.terathon.com

					// Calculate the clip-space corner point opposite the 
					// clipping plane
					// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
					// transform it into camera space by multiplying it
					// by the inverse of the projection matrix

					/* generalised version
					Vector4 q = matrix.inverse() * 
					Vector4(Maths::Sign(plane.normal.x), 
					Maths::Sign(plane.normal.y), 1.0f, 1.0f);
					*/
					Vector4 q;
					q.x = (Maths::Sign(plane.normal.x) + mProjMatrix[0][2]) / mProjMatrix[0][0];
					q.y = (Maths::Sign(plane.normal.y) + mProjMatrix[1][2]) / mProjMatrix[1][1];
					q.z = -1;
					q.w = (1 + mProjMatrix[2][2]) / mProjMatrix[2][3];

					// Calculate the scaled plane vector
					Vector4 clipPlane4d(plane.normal.x, plane.normal.y, plane.normal.z, plane.d);
					Vector4 c = clipPlane4d * (2 / (clipPlane4d.DotProduct(q)));

					// Replace the third row of the projection matrix
					mProjMatrix[2][0] = c.x;
					mProjMatrix[2][1] = c.y;
					mProjMatrix[2][2] = c.z + 1;
					mProjMatrix[2][3] = c.w; 
				}
			}else if (mProjType == ProjectionTypes::ORTHOGRAPHIC)
			{
				f32 A = 2 * inv_w;
				f32 B = 2 * inv_h;
				f32 C = - (right + left) * inv_w;
				f32 D = - (top + bottom) * inv_h;
				f32 q, qn;
				if (mFarDist == 0)
				{
					// Can not do infinite far plane here, avoid divided zero only
					q = - Frustum::INFINITE_FAR_PLANE_ADJUST / mNearDist;
					qn = - Frustum::INFINITE_FAR_PLANE_ADJUST - 1;
				}
				else
				{
					q = - 2 * inv_d;
					qn = - (mFarDist + mNearDist)  * inv_d;
				}

				// NB: This creates 'uniform' orthographic projection matrix,
				// which depth range [-1,1], right-handed rules
				//
				// [ A   0   0   C  ]
				// [ 0   B   0   D  ]
				// [ 0   0   q   qn ]
				// [ 0   0   0   1  ]
				//
				// A = 2 * / (right - left)
				// B = 2 * / (top - bottom)
				// C = - (right + left) / (right - left)
				// D = - (top + bottom) / (top - bottom)
				// q = - 2 / (far - near)
				// qn = - (far + near) / (far - near)

				mProjMatrix = Matrix4::ZERO;
				mProjMatrix[0][0] = A;
				mProjMatrix[0][3] = C;
				mProjMatrix[1][1] = B;
				mProjMatrix[1][3] = D;
				mProjMatrix[2][2] = q;
				mProjMatrix[2][3] = qn;
				mProjMatrix[3][3] = 1;
			}
		}

#if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
		// Deal with orientation mode
		mProjMatrix = mProjMatrix * Quaternion(Degree(mOrientationMode * 90.f), Vector3::UNIT_Z);
#endif
		// Calculate bounding box (local)
		// Box is from 0, down -Z, max dimensions as determined from far plane
		// If infinite view frustum just pick a far value
		f32 farDist = (mFarDist == 0) ? 100000 : mFarDist;
		// Near plane bounds
		Vector3 min(left, bottom, -farDist);
		Vector3 max(right, top, 0);

		if (mCustomProjMatrix)
		{
			// Some custom projection matrices can have unusual inverted settings
			// So make sure the AABB is the right way around to start with
			Vector3 tmp = min;
			min.MakeFloor(max);
			max.MakeCeil(tmp);
		}

		if (mProjType == ProjectionTypes::PERSPECTIVE)
		{
			// Merge with far plane bounds
			f32 radio = farDist / mNearDist;
			min.MakeFloor(Vector3(left * radio, bottom * radio, -farDist));
			max.MakeCeil(Vector3(right * radio, top * radio, 0));
		}
		mBoundingBox.SetExtents(min, max);

		mRecalcFrustum = false;

		// Signal to update frustum clipping planes
		mRecalcFrustumPlanes = true;
	}

	void Frustum::UpdateFrustum(void) const
	{
		if (IsFrustumOutOfDate())
		{
			UpdateFrustumImpl();
		}
	}

	void Frustum::GetVertexData(shared_ptr<SubMesh> targetSubMesh, bool updateOnlyIfChangedSinceLastAcquire, bool clearMesh) const
	{
		if (!updateOnlyIfChangedSinceLastAcquire || (mRecalcVertexData && updateOnlyIfChangedSinceLastAcquire))
		{
			// Note: Even though we can dealing with general projection matrix here,
			//	   but because it's incompatibly with infinite far plane, thus, we
			//	   still need to working with projection parameters.
	
			// Calc near plane corners
			f32 vpLeft, vpRight, vpBottom, vpTop;
			CalcProjectionParameters(vpLeft, vpRight, vpBottom, vpTop);

			// Treat infinite fardist as some arbitrary far value
			f32 farDist = (mFarDist == 0) ? 100000 : mFarDist;

			// Calc far plane corners
			f32 radio = mProjType == ProjectionTypes::PERSPECTIVE ? farDist / mNearDist : 1;
			f32 farLeft = vpLeft * radio;
			f32 farRight = vpRight * radio;
			f32 farBottom = vpBottom * radio;
			f32 farTop = vpTop * radio;

			// Calculate vertex positions (local)
			// 0 is the origin
			// 1, 2, 3, 4 are the points on the near plane, top left first, clockwise
			// 5, 6, 7, 8 are the points on the far plane, top left first, clockwise
			shared_ptr<VertexBuffer> vertexBuffer = targetSubMesh->GetVertexBuffer();
			VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
			const Size NUMBER_OF_REQUIRED_ELEMENTS = 32;
			if(clearMesh)
			{
				if(vertexBuffer->GetCapacity() < NUMBER_OF_REQUIRED_ELEMENTS)
				{
					// Need to re-allocate
					vertexBuffer->Allocate(NUMBER_OF_REQUIRED_ELEMENTS);
					// We use this value below as the start of the write position.
					vertexBuffer->SetNumberOfElements(0);
				}
			}else
			{
				// Do we have enough space?
				if(vertexBuffer->GetCapacity()-vertexBuffer->GetNumberOfElements() < NUMBER_OF_REQUIRED_ELEMENTS)
				{
					ECHO_LOG_WARNING("Target SubMesh does not have enough space for data and I was told not to clear it. " << NUMBER_OF_REQUIRED_ELEMENTS <<
							" vertices are required. Buffer's capacity: " << vertexBuffer->GetCapacity() << " Number of existing elements: " << vertexBuffer->GetNumberOfElements());
					return;
				}
			}
			
			Size i = vertexBuffer->GetNumberOfElements();
			
			// near plane (remember frustum is going in -Z direction)
			vertices[i++] = Vector3(vpLeft,		vpTop,		-mNearDist);
			vertices[i++] = Vector3(vpRight,	vpTop,		-mNearDist);

			vertices[i++] = Vector3(vpRight,	vpTop,		-mNearDist);
			vertices[i++] = Vector3(vpRight,	vpBottom,	-mNearDist);

			vertices[i++] = Vector3(vpRight,	vpBottom,	-mNearDist);
			vertices[i++] = Vector3(vpLeft,		vpBottom,	-mNearDist);

			vertices[i++] = Vector3(vpLeft,		vpBottom,	-mNearDist);
			vertices[i++] = Vector3(vpLeft,		vpTop,		-mNearDist);
			
			// far plane (remember frustum is going in -Z direction)
			vertices[i++] = Vector3(farLeft,	farTop,		-farDist);
			vertices[i++] = Vector3(farRight,	farTop,		-farDist);

			vertices[i++] = Vector3(farRight,	farTop,		-farDist);
			vertices[i++] = Vector3(farRight,	farBottom,	-farDist);

			vertices[i++] = Vector3(farRight,	farBottom,	-farDist);
			vertices[i++] = Vector3(farLeft,	farBottom,	-farDist);

			vertices[i++] = Vector3(farLeft,	farBottom,	-farDist);
			vertices[i++] = Vector3(farLeft,	farTop,		-farDist);

			// Sides of the pyramid
			vertices[i++] = Vector3::ZERO;
			vertices[i++] = Vector3(vpLeft,		vpTop,		-mNearDist);
			
			vertices[i++] = Vector3::ZERO;
			vertices[i++] = Vector3(vpRight,	vpTop,		-mNearDist);

			vertices[i++] = Vector3::ZERO;
			vertices[i++] = Vector3(vpRight,	vpBottom,	-mNearDist);

			vertices[i++] = Vector3::ZERO;
			vertices[i++] = Vector3(vpLeft,		vpBottom,	-mNearDist);

			// Sides of the box
			vertices[i++] = Vector3(vpLeft,		vpTop,		-mNearDist);
			vertices[i++] = Vector3(farLeft,	farTop,		-farDist);

			vertices[i++] = Vector3(vpRight,	vpTop,		-mNearDist);
			vertices[i++] = Vector3(farRight,	farTop,		-farDist);

			vertices[i++] = Vector3(vpRight,	vpBottom,	-mNearDist);
			vertices[i++] = Vector3(farRight,	farBottom,	-farDist);

			vertices[i++] = Vector3(vpLeft,		vpBottom,	-mNearDist);
			vertices[i++] = Vector3(farLeft,	farBottom,	-farDist);
			vertexBuffer->SetNumberOfElements(i);
			
			targetSubMesh->Finalise();

			mRecalcVertexData = false;
		}
	}

	bool Frustum::IsViewOutOfDate(void) const
	{
		return mRecalcView;
	}

	bool Frustum::IsFrustumOutOfDate(void) const
	{
		// Deriving custom near plane from linked plane?
		if (mObliqueDepthProjection)
		{
			// Out of date when view out of data since plane needs to be in view space
			if (IsViewOutOfDate())
			{
				mRecalcFrustum = true;
			}
		}

		return mRecalcFrustum;
	}

	void Frustum::UpdateViewImpl(void) const
	{
		// Get orientation from quaternion
		if (!mCustomViewMatrix)
		{
			Matrix3 rot;
			const Quaternion& orientation = GetOrientationForViewUpdate();
			const Vector3& position = GetPositionForViewUpdate();

			mViewMatrix = Maths::MakeViewMatrix(position, orientation, mReflect? &mReflectMatrix : 0);
		}

		mRecalcView = false;

		// Signal to update frustum clipping planes
		mRecalcFrustumPlanes = true;
		// Signal to update world space corners
		mRecalcWorldSpaceCorners = true;
		// Signal to update frustum if oblique plane enabled,
		// since plane needs to be in view space
		if (mObliqueDepthProjection)
		{
			mRecalcFrustum = true;
		}
	}

	void Frustum::CalcViewMatrixRelative(const Vector3& relPos, Matrix4& matToUpdate) const
	{
		Matrix4 matTrans = Matrix4::IDENTITY;
		matTrans.SetTranslation(relPos);
		matToUpdate = GetViewMatrix() * matTrans;

	}

	void Frustum::UpdateView(void) const
	{
		if (IsViewOutOfDate())
		{
			UpdateViewImpl();
		}
	}

	void Frustum::UpdateFrustumPlanesImpl(void) const
	{
		Matrix4 combo = mProjMatrix * mViewMatrix;

		mFrustumPlanes[FrustumPlanes::LEFT].normal.x = combo[3][0] + combo[0][0];
		mFrustumPlanes[FrustumPlanes::LEFT].normal.y = combo[3][1] + combo[0][1];
		mFrustumPlanes[FrustumPlanes::LEFT].normal.z = combo[3][2] + combo[0][2];
		mFrustumPlanes[FrustumPlanes::LEFT].d = combo[3][3] + combo[0][3];

		mFrustumPlanes[FrustumPlanes::RIGHT].normal.x = combo[3][0] - combo[0][0];
		mFrustumPlanes[FrustumPlanes::RIGHT].normal.y = combo[3][1] - combo[0][1];
		mFrustumPlanes[FrustumPlanes::RIGHT].normal.z = combo[3][2] - combo[0][2];
		mFrustumPlanes[FrustumPlanes::RIGHT].d = combo[3][3] - combo[0][3];

		mFrustumPlanes[FrustumPlanes::TOP].normal.x = combo[3][0] - combo[1][0];
		mFrustumPlanes[FrustumPlanes::TOP].normal.y = combo[3][1] - combo[1][1];
		mFrustumPlanes[FrustumPlanes::TOP].normal.z = combo[3][2] - combo[1][2];
		mFrustumPlanes[FrustumPlanes::TOP].d = combo[3][3] - combo[1][3];

		mFrustumPlanes[FrustumPlanes::BOTTOM].normal.x = combo[3][0] + combo[1][0];
		mFrustumPlanes[FrustumPlanes::BOTTOM].normal.y = combo[3][1] + combo[1][1];
		mFrustumPlanes[FrustumPlanes::BOTTOM].normal.z = combo[3][2] + combo[1][2];
		mFrustumPlanes[FrustumPlanes::BOTTOM].d = combo[3][3] + combo[1][3];

		mFrustumPlanes[FrustumPlanes::NEAR].normal.x = combo[3][0] + combo[2][0];
		mFrustumPlanes[FrustumPlanes::NEAR].normal.y = combo[3][1] + combo[2][1];
		mFrustumPlanes[FrustumPlanes::NEAR].normal.z = combo[3][2] + combo[2][2];
		mFrustumPlanes[FrustumPlanes::NEAR].d = combo[3][3] + combo[2][3];

		mFrustumPlanes[FrustumPlanes::FAR].normal.x = combo[3][0] - combo[2][0];
		mFrustumPlanes[FrustumPlanes::FAR].normal.y = combo[3][1] - combo[2][1];
		mFrustumPlanes[FrustumPlanes::FAR].normal.z = combo[3][2] - combo[2][2];
		mFrustumPlanes[FrustumPlanes::FAR].d = combo[3][3] - combo[2][3];

		// Renormalise any normals which were not unit length
		for(int i=0; i<6; i++ ) 
		{
			f32 length = mFrustumPlanes[i].normal.Normalise();
			mFrustumPlanes[i].d /= length;
		}

		mRecalcFrustumPlanes = false;
	}

	void Frustum::UpdateFrustumPlanes(void) const
	{
		UpdateView();
		UpdateFrustum();

		if (mRecalcFrustumPlanes)
		{
			UpdateFrustumPlanesImpl();
		}
	}

	void Frustum::UpdateWorldSpaceCornersImpl(void) const
	{
		Matrix4 eyeToWorld = mViewMatrix.InverseAffine();

		// Note: Even though we can dealing with general projection matrix here,
		//	   but because it's incompatibly with infinite far plane, thus, we
		//	   still need to working with projection parameters.

		// Calc near plane corners
		f32 nearLeft, nearRight, nearBottom, nearTop;
		CalcProjectionParameters(nearLeft, nearRight, nearBottom, nearTop);

		// Treat infinite fardist as some arbitrary far value
		f32 farDist = (mFarDist == 0) ? 100000 : mFarDist;

		// Calc far palne corners
		f32 radio = mProjType == ProjectionTypes::PERSPECTIVE ? farDist / mNearDist : 1;
		f32 farLeft = nearLeft * radio;
		f32 farRight = nearRight * radio;
		f32 farBottom = nearBottom * radio;
		f32 farTop = nearTop * radio;

		// near
		mWorldSpaceCorners[0] = eyeToWorld.TransformAffine(Vector3(nearRight, nearTop,	-mNearDist));
		mWorldSpaceCorners[1] = eyeToWorld.TransformAffine(Vector3(nearLeft,  nearTop,	-mNearDist));
		mWorldSpaceCorners[2] = eyeToWorld.TransformAffine(Vector3(nearLeft,  nearBottom, -mNearDist));
		mWorldSpaceCorners[3] = eyeToWorld.TransformAffine(Vector3(nearRight, nearBottom, -mNearDist));
		// far
		mWorldSpaceCorners[4] = eyeToWorld.TransformAffine(Vector3(farRight,  farTop,	 -farDist));
		mWorldSpaceCorners[5] = eyeToWorld.TransformAffine(Vector3(farLeft,   farTop,	 -farDist));
		mWorldSpaceCorners[6] = eyeToWorld.TransformAffine(Vector3(farLeft,   farBottom,  -farDist));
		mWorldSpaceCorners[7] = eyeToWorld.TransformAffine(Vector3(farRight,  farBottom,  -farDist));

		mRecalcWorldSpaceCorners = false;
	}

	void Frustum::UpdateWorldSpaceCorners(void) const
	{
		UpdateView();

		if (mRecalcWorldSpaceCorners)
		{
			UpdateWorldSpaceCornersImpl();
		}

	}

	f32 Frustum::GetAspectRatio(void) const
	{
		return mAspect;
	}

	void Frustum::SetAspectRatio(f32 r)
	{
		mAspect = r;
		InvalidateFrustum();
	}

	void Frustum::InvalidateFrustum() const
	{
		mRecalcFrustum = true;
		mRecalcFrustumPlanes = true;
		mRecalcWorldSpaceCorners = true;
		mRecalcVertexData = true;
	}

	void Frustum::InvalidateView() const
	{
		mRecalcView = true;
		mRecalcFrustumPlanes = true;
		mRecalcWorldSpaceCorners = true;
	}

	const Frustum::WorldSpaceCorners& Frustum::GetWorldSpaceCorners(void) const
	{
		UpdateWorldSpaceCorners();

		return mWorldSpaceCorners;
	}

	void Frustum::SetProjectionType(ProjectionType pt)
	{
		mProjType = pt;
		InvalidateFrustum();
	}

	ProjectionType Frustum::GetProjectionType(void) const
	{
		return mProjType;
	}

	const Vector3& Frustum::GetPositionForViewUpdate(void) const
	{
		return mLastParentPosition;
	}

	const Quaternion& Frustum::GetOrientationForViewUpdate(void) const
	{
		return mLastParentOrientation;
	}

	void Frustum::EnableReflection(const Plane& p)
	{
		mReflect = true;
		mReflectPlane = p;
		mReflectMatrix = Maths::BuildReflectionMatrix(p);
		InvalidateView();

	}

	void Frustum::DisableReflection(void)
	{
		mReflect = false;
		InvalidateView();
	}

	bool Frustum::ProjectSphere(const Sphere& sphere, f32* left, f32* top, f32* right, f32* bottom) const
	{
		// See http://www.gamasutra.com/features/20021011/lengyel_06.htm
		// Transform light position into camera space

		UpdateView();
		Vector3 eyeSpacePos = mViewMatrix.TransformAffine(sphere.GetCenter());

		// initialise
		*left = *bottom = -1.0f;
		*right = *top = 1.0f;

		if (eyeSpacePos.z < 0)
		{
			UpdateFrustum();
			const Matrix4& projMatrix = GetProjectionMatrix();
			f32 r = sphere.GetRadius();
			f32 rsq = r * r;

			// early-exit
			if (eyeSpacePos.LengthSquared() <= rsq)
				return false;

			f32 Lxz = Maths::Sqr(eyeSpacePos.x) + Maths::Sqr(eyeSpacePos.z);
			f32 Lyz = Maths::Sqr(eyeSpacePos.y) + Maths::Sqr(eyeSpacePos.z);

			// Find the tangent planes to the sphere
			// XZ first
			// calculate quadratic discriminant: b*b - 4ac
			// x = Nx
			// a = Lx^2 + Lz^2
			// b = -2rLx
			// c = r^2 - Lz^2
			f32 a = Lxz;
			f32 b = -2.0f * r * eyeSpacePos.x;
			f32 c = rsq - Maths::Sqr(eyeSpacePos.z);
			f32 D = b*b - 4.0f*a*c;

			// two roots?
			if (D > 0)
			{
				f32 sqrootD = Maths::Sqrt(D);
				// solve the quadratic to get the components of the normal
				f32 Nx0 = (-b + sqrootD) / (2 * a);
				f32 Nx1 = (-b - sqrootD) / (2 * a);
				
				// Derive Z from this
				f32 Nz0 = (r - Nx0 * eyeSpacePos.x) / eyeSpacePos.z;
				f32 Nz1 = (r - Nx1 * eyeSpacePos.x) / eyeSpacePos.z;

				// Get the point of tangency
				// Only consider points of tangency in front of the camera
				f32 Pz0 = (Lxz - rsq) / (eyeSpacePos.z - ((Nz0 / Nx0) * eyeSpacePos.x));
				if (Pz0 < 0)
				{
					// Project point onto near plane in worldspace
					f32 nearx0 = (Nz0 * mNearDist) / Nx0;
					// now we need to map this to viewport coords
					// use projection matrix since that will take into account all factors
					Vector3 relx0 = projMatrix * Vector3(nearx0, 0, -mNearDist);

					// find out whether this is a left side or right side
					f32 Px0 = -(Pz0 * Nz0) / Nx0;
					if (Px0 > eyeSpacePos.x)
					{
						*right = std::min(*right, relx0.x);
					}
					else
					{
						*left = std::max(*left, relx0.x);
					}
				}
				f32 Pz1 = (Lxz - rsq) / (eyeSpacePos.z - ((Nz1 / Nx1) * eyeSpacePos.x));
				if (Pz1 < 0)
				{
					// Project point onto near plane in worldspace
					f32 nearx1 = (Nz1 * mNearDist) / Nx1;
					// now we need to map this to viewport coords
					// use projection matrix since that will take into account all factors
					Vector3 relx1 = projMatrix * Vector3(nearx1, 0, -mNearDist);

					// find out whether this is a left side or right side
					f32 Px1 = -(Pz1 * Nz1) / Nx1;
					if (Px1 > eyeSpacePos.x)
					{
						*right = std::min(*right, relx1.x);
					}
					else
					{
						*left = std::max(*left, relx1.x);
					}
				}
			}

			// Now YZ 
			// calculate quadratic discriminant: b*b - 4ac
			// x = Ny
			// a = Ly^2 + Lz^2
			// b = -2rLy
			// c = r^2 - Lz^2
			a = Lyz;
			b = -2.0f * r * eyeSpacePos.y;
			c = rsq - Maths::Sqr(eyeSpacePos.z);
			D = b*b - 4.0f*a*c;

			// two roots?
			if (D > 0)
			{
				f32 sqrootD = Maths::Sqrt(D);
				// solve the quadratic to get the components of the normal
				f32 Ny0 = (-b + sqrootD) / (2 * a);
				f32 Ny1 = (-b - sqrootD) / (2 * a);

				// Derive Z from this
				f32 Nz0 = (r - Ny0 * eyeSpacePos.y) / eyeSpacePos.z;
				f32 Nz1 = (r - Ny1 * eyeSpacePos.y) / eyeSpacePos.z;

				// Get the point of tangency
				// Only consider points of tangency in front of the camera
				f32 Pz0 = (Lyz - rsq) / (eyeSpacePos.z - ((Nz0 / Ny0) * eyeSpacePos.y));
				if (Pz0 < 0)
				{
					// Project point onto near plane in worldspace
					f32 neary0 = (Nz0 * mNearDist) / Ny0;
					// now we need to map this to viewport coords
					// use projection matriy since that will take into account all factors
					Vector3 rely0 = projMatrix * Vector3(0, neary0, -mNearDist);

					// find out whether this is a top side or bottom side
					f32 Py0 = -(Pz0 * Nz0) / Ny0;
					if (Py0 > eyeSpacePos.y)
					{
						*top = std::min(*top, rely0.y);
					}
					else
					{
						*bottom = std::max(*bottom, rely0.y);
					}
				}
				f32 Pz1 = (Lyz - rsq) / (eyeSpacePos.z - ((Nz1 / Ny1) * eyeSpacePos.y));
				if (Pz1 < 0)
				{
					// Project point onto near plane in worldspace
					f32 neary1 = (Nz1 * mNearDist) / Ny1;
					// now we need to map this to viewport coords
					// use projection matriy since that will take into account all factors
					Vector3 rely1 = projMatrix * Vector3(0, neary1, -mNearDist);

					// find out whether this is a top side or bottom side
					f32 Py1 = -(Pz1 * Nz1) / Ny1;
					if (Py1 > eyeSpacePos.y)
					{
						*top = std::min(*top, rely1.y);
					}
					else
					{
						*bottom = std::max(*bottom, rely1.y);
					}
				}
			}
		}

		return (*left != -1.0f) || (*top != 1.0f) || (*right != 1.0f) || (*bottom != -1.0f);
	}

	void Frustum::EnableCustomNearClipPlane(const Plane& plane)
	{
		mObliqueDepthProjection = true;
		mObliqueProjPlane = plane;
		InvalidateFrustum();
	}

	void Frustum::DisableCustomNearClipPlane(void)
	{
		mObliqueDepthProjection = false;
		InvalidateFrustum();
	}

	void Frustum::SetCustomViewMatrix(bool enable, const Matrix4& viewMatrix)
	{
		mCustomViewMatrix = enable;
		if (enable)
		{
			assert(viewMatrix.IsAffine());
			mViewMatrix = viewMatrix;
		}
		InvalidateView();
	}

	void Frustum::SetCustomProjectionMatrix(bool enable, const Matrix4& projMatrix)
	{
		mCustomProjMatrix = enable;
		if (enable)
		{
			mProjMatrix = projMatrix;
		}
		InvalidateFrustum();
	}

	void Frustum::SetOrthoWindow(f32 w, f32 h)
	{
		mOrthoHeight = h;
		mAspect = w / h;
		InvalidateFrustum();
	}

	void Frustum::SetOrthoWindowHeight(f32 h)
	{
		mOrthoHeight = h;
		InvalidateFrustum();
	}

	void Frustum::SetOrthoWindowWidth(f32 w)
	{
		mOrthoHeight = w / mAspect;
		InvalidateFrustum();
	}

	f32 Frustum::GetOrthoWindowHeight() const
	{
		return mOrthoHeight;
	}

	f32 Frustum::GetOrthoWindowWidth() const
	{
		return mOrthoHeight * mAspect;	
	}

	void Frustum::SetFrustumExtents(f32 left, f32 right, f32 top, f32 bottom)
	{
		mFrustumExtentsManuallySet = true;
		mLeft = left;
		mRight = right;
		mTop = top;
		mBottom = bottom;

		InvalidateFrustum();
	}

	void Frustum::ResetFrustumExtents()
	{
		mFrustumExtentsManuallySet = false;
		InvalidateFrustum();
	}

	void Frustum::GetFrustumExtents(f32& outleft, f32& outright, f32& outtop, f32& outbottom) const
	{
		UpdateFrustum();
		outleft = mLeft;
		outright = mRight;
		outtop = mTop;
		outbottom = mBottom;
	}

	void Frustum::SetOrientationMode(OrientationMode orientationMode)
	{
		mOrientationMode = orientationMode;
		InvalidateFrustum();
	}
	

	OrientationMode Frustum::GetOrientationMode() const
	{
		return mOrientationMode;
	}
}
