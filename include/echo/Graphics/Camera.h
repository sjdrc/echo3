#ifndef _ECHOCAMERA_H_
#define _ECHOCAMERA_H_

#include <echo/Maths/Matrix4.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Plane.h>
#include <echo/Maths/Quaternion.h>
#include <echo/Maths/Ray.h>
#include <echo/Maths/PlaneBoundedVolume.h>
#include <echo/Graphics/Frustum.h>
#include <echo/Graphics/Node.h>

namespace Echo
{
	class Viewport;
	class Scene;
	class RenderTarget;

	/** A camera represents the viewing object from which the scene will be rendered.
		@remarks
			Echo renders scenes from a camera viewpoint into a buffer of
			some sort, normally a window or a texture (a subclass of
			RenderTarget). Echo cameras support both perspective projection (the default,
			meaning objects get smaller the further away they are) and
			orthographic projection (blueprint-style, no decrease in size
			with distance). Each camera carries with it a field of view,
			rendering distances, projection etc, allowing you to use Echo to create
			complex multi-window views if required. In addition, more than
			one camera can point at a single render target if required,
			each rendering to a subset of the target, allowing split screen
			and picture-in-picture views.
		@par
			Cameras maintain their own aspect ratios, field of view, and frustum,
			and project co-ordinates into a space measured from -1 to 1 in x and y,
			and 0 to 1 in z. At render time, the camera will be rendering to a
			Viewport which will translate these parametric co-ordinates into real screen
			co-ordinates. Obviously it is advisable that the viewport has the same
			aspect ratio as the camera to avoid distortion (unless you want it!).
	 * @see
	 * 	Refer also to CameraDollies.h for classes that automatically position
	 * 	a camera to face and/or track various NodeInterface objects.
	 */
	class Camera : public Frustum, public Node
	{
	public:
		/** Standard constructor.
		 */
		Camera(const std::string& name, Scene& scene);

		/** Standard destructor.
		 */
		virtual ~Camera();

		/** Returns a pointer to the SceneManager this camera is rendering through.
		 */
		Scene& GetScene(void) const;

		/** Gets the camera's name.
		 */
		virtual const std::string& GetName(void) const;

		/** Sets the camera's position.
		 */
		void SetPosition(f32 x, f32 y, f32 z) override;

		/** Sets the camera's position.
		 */
		void SetPosition(const Vector3& vec) override;

		/** Moves the camera's position by the vector offset provided along world axes.
		 */
		void Move(const Vector3& vec);

		/** Moves the camera's position by the vector offset provided along its own axes (relative to orientation).
		 */
		void MoveRelative(const Vector3& vec);

		/** Sets the camera's direction vector.
			@remarks
				Note that the 'up' vector for the camera will automatically be recalculated based on the
				current 'up' vector (i.e. the Roll will remain the same).
		 */
		void SetDirection(f32 x, f32 y, f32 z);

		/** Sets the camera's direction vector.
		 */
		void SetDirection(const Vector3& vec);

		/* Gets the camera's direction.
		 */
		Vector3 GetDirection(void) const;

		/** Gets the camera's up vector.
		 */
		Vector3 GetUp(void) const;

		/** Gets the camera's right vector.
		 */
		Vector3 GetRight(void) const;

		/** Points the camera at a location in worldspace.
			@remarks
				This is a helper method to automatically generate the
				direction vector for the camera, based on its current position
				and the supplied look-at point.
			@param
				targetPoint A vector specifying the look at point.
		 */
		void LookAt(const Vector3& targetPoint);

		/** Points the camera at a location in worldspace.
			@remarks
				This is a helper method to automatically generate the
				direction vector for the camera, based on its current position
				and the supplied look-at point.
			@param
				x
			@param
				y
			@param
				z Co-ordinates of the point to look at.
		 */
		void LookAt(f32 x, f32 y, f32 z);
		
