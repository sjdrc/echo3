#ifndef _ECHOFRUSTUM_H_
#define _ECHOFRUSTUM_H_

#include <echo/Maths/AxisAlignedBox.h>
#include <echo/Maths/Vector2.h>
#include <string>

#undef FAR
#undef NEAR

namespace Echo
{
	class SubMesh;
	
	/** \addtogroup Math
	*  @{
	*/
	/** Specifies orientation mode.
	*/
	struct OrientationModes
	{
		enum _
		{
			DEGREE_0	   = 0,
			DEGREE_90	  = 1,
			DEGREE_180	 = 2,
			DEGREE_270	 = 3,
			PORTRAIT	   = DEGREE_0,
			LANDSCAPERIGHT = DEGREE_90,
			LANDSCAPELEFT  = DEGREE_270
		};
	};
	typedef OrientationModes::_ OrientationMode;

	/** Specifies perspective (realistic) or orthographic (architectural) projection.
	*/
	struct ProjectionTypes
	{
		enum _
		{
			ORTHOGRAPHIC,
			PERSPECTIVE
		};
	};
	typedef ProjectionTypes::_ ProjectionType;

	/** Worldspace clipping planes.
	*/
	struct FrustumPlanes
	{
		enum _
		{
			NEAR   = 0,
			FAR	= 1,
			LEFT   = 2,
			RIGHT  = 3,
			TOP	= 4,
			BOTTOM = 5
		};
	};
	typedef FrustumPlanes::_ FrustumPlane;

	/** A frustum represents a pyramid, capped at the near and far end which is
		used to represent either a visible area or a projection area. Can be used
		for a number of applications.
	*/
	class Frustum
	{
	protected:
		/// Orthographic or perspective?
		ProjectionType mProjType;

		/// y-direction field-of-view (default 45)
		Radian mFOVy;
		/// Far clip distance - default 10000
		f32 mFarDist;
		/// Near clip distance - default 100
		f32 mNearDist;
		/// x/y viewport ratio - default 1.3333
		f32 mAspect;
		/// Ortho height size (world units)
		f32 mOrthoHeight;
		/// Off-axis frustum center offset - default (0.0, 0.0)
		Vector2 mFrustumOffset;
		/// Focal length of frustum (for stereo rendering, defaults to 1.0)
		f32 mFocalLength;

		/// The 6 main clipping planes
		mutable Plane mFrustumPlanes[6];

		/// Stored versions of parent orientation / position
		mutable Quaternion mLastParentOrientation;
		mutable Vector3 mLastParentPosition;

		/// Pre-calced standard projection matrix
		mutable Matrix4 mProjMatrix;
		/// Pre-calced view matrix
		mutable Matrix4 mViewMatrix;
		/// Something's changed in the frustum shape?
		mutable bool mRecalcFrustum;
		/// Something re the view pos has changed
		mutable bool mRecalcView;
		/// Something re the frustum planes has changed
		mutable bool mRecalcFrustumPlanes;
		/// Something re the world space corners has changed
		mutable bool mRecalcWorldSpaceCorners;
		/// Something re the vertex data has changed
		mutable bool mRecalcVertexData;
		/// Are we using a custom view matrix?
		bool mCustomViewMatrix;
		/// Are we using a custom projection matrix?
		bool mCustomProjMatrix;
		/// Have the frustum extents been manually set?
		bool mFrustumExtentsManuallySet;
		/// Frustum extents
		mutable f32 mLeft, mRight, mTop, mBottom;
		/// Frustum orientation mode
		mutable OrientationMode mOrientationMode;
		
		// Internal functions for calcs
		virtual void CalcProjectionParameters(f32& left, f32& right, f32& bottom, f32& top) const;
		/// Update frustum if out of date
		virtual void UpdateFrustum(void) const;
		/// Update view if out of date
		virtual void UpdateView(void) const;
		/// Implementation of updateFrustum (called if out of date)
		virtual void UpdateFrustumImpl(void) const;
		/// Implementation of updateView (called if out of date)
		virtual void UpdateViewImpl(void) const;
		virtual void UpdateFrustumPlanes(void) const;
		/// Implementation of updateFrustumPlanes (called if out of date)
		virtual void UpdateFrustumPlanesImpl(void) const;
		virtual void UpdateWorldSpaceCorners(void) const;
		/// Implementation of updateWorldSpaceCorners (called if out of date)
		virtual void UpdateWorldSpaceCornersImpl(void) const;

