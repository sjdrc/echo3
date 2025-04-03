#ifndef _ECHOBULLETMESHDEBUGDRAWER_H_
#define _ECHOBULLETMESHDEBUGDRAWER_H_

#include <LinearMath/btIDebugDraw.h>
#include <echo/Graphics/Mesh.h>
#include <echo/Kernel/Mutex.h>

namespace Echo
{
	/**
	 * BulletMeshDebugDrawer will draw lines to a SubMesh which can then be
	 * used to render in a Scene.
	 * @note The class only draws lines. Points are not supported.
	 * @note The class clears after every render so if you have multiple viewports you will only see the debug information in the first one.
	 * @
	 */
	class BulletMeshDebugDrawer : public btIDebugDraw, public Mesh
	{
	public:
		BulletMeshDebugDrawer();
		~BulletMeshDebugDrawer();

		/**
		 * Draw a line.
		 * Override from btIDebugDraw.
		 */
		virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& colour) override;
		
		/**
		 * Drawing contact points is currently unsupported.
		 * Override from btIDebugDraw.
		 */
		virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& colour) override;

		/**
		 * Output a warning to stdout.
		 * override from btIDebugDraw.
		 * @param warningString.
		 */
		virtual void reportErrorWarning(const char* warningString) override;
		
		/**
		 * Set the debug draw mode.
		 * DBG_DrawWireframe | DBG_DrawAabb is a common combination. See btIDebugDraw for more.
		 * @note not all debug information will be displayed with this drawer.
		 * override from btIDebugDraw.
		 */
		virtual void setDebugMode(int debugMode) override;

		/**
		 * Get the debug draw mode.
		 * override from btIDebugDraw.
		 * @return 
		 */
		virtual int getDebugMode() const override;

		/**
		 * Drawing 3D text is currently unsupported.
		 * override from btIDebugDraw.
		 */
		virtual void draw3dText(const btVector3& location,const char* textString) override;

		/**
		 * Mesh Render override.
		 * Clears the mesh data after rendering.
		 */
		virtual void Render(RenderContext& renderContext, const Matrix4& world, const Matrix4& worldView, Colour compoundDiffuse) override;
		
		/**
		 * Lock the object to prevent it from rendering.
		 * If you're using different threads for rendering and debug drawing you should lock before debug drawing and unlock when you're done.
		 */
		void Lock();

		/**
		 * Unlock the object to allow it to render.
		 * If you're using different threads for rendering and debug drawing you should lock before debug drawing and unlock when you're done.
		 */
		void Unlock();
	private:
		int mDebugMode;
		Size mNumberOfVertices;
		Size mNumberOfLines;
		bool mJustRendered;
		shared_ptr<SubMesh> mSubMesh;
		shared_ptr<VertexBuffer> mVertexBuffer;
		VertexBuffer::Accessor<Vector3> mVertices;
		VertexBuffer::Accessor<VertexColour> mColours;
		shared_ptr< ElementBuffer > mElementBuffer;
		ElementBuffer::Accessor< ElementBuffer::Line< u32 > > mLines;
		Mutex mMutex;
	};
}
#endif