		/**
		 * Position the camera at a distance so that the specified height takes up the viewport's vertical entirely.
		 * @param viewHeight The number of vertical units the camera's view should take up.
		 * @param lookAt This position represents the centre point of the viewing plane. The plane is parallel to the
		 * near and far planes.
		 * @param distanceDirection A vector that represents the direction from the lookAt point in which to offset.
		 * @note The yFOV and aspect ratio should be set before calling method. Calling either method after this one
		 * will probably result in an undesirable configuration.
		 * @note This method sets the far plane such that it is twice the viewing distance.
		 */
		void CalculateAndSetDistanceForYFOV(f32 viewHeight, Vector3 lookAt, Vector3 distanceDirection=Vector3(0,0,1))
		{
			// Calculate so that the vertical view takes up viewHeight units.
			Radian halfFOVy = GetFOVy() / 2;
			f32 halfYRes = viewHeight / 2;
			f32 distance = halfYRes / Maths::Tan<f32>(halfFOVy);
			distanceDirection.Normalise();
			SetPosition(lookAt + (distanceDirection*distance));
			LookAt(lookAt);
			SetFarPlane(distance*2);
		}

		/**
		 * Position the camera at a distance so that the specified width takes up the viewport's horizontal entirely.
		 * @param viewHeight The number of horizontal units the camera's view should take up.
		 * @param lookAt This position represents the centre point of the viewing plane. The plane is parallel to the
		 * near and far planes.
		 * @param distanceDirection A vector that represents the direction from the lookAt point in which to offset.
		 * @note The yFOV and aspect ratio should be set before calling method. Calling either method after this one
		 * will probably result in an undesirable configuration.
		 * @note This method sets the far plane such that it is twice the viewing distance.
		 */
		void CalculateAndSetDistanceForXFOV(f32 viewWidth, Vector3 lookAt, Vector3 distanceDirection=Vector3(0,0,1))
		{
			// Calculate so that the horizontal view takes up viewWidth units.
			Radian halfFOVx = GetFOV() / 2;
			f32 halfXRes = viewWidth / 2;
			f32 distance = halfXRes / Maths::Tan<f32>(halfFOVx);
			distanceDirection.Normalise();
			SetPosition(lookAt + (distanceDirection*distance));
			LookAt(lookAt);
			SetFarPlane(distance*2);
		}

		/** Rolls the camera anticlockwise, around its local z axis.
		 */
		void Roll(const Radian& angle);

		/** Rotates the camera anticlockwise around its local y axis.
		 */
		void Yaw(const Radian& angle);

		/** Pitches the camera up/down anticlockwise around its local z axis.
		 */
		void Pitch(const Radian& angle);

		/** Rotate the camera around an arbitrary axis.
		 */
		void Rotate(const Vector3& axis, const Radian& angle) override;

		/** Rotate the camera around an arbitrary axis using a EQuaternion.
		 */
		void Rotate(const Quaternion& q) override;

		/** Tells the camera whether to Yaw around its own local Y axis or a 
			fixed axis of choice.
			@remarks
				This method allows you to change the Yaw behaviour of the camera
				- by default, the camera yaws around a fixed Y axis. This is 
				often what you want - for example if you're making a first-person 
				shooter, you really don't want the Yaw axis to reflect the local 
				camera Y, because this would mean a different Yaw axis if the 
				player is looking upwards rather than when they are looking
				straight ahead. You can change this behaviour by calling this 
				method, which you will want to do if you are making a completely
				free camera like the kind used in a flight simulator. 
			@param
				useFixed If true, the axis passed in the second parameter will 
				always be the Yaw axis no matter what the camera orientation. 
				If false, the camera yaws around the local Y.
			@param
				fixedAxis The axis to use if the first parameter is true.
		 */
		void SetFixedYawAxis(bool useFixed, const Vector3& fixedAxis = Vector3::UNIT_Y);

		/** Sets the camera's orientation.
		 */
		void SetOrientation(const Quaternion& q) override;