		virtual bool IsViewOutOfDate(void) const;
		virtual bool IsFrustumOutOfDate(void) const;
		/// Signal to update frustum information.
		virtual void InvalidateFrustum(void) const;
		/// Signal to update view information.
		virtual void InvalidateView(void) const;

		mutable AxisAlignedBox mBoundingBox;

		mutable Vector3 mWorldSpaceCorners[8];

		/// Is this frustum to act as a reflection of itself?
		bool mReflect;
		/// Derived reflection matrix
		mutable Matrix4 mReflectMatrix;
		/// Fixed reflection plane
		mutable Plane mReflectPlane;
		
		/// Is this frustum using an oblique depth projection?
		bool mObliqueDepthProjection;
		/// Fixed oblique projection plane
		mutable Plane mObliqueProjPlane;
	public:

		Frustum();
		virtual ~Frustum();
		/** Sets the Y-dimension Field Of View (FOV) of the frustum.
			@remarks
				Field Of View (FOV) is the angle made between the frustum's position, and the edges
				of the 'screen' onto which the scene is projected. High values (90+ degrees) result in a wide-angle,
				fish-eye kind of view, low values (30- degrees) in a stretched, telescopic kind of view. Typical values
				are between 45 and 60 degrees.
			@par
				This value represents the VERTICAL field-of-view. The horizontal field of view is calculated from
				this depending on the dimensions of the viewport (they will only be the same if the viewport is square).
			@note
				Setting the FOV overrides the value supplied for frustum::setNearClipPlane.
		 */
		virtual void SetFOVy(const Radian& fovy);

		/** Retrieves the frustum's Y-dimension Field Of View (FOV).
		*/
		virtual const Radian& GetFOVy(void) const;
		
		/**
		 * Set the horizontal FOV.
		 * @note This is a helper method to set the vertical FOV (SetFOVy) relative to the desired horizontal FOV. The
		 * vertical FOV is calculated based on the aspect ratio.
		 * @note Setting the horizontal FOV then changing the aspect ratio does not maintain the horizontal FOV, instead the vertical FOV is maintained.
		 * @param fov
		 */
		void SetFOV(const Radian& fov);

		/**
		 * Get the horizontal FOV.
		 */
		Radian GetFOV() const;

		/** Sets the position of the near clipping plane.
			@remarks
				The position of the near clipping plane is the distance from the frustums position to the screen
				on which the world is projected. The near plane distance, combined with the field-of-view and the
				aspect ratio, determines the size of the viewport through which the world is viewed (in world
				co-ordinates). Note that this world viewport is different to a screen viewport, which has it's
				dimensions expressed in pixels. The frustums viewport should have the same aspect ratio as the
				screen viewport it renders into to avoid distortion.
			@param
				near The distance to the near clipping plane from the frustum in world coordinates.
		 */
		virtual void SetNearPlane(f32 nearDist);

		/** Sets the position of the near clipping plane.
		*/
		virtual f32 GetNearPlane(void) const;

		/** Sets the distance to the far clipping plane.
			@remarks
				The view frustum is a pyramid created from the frustum position and the edges of the viewport.
				This method sets the distance for the far end of that pyramid. 
				Different applications need different values: e.g. a flight sim
				needs a much further far clipping plane than a first-person 
				shooter. An important point here is that the larger the ratio 
				between near and far clipping planes, the lower the accuracy of
				the Z-buffer used to depth-cue pixels. This is because the
				Z-range is limited to the size of the Z buffer (16 or 32-bit) 
				and the max values must be spread over the gap between near and
				far clip planes. As it happens, you can affect the accuracy far 
				more by altering the near distance rather than the far distance, 
				but keep this in mind.
			@param
				far The distance to the far clipping plane from the frustum in 
				world coordinates.If you specify 0, this means an infinite view
				distance which is useful especially when projecting shadows; but
				be careful not to use a near distance too close.
		*/
		virtual void SetFarPlane(f32 farDist);

		/** Retrieves the distance from the frustum to the far clipping plane.
		*/
		virtual f32 GetFarPlane(void) const;

		/** Sets the aspect ratio for the frustum viewport.
			@remarks
				The ratio between the x and y dimensions of the rectangular area visible through the frustum
				is known as aspect ratio: aspect = width / height .
			@par
				The default for most fullscreen windows is 1.3333 - this is also assumed by Ogre unless you
				use this method to state otherwise.
		*/
		virtual void SetAspectRatio(f32 ratio);

		/** Retreives the current aspect ratio.
		*/
		virtual f32 GetAspectRatio(void) const;

