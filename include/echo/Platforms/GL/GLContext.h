#include <echo/Platforms/GL/GLSupport.h>

#ifdef ECHO_RENDERER_GL
#ifndef _ECHOGLRENDERCONTEXT_H_
#define _ECHOGLRENDERCONTEXT_H_

#include <echo/Graphics/RenderPass.h>
#include <echo/Util/PseudoAtomicSet.h>
#include <map>

namespace Echo
{
	class Texture;
	class GLTexture;
	class Shader;
	class GLShaderProgram;
	class VertexBuffer;
	class GLVertexBuffer;
	class CubeMapTexture;
	class GLCubeMapTexture;
	
	/**
	 * GLContext is used to store the render state of a GL context.
	 */
	class GLContext
	{
	public:
		GLContext()
		{
			mDepthTestEnabled = false;
			mDepthTestShouldBeDisabled = false;
			mDepthWriteEnabled = false;
			mDepthFunction = RenderPass::DepthFunctions::LESS;
			mNonForceDepthFunction = RenderPass::DepthFunctions::LESS;
			mAlphaTestFunction = RenderPass::AlphaTestFunctions::ALWAYS;

			mLightingEnabled = false;
			mVertexColourEnabled = false;
			mAlphaTestEnabled = false;
			mColourMaterialEnabled = false;
			mVertexArrayEnabled = false;
			mColourArrayEnabled = false;
			mNormalArrayEnabled = false;
			mTextureCoordinateArrayEnabled = 0;
			mMaxTextureStagesUsed = 0;
			mTextureCoordinateArrayIndex = nullptr;
			mTexture2DEnabled = 0;
			mClearDepth = 1.0f;
			mAlphaTestValue = 0.f;
			mContextReady = false;
			mActiveTextureStage = 0;
			mProjectionMatrix=Matrix4::IDENTITY;
			mScissorRectangle = Viewport::Rectangle(0,0,1,1);
			mScissorEnabled = false;
		}
		~GLContext()
		{
			delete [] mTexture2DEnabled;
			delete [] mTextureCoordinateArrayEnabled;
			delete [] mTextureCoordinateArrayIndex;
		}
		RenderPass::DepthFunction mDepthFunction;		//!< Depth funciton used by depth test.
		RenderPass::DepthFunction mNonForceDepthFunction;	//!< Depth write depends on depth test (see GL docs), used to restore depth funciton after enabling for depth write.
		RenderPass::AlphaTestFunction mAlphaTestFunction;
		bool mDepthTestEnabled;					//!< Depth test state.
		bool mDepthTestShouldBeDisabled;			//!< Depth write depends on depth test (see GL docs), we track whether we enabled depth test for depth write.
		bool mDepthWriteEnabled;					//!< Depth write state.
		
		bool mLightingEnabled;
		bool mVertexColourEnabled;
		bool mColourMaterialEnabled;
		bool mAlphaTestEnabled;
		
		bool mVertexArrayEnabled;
		bool mColourArrayEnabled;
		bool mNormalArrayEnabled;
		
		bool* mTextureCoordinateArrayEnabled;
		Size* mTextureCoordinateArrayIndex;
		bool* mTexture2DEnabled;
		f32 mClearDepth;
		f32 mAlphaTestValue;
		bool mContextReady;
		u32 mActiveTextureStage;
		u32 mMaxTextureStagesUsed;
		Viewport::Rectangle mScissorRectangle;
		bool mScissorEnabled;
		shared_ptr<GLVertexBuffer> mActiveVertexBuffer;
		shared_ptr<GLShaderProgram> mActiveProgram;
		
		Mutex mTextureLookupMutex;
		std::map< Texture*, shared_ptr<GLTexture> > mTextureLookup;
		Mutex mVertexBufferLookupMutex;
		std::map< VertexBuffer*, shared_ptr<GLVertexBuffer> > mVertexBufferLookup;
		Mutex mShaderProgramLookupMutex;
		std::map< ShaderProgram*, shared_ptr<GLShaderProgram> > mShaderProgramLookup;
		Mutex mCubeMapTextureLookupMutex;
		std::map< CubeMapTexture*, shared_ptr<GLCubeMapTexture> > mCubeMapTextureLookup;

		PseudoAtomicSet< shared_ptr<GLTexture> > mTexturesToClean;
		PseudoAtomicSet< shared_ptr<GLCubeMapTexture> > mCubeMapTexturesToClean;
		PseudoAtomicSet< shared_ptr<GLVertexBuffer> > mVertexBuffersToClean;
		Matrix4 mProjectionMatrix;
	};
}
#endif 
#endif