		//!\brief Tells the Camera to render the scene it is in.
		void RenderScene(RenderTarget& renderTarget);

		/** Function for outputting to a stream.
		 */
		friend std::ostream& operator<<(std::ostream& o, const Camera& c);

		/** Gets the derived orientation of the camera, including any
			rotation inherited from a node attachment and reflection matrix. */
		const Quaternion& GetDerivedOrientation(void) const override;

		/** Gets the derived position of the camera, including any
			translation inherited from a node attachment and reflection matrix. */
		const Vector3& GetDerivedPosition(void) const override;

		/** Gets the derived direction vector of the camera, including any
			rotation inherited from a node attachment and reflection matrix. */
		Vector3 GetDerivedDirection(void) const;

		/** Gets the derived up vector of the camera, including any
			rotation inherited from a node attachment and reflection matrix. */
		Vector3 GetDerivedUp(void) const;

		/** Gets the derived right vector of the camera, including any
			rotation inherited from a node attachment and reflection matrix. */
		Vector3 GetDerivedRight(void) const;

		/** Gets the real world orientation of the camera, including any
			rotation inherited from a node attachment */
		const Quaternion& GetRealOrientation(void) const;

		/** Gets the real world position of the camera, including any
			translation inherited from a node attachment. */
		const Vector3& GetRealPosition(void) const;

		/** Gets the real world direction vector of the camera, including any
			rotation inherited from a node attachment. */
		Vector3 GetRealDirection(void) const;

		/** Gets the real world up vector of the camera, including any
			rotation inherited from a node attachment. */
		Vector3 GetRealUp(void) const;

		/** Gets the real world right vector of the camera, including any
			rotation inherited from a node attachment. */
		Vector3 GetRealRight(void) const;
		
		/** Enables / disables automatic tracking of a SceneNode.
		@remarks
			If you enable auto-tracking, this Camera will automatically Rotate to
			look at the target SceneNode every frame, no matter how 
			it or SceneNode Move. This is handy if you want a Camera to be focussed on a
			single object or group of objects. Note that by default the Camera looks at the 
			origin of the SceneNode, if you want to tweak this, e.g. if the object which is
			attached to this target node is quite big and you want to point the camera at
			a specific point on it, provide a vector in the 'offset' parameter and the 
			camera's target point will be adjusted.
		@param enabled If true, the Camera will track the SceneNode supplied as the next 
			parameter (cannot be null). If false the camera will cease tracking and will
			remain in its current orientation.
		@param target Pointer to the NodeInterface which this Camera will track. Can be null if
			and only if the enabled param is false.
		@param offset If supplied, the camera targets this point in local space of the target node
			instead of the origin of the target node. Good for fine tuning the look at point.
		 * @see
		 * 	For more advanced NodeInterface tracking techniques, refer also to
		 * 	CameraDollies.h for classes that automatically position a camera to
		 * 	face and/or track various NodeInterface objects.
		 */
		void SetAutoTracking(bool enabled, shared_ptr<NodeInterface> target = shared_ptr<NodeInterface>(), const Vector3& offset = Vector3::ZERO);

		/** Gets a world space ray as cast from the camera through a viewport position.
		@param screenx, screeny The x and y position at which the ray should intersect the viewport, 
			in normalised screen coordinates [0,1]
		 */
		Ray GetCameraToViewportRay(f32 screenx, f32 screeny) const;

		/** Gets a world space ray as cast from the camera through a viewport position.
		@param screenx, screeny The x and y position at which the ray should intersect the viewport, 
			in normalised screen coordinates [0,1]
		@param outRay ERay instance to populate with result
		 */
		void GetCameraToViewportRay(f32 screenx, f32 screeny, Ray& outRay) const;

		void UpdateAspectForViewport(const Viewport& viewport, const RenderTarget& renderTarget);
		
		/**
		 * Internal method used by Echo to update auto-tracking cameras.
		 * @see SetAutoTracking().
		 */
		void _AutoTrack(void);
		