		/** Sets frustum offsets, used in stereo rendering.
			@remarks
				You can set both horizontal and vertical plane offsets of "eye"; in
				stereo rendering frustum is moved in horizontal plane. To be able to
				render from two "eyes" you'll need two cameras rendering on two
				RenderTargets.
			@par
				The frustum offsets is in world coordinates, and default to (0, 0) - no offsets.
			@param
				offset The horizontal and vertical plane offsets.
		*/
		virtual void SetFrustumOffset(const Vector2& offset);

		/** Sets frustum offsets, used in stereo rendering.
			@remarks
				You can set both horizontal and vertical plane offsets of "eye"; in
				stereo rendering frustum is moved in horizontal plane. To be able to
				render from two "eyes" you'll need two cameras rendering on two
				RenderTargets.
			@par
				The frustum offsets is in world coordinates, and default to (0, 0) - no offsets.
			@param
				horizontal The horizontal plane offset.
			@param
				vertical The vertical plane offset.
		*/
		virtual void SetFrustumOffset(f32 horizontal = 0.0, f32 vertical = 0.0);

		/** Retrieves the frustum offsets.
		*/
		virtual const Vector2& GetFrustumOffset() const;

		/** Sets frustum focal length (used in stereo rendering).
			@param
				focalLength The distance to the focal plane from the frustum in world coordinates.
		*/
		virtual void SetFocalLength(f32 focalLength = 1.0);

		/** Returns focal length of frustum.
		*/
		virtual f32 GetFocalLength() const;

		/** Manually set the extents of the frustum.
		@param left, right, top, bottom The position where the side clip planes intersect
			the near clip plane, in eye space
		*/
		virtual void SetFrustumExtents(f32 left, f32 right, f32 top, f32 bottom);
		/** Reset the frustum extents to be automatically derived from other params. */
		virtual void ResetFrustumExtents(); 
		/** Get the extents of the frustum in view space. */
		virtual void GetFrustumExtents(f32& outleft, f32& outright, f32& outtop, f32& outbottom) const;

		virtual const Matrix4& GetProjectionMatrix(void) const;

		/**
		 * Get the vertex data that represents the frustum planes.
         * @param targetSubMesh The mesh that will contain the mesh.
         * @param updateOnlyIfChangedSinceLastAcquire If true will only update the mesh if the
		 * frustum has changed since the last time this method was called.
         * @param clearMesh If the mesh will be updated, this flag determines if the mesh
		 * will be cleared before the vertex data is set. This probably only going to be false if
		 * a child class wants to add extra vertices before calling this method.
         */
		virtual void GetVertexData(shared_ptr<SubMesh> targetSubMesh, bool updateOnlyIfChangedSinceLastAcquire = false, bool clearMesh = true) const;
		
		/** Gets the view matrix for this frustum.
		*/
		virtual const Matrix4& GetViewMatrix(void) const;

		/** Calculate a view matrix for this frustum, relative to a potentially dynamic point. 
			Mainly for used when using camera-relative rendering
			for frustums that are not the centre (e.g. texture projection)
		*/
		virtual void CalcViewMatrixRelative(const Vector3& relPos, Matrix4& matToUpdate) const;

		/** Set whether to use a custom view matrix on this frustum.
		@remarks
			This is an advanced method which allows you to manually set
			the view matrix on this frustum, rather than having it calculate
			itself based on it's position and orientation. 
		@note
			After enabling a custom view matrix, the frustum will no longer
			update on its own based on position / orientation changes. You 
			are completely responsible for keeping the view matrix up to date.
			The custom matrix will be returned from getViewMatrix.
		@param enable If true, the custom view matrix passed as the second 
			parameter will be used in preference to an auto calculated one. If
			false, the frustum will revert to auto calculating the view matrix.
		@param viewMatrix The custom view matrix to use, the matrix must be an
			affine matrix.
		@see Frustum::setCustomProjectionMatrix, EMatrix4::IsAffine
		*/
		virtual void SetCustomViewMatrix(bool enable, 
			const Matrix4& viewMatrix = Matrix4::IDENTITY);
		/// Returns whether a custom view matrix is in use
		virtual bool IsCustomViewMatrixEnabled(void) const 
		{ return mCustomViewMatrix; }
		