		/** Sets the viewing window inside of viewport.
		@remarks
		This method can be used to set a subset of the viewport as the rendering
		target. 
		@param Left Relative to Viewport - 0 corresponds to left edge, 1 - to right edge (default - 0).
		@param Top Relative to Viewport - 0 corresponds to top edge, 1 - to bottom edge (default - 0).
		@param Right Relative to Viewport - 0 corresponds to left edge, 1 - to right edge (default - 1).
		@param Bottom Relative to Viewport - 0 corresponds to top edge, 1 - to bottom edge (default - 1).
		 */
		virtual void SetWindow(f32 Left, f32 Top, f32 Right, f32 Bottom);

		/// Cancel view window.
		virtual void ResetWindow(void);

		/// Returns if a viewport window is being used
		virtual bool IsWindowSet(void) const
		{
			return mWindowSet;
		}

		/// Gets the window clip planes, only applicable if isWindowSet == true
		const std::vector<Plane>& GetWindowPlanes(void) const;

		/** Overridden from MovableObject */
		f32 GetBoundingRadius(void) const;

		/** Get the auto tracking target for this camera, if any. */
		shared_ptr<NodeInterface> GetAutoTrackTarget(void) const
		{
			return mAutoTrackTarget;
		}

		/** Get the auto tracking offset for this camera, if it is auto tracking. */
		const Vector3& GetAutoTrackOffset(void) const
		{
			return mAutoTrackOffset;
		}

		/** If set to true a viewport that owns this frustum will be able to 
			recalculate the aspect ratio whenever the frustum is resized.
		@remarks
			You should set this to true only if the frustum / camera is used by 
			one viewport at the same time. Otherwise the aspect ratio for other 
			viewports may be wrong.
		 */
		void SetAutoAspectRatio(bool autoratio);

		/** Retrieves if AutoAspectRatio is currently set or not
		 */
		bool GetAutoAspectRatio(void) const;

		/** Tells the camera to use a separate Frustum instance to perform culling.
		@remarks
			By calling this method, you can tell the camera to perform culling
			against a different frustum to its own. This is mostly useful for
			debug cameras that allow you to show the culling behaviour of another
			camera, or a manual frustum instance. 
		@param frustum Pointer to a frustum to use; this can either be a manual
			Frustum instance (which you can attach to scene nodes like any other
			MovableObject), or another camera. If you pass 0 to this method it
			reverts the camera to normal behaviour.
		 */
		void SetCullingFrustum(Frustum* frustum)
		{
			mCullFrustum = frustum;
		}

		/** Returns the custom culling frustum in use. */
		Frustum* SetCullingFrustum(void) const
		{
			return mCullFrustum;
		}

		/** Forward projects frustum rays to find forward intersection with plane.
		 @remarks
			Forward projection may lead to intersections at infinity.
		 */
		virtual void ForwardIntersect(const Plane& worldPlane, std::vector<Vector4>& intersect3d) const;

		/// @copydoc Frustum::IsVisible
		bool IsVisible(const AxisAlignedBox& bound, FrustumPlane* culledBy = 0) const override;

		/// @copydoc Frustum::IsVisible
		bool IsVisible(const Sphere& bound, FrustumPlane* culledBy = 0) const override;

		/// @copydoc Frustum::IsVisible
		bool IsVisible(const Vector3& vert, FrustumPlane* culledBy = 0) const override;

		/// @copydoc Frustum::GetWorldSpaceCorners
		const WorldSpaceCorners& GetWorldSpaceCorners(void) const override;

		/// @copydoc Frustum::GetFrustumPlane
		const Plane& GetFrustumPlane(unsigned short plane) const override;

		/// @copydoc Frustum::ProjectSphere
		bool ProjectSphere(const Sphere& sphere, f32* left, f32* top, f32* right, f32* bottom) const override;

		/// @copydoc Frustum::GetNearClipDistance
		f32 GetNearPlane(void) const override;