		/** Set whether to use a custom projection matrix on this frustum.
		@remarks
			This is an advanced method which allows you to manually set
			the projection matrix on this frustum, rather than having it 
			calculate itself based on it's position and orientation. 
		@note
			After enabling a custom projection matrix, the frustum will no 
			longer update on its own based on field of view and near / far
			distance changes. You are completely responsible for keeping the 
			projection matrix up to date if those values change. The custom 
			matrix will be returned from getProjectionMatrix and derivative
			functions.
		@param enable If true, the custom projection matrix passed as the 
			second parameter will be used in preference to an auto calculated 
			one. If	false, the frustum will revert to auto calculating the 
			projection matrix.
		@param projectionMatrix The custom view matrix to use
		@see Frustum::setCustomViewMatrix
		*/
		virtual void SetCustomProjectionMatrix(bool enable, 
			const Matrix4& projectionMatrix = Matrix4::IDENTITY);
		/// Returns whether a custom projection matrix is in use
		virtual bool IsCustomProjectionMatrixEnabled(void) const
		{ return mCustomProjMatrix; }

		/** Retrieves the clipping planes of the frustum (world space).
		@remarks
			The clipping planes are ordered as declared in enumerate constants FrustumPlane.
		*/
		typedef Plane FrustumPlaneArray[6];
		virtual const FrustumPlaneArray& GetFrustumPlanes(void) const;

		/** Retrieves a specified plane of the frustum (world space).
			@remarks
				Gets a reference to one of the planes which make up the frustum frustum, e.g. for clipping purposes.
		*/
		virtual const Plane& GetFrustumPlane( unsigned short plane ) const;

		/** Tests whether the given container is visible in the Frustum.
			@param
				bound Bounding box to be checked (world space)
			@param
				culledBy Optional pointer to an int which will be filled by the plane number which culled
				the box if the result was false;
			@returns
				If the box was visible, true is returned.
			@par
				Otherwise, false is returned.
		*/
		virtual bool IsVisible(const AxisAlignedBox& bound, FrustumPlane* culledBy = 0) const;

		/** Tests whether the given container is visible in the Frustum.
			@param
				bound Bounding sphere to be checked (world space)
			@param
				culledBy Optional pointer to an int which will be filled by the plane number which culled
				the box if the result was false;
			@returns
				If the sphere was visible, true is returned.
			@par
				Otherwise, false is returned.
		*/
		virtual bool IsVisible(const Sphere& bound, FrustumPlane* culledBy = 0) const;

		/** Tests whether the given vertex is visible in the Frustum.
			@param
				vert Vertex to be checked (world space)
			@param
				culledBy Optional pointer to an int which will be filled by the plane number which culled
				the box if the result was false;
			@returns
				If the box was visible, true is returned.
			@par
				Otherwise, false is returned.
		*/
		virtual bool IsVisible(const Vector3& vert, FrustumPlane* culledBy = 0) const;

		/** Gets the world space corners of the frustum.
		@remarks
			The corners are ordered as follows:
			- top-right near,
			- top-left near,
			- bottom-left near,
			- bottom-right near,
			- top-right far,
			- top-left far,
			- bottom-left far,
			- bottom-right far.
		*/
		typedef Vector3 WorldSpaceCorners[8];
		virtual const WorldSpaceCorners& GetWorldSpaceCorners(void) const;

		/** Sets the type of projection to use (orthographic or perspective). Default is perspective.
		*/
		virtual void SetProjectionType(ProjectionType pt);

		/** Retrieves info on the type of projection used (orthographic or perspective).
		*/
		virtual ProjectionType GetProjectionType(void) const;

		/** Sets the orthographic window settings, for use with orthographic rendering only. 
		@note Calling this method will recalculate the aspect ratio, use 
			setOrthoWindowHeight or setOrthoWindowWidth alone if you wish to 
			preserve the aspect ratio but just fit one or other dimension to a 
			particular size.
		@param w, h The dimensions of the view window in world units
		*/
		virtual void SetOrthoWindow(f32 w, f32 h);
		/** Sets the orthographic window height, for use with orthographic rendering only. 
		@note The width of the window will be calculated from the aspect ratio. 
		@param h The height of the view window in world units
		*/
		virtual void SetOrthoWindowHeight(f32 h);
		/** Sets the orthographic window width, for use with orthographic rendering only. 
		@note The height of the window will be calculated from the aspect ratio. 
		@param w The width of the view window in world units
		*/
		virtual void SetOrthoWindowWidth(f32 w);
		/** Gets the orthographic window height, for use with orthographic rendering only. 
		*/
		virtual f32 GetOrthoWindowHeight() const;
		/** Gets the orthographic window width, for use with orthographic rendering only. 
		@note This is calculated from the orthographic height and the aspect ratio
		*/
		virtual f32 GetOrthoWindowWidth() const;

		/** Modifies this frustum so it always renders from the reflection of itself through the
		plane specified.
		@remarks
		This is obviously useful for performing planar reflections. 
		*/
		virtual void EnableReflection(const Plane& p);

		/** Disables reflection modification previously turned on with enableReflection */
		virtual void DisableReflection(void);

		/// Returns whether this frustum is being reflected
		virtual bool IsReflected(void) const { return mReflect; }
		/// Returns the reflection matrix of the frustum if appropriate
		virtual const Matrix4& GetReflectionMatrix(void) const { return mReflectMatrix; }
		/// Returns the reflection plane of the frustum if appropriate
		virtual const Plane& GetReflectionPlane(void) const { return mReflectPlane; }

		/** Project a sphere onto the near plane and get the bounding rectangle. 
		@param sphere The world-space sphere to project
		@param radius Radius of the sphere
		@param left, top, right, bottom Pointers to destination values, these
			will be completed with the normalised device coordinates (in the 
			range {-1,1})
		@returns true if the sphere was projected to a subset of the near plane,
			false if the entire near plane was contained
		*/
		virtual bool ProjectSphere(const Sphere& sphere, f32* left, f32* top, f32* right, f32* bottom) const;

		/** Links the frustum to a custom near clip plane, which can be used
			to clip geometry in a custom manner without using user clip planes.
		@remarks
			There are several applications for clipping a scene arbitrarily by
			a single plane; the most common is when rendering a reflection to 
			a texture, and you only want to render geometry that is above the 
			water plane (to do otherwise results in artefacts). Whilst it is
			possible to use user clip planes, they are not supported on all
			cards, and sometimes are not hardware accelerated when they are
			available. Instead, where a single clip plane is involved, this
			technique uses a 'fudging' of the near clip plane, which is 
			available and fast on all hardware, to perform as the arbitrary
			clip plane. This does change the shape of the frustum, leading 
			to some depth buffer loss of precision, but for many of the uses of
			this technique that is not an issue.
		@par 
			This version of the method links to a plane, rather than requiring
			a by-value plane definition, and therefore you can 
			make changes to the plane (e.g. by moving / rotating the node it is
			attached to) and they will automatically affect this object.
		@note This technique only works for perspective projection.
		@param plane The plane to link to to perform the clipping. This plane
			must continue to exist while the camera is linked to it; do not
			destroy it before the frustum. 
		*/
		//virtual void enableCustomNearClipPlane(const MovablePlane* plane);
		/** Links the frustum to a custom near clip plane, which can be used
			to clip geometry in a custom manner without using user clip planes.
		@remarks
			There are several applications for clipping a scene arbitrarily by
			a single plane; the most common is when rendering a reflection to  
			a texture, and you only want to render geometry that is above the 
			water plane (to do otherwise results in artefacts). Whilst it is
			possible to use user clip planes, they are not supported on all
			cards, and sometimes are not hardware accelerated when they are
			available. Instead, where a single clip plane is involved, this
			technique uses a 'fudging' of the near clip plane, which is 
			available and fast on all hardware, to perform as the arbitrary
			clip plane. This does change the shape of the frustum, leading 
			to some depth buffer loss of precision, but for many of the uses of
			this technique that is not an issue.
		@note This technique only works for perspective projection.
		@param plane The plane to link to to perform the clipping. This plane
			must continue to exist while the camera is linked to it; do not
			destroy it before the frustum. 
		*/
		virtual void EnableCustomNearClipPlane(const Plane& plane);
		/** Disables any custom near clip plane. */
		virtual void DisableCustomNearClipPlane(void);
		/** Is a custom near clip plane in use? */
		virtual bool IsCustomNearClipPlaneEnabled(void) const 
		{ return mObliqueDepthProjection; }

		/// Small constant used to reduce far plane projection to avoid inaccuracies
		static const f32 INFINITE_FAR_PLANE_ADJUST;

		/** Get the derived position of this frustum. */
		virtual const Vector3& GetPositionForViewUpdate(void) const;
		/** Get the derived orientation of this frustum. */
		virtual const Quaternion& GetOrientationForViewUpdate(void) const;

		/** Gets a world-space list of planes enclosing the frustum.
		*/
		//PlaneBoundedVolume getPlaneBoundedVolume();

		/** Set the orientation mode of the frustum. Default is DEGREE_0
			 @remarks
				Setting the orientation of a frustum is only supported on
				iPhone at this time.  An exception is thrown on other platforms.
		*/
		void SetOrientationMode(OrientationMode orientationMode);

		/** Get the orientation mode of the frustum.
			 @remarks
				Getting the orientation of a frustum is only supported on
				iPhone at this time.  An exception is thrown on other platforms.
		*/
		OrientationMode GetOrientationMode() const;

	};
}
#endif 