		/// @copydoc Frustum::GetFarClipDistance
		f32 GetFarPlane(void) const override;

		/// @copydoc Frustum::GetViewMatrix
		const Matrix4& GetViewMatrix(void) const override;

		/** Specialised version of GetViewMatrix allowing caller to differentiate
			whether the custom culling frustum should be allowed or not. 
		@remarks
			The default behaviour of the standard GetViewMatrix is to delegate to 
			the alternate culling frustum, if it is set. This is expected when 
			performing CPU calculations, but the final rendering must be performed
			using the real view matrix in order to display the correct debug view.
		 */
		const Matrix4& GetViewMatrix(bool ownFrustumOnly) const;

		/** Set whether this camera should use the 'rendering distance' on
			objects to exclude distant objects from the final image. The
			default behaviour is to use it.
		@param use True to use the rendering distance, false not to.
		 */
		virtual void SetUseRenderingDistance(bool use)
		{
			mUseRenderingDistance = use;
		}

		/** Get whether this camera should use the 'rendering distance' on
			objects to exclude distant objects from the final image.
		 */
		virtual bool GetUseRenderingDistance(void) const
		{
			return mUseRenderingDistance;
		}

		/** Synchronise core camera settings with another. 
		@remarks
			Copies the position, orientation, clip distances, projection type, 
			FOV, focal length and aspect ratio from another camera. Other settings like query flags, 
			reflection etc are preserved.
		 */
		virtual void SynchroniseBaseSettingsWith(const Camera* cam);

		/** Get the derived position of this frustum. */
		const Vector3& GetPositionForViewUpdate(void) const override;

		/** Get the derived orientation of this frustum. */
		const Quaternion& GetOrientationForViewUpdate(void) const override;
	protected:
		/// Camera name
		std::string mName;
		Scene& mScene;

		/// The Derived orientation/position of the camera include reflection

		/// f32 world orientation/position of the camera
		mutable Quaternion mRealOrientation;
		mutable Vector3 mRealPosition;

		/// Whether to Yaw around a fixed axis.
		bool mYawFixed;
		/// Fixed axis to Yaw around
		Vector3 mYawFixedAxis;

		/// Stored number of visible faces in the last render
		unsigned int mVisFacesLastRender;

		/// Stored number of visible faces in the last render
		unsigned int mVisBatchesLastRender;

		/// SceneNode which this Camera will automatically track
		shared_ptr<NodeInterface> mAutoTrackTarget;
		/// Tracking offset for fine tuning
		Vector3 mAutoTrackOffset;

		/** Viewing window. 
		@remarks
		Generalise camera class for the case, when viewing frustum doesn't cover all viewport.
		 */
		f32 mWLeft, mWTop, mWRight, mWBottom;
		/// Is viewing window used.
		bool mWindowSet;
		/// Windowed viewport clip planes 
		mutable std::vector<Plane> mWindowClipPlanes;
		// Was viewing window changed.
		mutable bool mRecalcWindow;

		/** A hint for Renderers to indicate that the aspect ratio should be adjusted when rendering.
		 * Default is to auto adjust the aspect ratio.
		 */
		bool mAutoAspectRatio;
		
		/// Custom culling frustum
		Frustum *mCullFrustum;
		/// Whether or not the rendering distance of objects should take effect for this camera
		bool mUseRenderingDistance;

		// Internal functions for calcs
		bool IsViewOutOfDate(void) const override;
		/// Signal to update frustum information.
		void InvalidateFrustum(void) const override;
		/// Signal to update view information.
		void InvalidateView(void) const override;

		/** Do actual window setting, using parameters set in SetWindow call
		@remarks
			The method will called on demand.
		 */
		virtual void SetWindowImpl(void) const;

		/** Helper function for forwardIntersect that intersects rays with canonical plane */
		virtual std::vector<Vector4> GetRayForwardIntersect(const Vector3& anchor, const Vector3 *dir, f32 planeOffset) const;
	};
}
#endif
