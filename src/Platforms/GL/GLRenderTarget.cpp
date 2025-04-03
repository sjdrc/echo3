#include <echo/Platforms/GL/GLRenderTarget.h>
#ifdef ECHO_RENDERER_GL
#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/BlendMode.h>
#include <echo/Graphics/Texture.h>
#include <echo/Graphics/CubeMapTexture.h>
#include <echo/Graphics/Light.h>
#include <echo/Graphics/VertexBuffer.h>
#include <echo/Graphics/ElementBuffer.h>
#include <echo/Platforms/GL/GLTexture.h>
#include <echo/Platforms/GL/GLCubeMapTexture.h>
#include <echo/Platforms/GL/GLShaderProgram.h>
#include <echo/Platforms/GL/GLVertexBuffer.h>
#include <boost/foreach.hpp>
#include <sstream>
#include <iostream>

#ifdef ECHO_GLES_SUPPORT
#define GL_SOURCE0_RGB GL_SRC0_RGB
#define GL_SOURCE1_RGB GL_SRC1_RGB
#define GL_SOURCE2_RGB GL_SRC2_RGB

#define GL_SOURCE0_ALPHA GL_SRC0_ALPHA
#define GL_SOURCE1_ALPHA GL_SRC1_ALPHA
#define GL_SOURCE2_ALPHA GL_SRC2_ALPHA
#endif
namespace Echo
{
	GLRenderTarget::GLRenderTarget(shared_ptr<GLContext> context) : mContext(context)
	{
		mWidth = 0;
		mHeight = 0;
		mBytesPerPixel = 0;
		mContext->mActiveTextureStage = 0;
		mMaxTextureStages = 1;
		mPointAndLineSize = 1;
		mMaxFixedLights = 0;
		mClearMask = ClearMaskFlags::DEPTH | ClearMaskFlags::COLOUR;
		mTextureDelegate = ResourceDelegate<Texture>::Create();
		mCubeMapTextureDelegate = ResourceDelegate<CubeMapTexture>::Create();
		mVertexBufferDelegate = ResourceDelegate<VertexBuffer>::Create();
		//std::string glVersion(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
		//ECHO_LOG_INFO("Open GL version: " << glVersion);
	}

	GLRenderTarget::~GLRenderTarget()
	{
	}

	bool GLRenderTarget::_SetActiveTextureStage(u32 stage)
	{
		if(stage >= mMaxTextureStages)
			return false;
		mContext->mMaxTextureStagesUsed = std::max(stage+1,mContext->mMaxTextureStagesUsed);
		mContext->mActiveTextureStage = stage;
		glActiveTexture(GL_TEXTURE0 + stage);
		glClientActiveTexture(GL_TEXTURE0 + stage);
		return true;
	}

	void GLRenderTarget::DetermineCapabilities()
	{
		//Determine the maximum number of texture units
		// glActiveTexture() -	texture should be one of GL_TEXTUREi, where i ranges from 0 to the larger
		//						of (GL_MAX_TEXTURE_COORDS-1) and (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS-1).
		//						https://www.opengl.org/sdk/docs/man2/xhtml/glActiveTexture.xml
		GLint iUnits;
		glGetIntegerv(GL_MAX_TEXTURE_UNITS, &iUnits);
		mMaxTextureStages = iUnits;
		mMaxFixedLights = 0;

		GLint maxlights;
		glGetIntegerv(GL_MAX_LIGHTS, &maxlights);
		mMaxFixedLights = maxlights;
		
#ifndef ECHO_GLES_SUPPORT
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &iUnits);
		if(static_cast<u32>(iUnits)>mMaxTextureStages)
		{
			mMaxTextureStages = iUnits;
		}
#endif
		delete [] mContext->mTextureCoordinateArrayEnabled;
		delete [] mContext->mTexture2DEnabled;
		delete [] mContext->mTextureCoordinateArrayIndex;
		mContext->mTextureCoordinateArrayEnabled = new bool[mMaxTextureStages];
		mContext->mTexture2DEnabled = new bool[mMaxTextureStages];
		mContext->mTextureCoordinateArrayIndex = new Size[mMaxTextureStages];
		
	}

	bool GLRenderTarget::Activate()
	{
		if(mContext->mContextReady)
		{
			return true;
		}
		return false;
	}

	void GLRenderTarget::Deactivate()
	{
		// Clean resources on the render thread.
		mContext->mTexturesToClean.GetSet().clear();
		mContext->mVertexBuffersToClean.GetSet().clear();
	}

	void GLRenderTarget::SwapBuffers()
	{
	}

	u32 GLRenderTarget::GetWidth() const
	{
		return mWidth;
	}
	
	u32 GLRenderTarget::GetHeight() const
	{
		return mHeight;
	}
	
	u32 GLRenderTarget::GetBytesPerPixel() const
	{
		return mBytesPerPixel;
	}
	
	void GLRenderTarget::ContextLost()
	{
		mContext->mContextReady = false;
		{
			ScopedLock lock(mContext->mTextureLookupMutex);
			mContext->mTextureLookup.clear();
		}
		{
			ScopedLock lock(mContext->mVertexBufferLookupMutex);
			mContext->mVertexBufferLookup.clear();
		}
		{
			ScopedLock lock(mContext->mShaderProgramLookupMutex);
			mContext->mShaderProgramLookup.clear();
		}
	}

	void GLRenderTarget::ContextRestored()
	{
		mContext->mContextReady = true;
		//Context restored could also restore texture resources if they aren't deleted on ContextLost().
	}
	
	void GLRenderTarget::SetProjectionMatrix(const Matrix4& matrix, bool isOrtho)
	{
		mContext->mProjectionMatrix = matrix;
		glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
		GLfloat gl_matrix[16];
		size_t x = 0;
		for(size_t i = 0; i < 4; i++)
		{
			for(size_t j = 0; j < 4; j++)
			{
				gl_matrix[x] = matrix[j][i];
				x++;
			}
		}
		glLoadMatrixf(gl_matrix);
		//glLoadMatrixf((GLfloat*)(matrix));
		EchoCheckOpenGLError();
	}

	void GLRenderTarget::SetModelViewMatrix(const Matrix4& matrix)
	{
		glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
		GLfloat gl_matrix[16];
		size_t x = 0;
		//j i default
		for(size_t i = 0; i < 4; i++)
		{
			for(size_t j = 0; j < 4; j++)
			{
				gl_matrix[x] = matrix[j][i];
				x++;
			}
		}
		glLoadMatrixf(gl_matrix);
		//glLoadMatrixf((GLfloat*)(matrix));
		EchoCheckOpenGLError();
	}

	void GLRenderTarget::SetTextureMatrix(const Matrix4& matrix, u32 stage)
	{
		if(!_SetActiveTextureStage(stage))
			return;
		glMatrixMode(GL_TEXTURE); // Select The Texture Matrix
		GLfloat gl_matrix[16];
		size_t x = 0;
		for(size_t i = 0; i < 4; i++)
		{
			for(size_t j = 0; j < 4; j++)
			{
				gl_matrix[x] = matrix[j][i];
				x++;
			}
		}
		glLoadMatrixf(gl_matrix);
		//glLoadMatrixf((GLfloat*)(matrix->m));
		EchoCheckOpenGLError();
	}

	void GLRenderTarget::SetViewport(Viewport& vp)
	{
		f32 targetWidth = static_cast<f32>(GetWidth());
		f32 targetHeight = static_cast<f32>(GetHeight());
		Viewport::Rectangle rectangle = vp.GetRectangle(targetWidth / targetHeight);

		GLint left = targetWidth * rectangle.mLeft;
		GLint width = (targetWidth * rectangle.mRight) - left;
		GLsizei bottom = targetHeight * (1.0f - rectangle.mBottom);
		GLsizei height = targetHeight * (rectangle.mBottom - rectangle.mTop);
		
		//We use left,top = 0,0 and right,bottom = 1,1
		//GL uses left,bottom and right,top as width,height
		glViewport(left, bottom, width, height);
		std::stringstream ss;
		ss << left << "," << bottom << "," << width << "," << height;
		EchoCheckOpenGLErrorInfo(ss.str());

		mCurrentViewport = &vp;
	}
	
	void GLRenderTarget::SetScissor(const Viewport::Rectangle& rectangle)
	{
		if(mContext->mScissorEnabled && mContext->mScissorRectangle==rectangle)
		{
			return;
		}
		
		glEnable(GL_SCISSOR_TEST);
		f32 targetWidth = static_cast<f32>(GetWidth());
		f32 targetHeight = static_cast<f32>(GetHeight());

		GLint left = targetWidth * rectangle.mLeft;
		GLint width = (targetWidth * rectangle.mRight) - left;
		GLsizei bottom = targetHeight * (1.0f - rectangle.mBottom);
		GLsizei height = targetHeight * (rectangle.mBottom - rectangle.mTop);
		
		glScissor(left, bottom, width, height);
		std::stringstream ss;
		ss << left << "," << bottom << "," << width << "," << height;
		EchoCheckOpenGLErrorInfo(ss.str());
		mContext->mScissorEnabled = true;
		mContext->mScissorRectangle=rectangle;
	}
	
	void GLRenderTarget::ResetScissor()
	{
		if(mContext->mScissorEnabled)
		{
			glScissor(0, 0, GetWidth(), GetHeight());
			std::stringstream ss;
			ss << 0 << "," << 0 << "," << GetWidth() << "," << GetHeight();
			EchoCheckOpenGLErrorInfo(ss.str());
			glEnable(GL_SCISSOR_TEST);
			mContext->mScissorEnabled = false;
			mContext->mScissorRectangle=Viewport::Rectangle(0,0,1,1);
		}
	}

	void GLRenderTarget::SetLightingEnabled(bool enabled)
	{
		if(enabled && !mContext->mLightingEnabled)
		{
			glEnable(GL_LIGHTING);
			mContext->mLightingEnabled = enabled;
		}else
		if(!enabled && mContext->mLightingEnabled)
		{
			glDisable(GL_LIGHTING);
			mContext->mLightingEnabled = enabled;
		}
	}
	
	void GLRenderTarget::SetVertexColourEnabled(bool enabled)
	{
		mContext->mVertexColourEnabled = enabled;
	}
	
	void GLRenderTarget::SetMaterialColourEnabled(bool enabled)
	{
		if(enabled && !mContext->mColourMaterialEnabled)
		{
			glEnable(GL_COLOR_MATERIAL);
			mContext->mColourMaterialEnabled = enabled;
		}else
		if(!enabled && mContext->mColourMaterialEnabled)
		{
			glDisable(GL_COLOR_MATERIAL);
			mContext->mColourMaterialEnabled = enabled;
		}
	}
	
	void GLRenderTarget::SetTexture2DEnabled(bool enabled, u32 stage)
	{
		if(enabled && !mContext->mTexture2DEnabled[stage])
		{
			glEnable(GL_TEXTURE_2D);
			mContext->mTexture2DEnabled[stage] = enabled;
		}else
		if(!enabled && mContext->mTexture2DEnabled[stage])
		{
			glDisable(GL_TEXTURE_2D);
			mContext->mTexture2DEnabled[stage] = enabled;
		}
	}
	
	void GLRenderTarget::ResetTextureUnits()
	{
		for(u32 i=0; i<mContext->mMaxTextureStagesUsed;++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glClientActiveTexture(GL_TEXTURE0 + i);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			if(mContext->mTexture2DEnabled[i])
			{
				glBindTexture(GL_TEXTURE_2D, 0);
				glDisable(GL_TEXTURE_2D);
				mContext->mTexture2DEnabled[i] = false;
			}
		}
		mContext->mMaxTextureStagesUsed=0;
	}
	
	void GLRenderTarget::ClearSources()
	{
		//We don't need to do anything here.
	}

	void GLRenderTarget::SetVertexBuffer(shared_ptr<VertexBuffer> vertexBuffer)
	{
		if(mContext->mActiveVertexBuffer)
		{
			mContext->mActiveVertexBuffer->Unbind();
		}

		if(mContext->mActiveProgram)
		{
			mContext->mActiveVertexBuffer = GetGLVertexBuffer(vertexBuffer.get());
			if(mContext->mActiveVertexBuffer)
			{
				mContext->mActiveVertexBuffer->Bind();
			}
		}else
		{
			if(!vertexBuffer)
			{
				if(mContext->mVertexArrayEnabled)
				{
					glDisableClientState(GL_VERTEX_ARRAY);
					mContext->mVertexArrayEnabled = false;
				}
				if(mContext->mNormalArrayEnabled)
				{
					glDisableClientState(GL_NORMAL_ARRAY);
					mContext->mNormalArrayEnabled = false;
				}
				if(mContext->mColourArrayEnabled)
				{
					glDisableClientState(GL_COLOR_ARRAY);
					mContext->mColourArrayEnabled = false;
				}
				for(Size i=0;i<mContext->mMaxTextureStagesUsed; ++i)
				{
					mContext->mActiveTextureStage = i;
					glActiveTexture(GL_TEXTURE0 + i);
					glClientActiveTexture(GL_TEXTURE0 + i);
					if(mContext->mTextureCoordinateArrayEnabled[i])
					{
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						mContext->mTextureCoordinateArrayEnabled[i] = false;
					}
				}
				return;
			}
			VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
			if(vertices)
			{
				glVertexPointer(3, GL_FLOAT, vertexBuffer->GetStride(), &(vertices[0]));
				if(!mContext->mVertexArrayEnabled)
				{
					glEnableClientState(GL_VERTEX_ARRAY);
					mContext->mVertexArrayEnabled = true;
				}
			}
			VertexBuffer::Accessor<Vector3> normals = vertexBuffer->GetAccessor<Vector3>("Normal");
			if(normals)
			{
				glNormalPointer(GL_FLOAT, vertexBuffer->GetStride(), &(normals[0]));
				if(!mContext->mNormalArrayEnabled)
				{
					glEnableClientState(GL_NORMAL_ARRAY);
					mContext->mNormalArrayEnabled = true;
				}
			}
			if(mContext->mVertexColourEnabled)
			{
				VertexBuffer::Accessor<VertexColour> colours = vertexBuffer->GetAccessor<VertexColour>("Colour");
				if(colours)
				{
					glColorPointer(4, GL_UNSIGNED_BYTE, vertexBuffer->GetStride(), &(colours[0]));
					if(!mContext->mColourArrayEnabled)
					{
						glEnableClientState(GL_COLOR_ARRAY);
						mContext->mColourArrayEnabled = true;
					}
				}
			}
			for(Size i=0;i<mContext->mMaxTextureStagesUsed; ++i)
			{
				std::stringstream ss;
				ss << "UV" << mContext->mTextureCoordinateArrayIndex[i];
				VertexBuffer::Accessor<TextureUV> uvs = vertexBuffer->GetAccessor<TextureUV>(ss.str());
				if(uvs)
				{
					mContext->mActiveTextureStage = i;
					glActiveTexture(GL_TEXTURE0 + i);
					glClientActiveTexture(GL_TEXTURE0 + i);
					glTexCoordPointer(2, GL_FLOAT, vertexBuffer->GetStride(), &(uvs[0]));
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					SetTexture2DEnabled(true,i);
					mContext->mTextureCoordinateArrayEnabled[i] = true;
				}
			}
		}

	}
	
	void GLRenderTarget::SetVertexSource(Vector2* source)
	{
		if(source)
		{
			glVertexPointer(2, GL_FLOAT, 0, source);
			if(!mContext->mVertexArrayEnabled)
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				mContext->mVertexArrayEnabled = true;
			}
		}else
		{
			if(mContext->mVertexArrayEnabled)
			{
				glDisableClientState(GL_VERTEX_ARRAY);
				mContext->mVertexArrayEnabled = false;
			}
		}
	}
	
	void GLRenderTarget::SetVertexSource(Vector3* source)
	{
		if(source)
		{
			glVertexPointer(3, GL_FLOAT, 0, source);
			if(!mContext->mVertexArrayEnabled)
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				mContext->mVertexArrayEnabled = true;
			}
		}else
		{
			if(mContext->mVertexArrayEnabled)
			{
				glDisableClientState(GL_VERTEX_ARRAY);
				mContext->mVertexArrayEnabled = false;
			}
		}
	}
	
	void GLRenderTarget::SetNormalSource(Vector3* source)
	{
		if(source)
		{
			glNormalPointer(GL_FLOAT, 0, source);
			if(!mContext->mNormalArrayEnabled)
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				mContext->mNormalArrayEnabled = true;
			}
		}else
		{
			if(mContext->mNormalArrayEnabled)
			{
				glDisableClientState(GL_NORMAL_ARRAY);
				mContext->mNormalArrayEnabled = false;
			}
		}
	}

	
	void GLRenderTarget::SetColourSource(VertexColour* source)
	{
		if(source)
		{
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, source);
			if(!mContext->mColourArrayEnabled)
			{
				glEnableClientState(GL_COLOR_ARRAY);
				mContext->mColourArrayEnabled = true;
			}
		}else
		{
			if(mContext->mColourArrayEnabled)
			{
				glDisableClientState(GL_COLOR_ARRAY);
				mContext->mColourArrayEnabled = false;
			}
		}
	}

	void GLRenderTarget::SetColourSource(Colour* source)
	{
		if(source)
		{
			glColorPointer(4, GL_FLOAT, 0, source);
			if(!mContext->mColourArrayEnabled)
			{
				glEnableClientState(GL_COLOR_ARRAY);
				mContext->mColourArrayEnabled = true;
			}
		}else
		{
			if(mContext->mColourArrayEnabled)
			{
				glDisableClientState(GL_COLOR_ARRAY);
				mContext->mColourArrayEnabled = false;
			}
		}
	}
	
	void GLRenderTarget::SetTextureCoordinateSource(TextureUV* source, u32 set)
	{
		if(_SetActiveTextureStage(set))
		{
			if(source)
			{
				glTexCoordPointer(2, GL_FLOAT, 0, source);
				if(!mContext->mTextureCoordinateArrayEnabled[set])
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					mContext->mTextureCoordinateArrayEnabled[set] = true;
				}
			}else
			{
				if(mContext->mTextureCoordinateArrayEnabled[set])
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					mContext->mTextureCoordinateArrayEnabled[set] = false;
				}
			}
		}
	}
	
	void GLRenderTarget::SetTextureCoordinateSourceIndex(Size set, Size index)
	{
		if(set < mMaxTextureStages && _SetActiveTextureStage(set))
		{
			mContext->mTextureCoordinateArrayIndex[set] = index;
		}
	}

	void GLRenderTarget::SetPointAndLineSize(f32 pointAndLineSize)
	{
		mPointAndLineSize = pointAndLineSize;
	}
	
	void GLRenderTarget::DrawElements(const ElementBuffer& elementBuffer)
	{
		GLint indexType = elementBuffer.GetIndexType()==ElementBuffer::IndexTypes::UNSIGNED_16BIT ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		GLenum elementType;
		switch(elementBuffer.GetElementType())
		{
			case ElementBuffer::ElementTypes::LINES:			elementType=GL_LINES;			break;
			case ElementBuffer::ElementTypes::POINTS:
				#ifndef ECHO_GLES_SUPPORT
						glEnable(GL_POINT_SPRITE);
						glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
						glPointSize(mPointAndLineSize);
				#else
						ECHO_LOG_WARNING_LOCATION("GLES does not support GL_POINT_SPRITE");
				#endif
				elementType=GL_POINTS;
			break;
			case ElementBuffer::ElementTypes::LINE_STRIP:		elementType=GL_LINE_STRIP;		break;
			case ElementBuffer::ElementTypes::LINE_LOOP:		elementType=GL_LINE_LOOP;		break;
			case ElementBuffer::ElementTypes::TRIANGLE:			elementType=GL_TRIANGLES;		break;
			case ElementBuffer::ElementTypes::TRIANGLE_STRIP:	elementType=GL_TRIANGLE_STRIP;	break;
			case ElementBuffer::ElementTypes::TRIANGLE_FAN:		elementType=GL_TRIANGLE_FAN;	break;
			default:
				ECHO_LOG_ERROR("Invalid element type");
				return;
		}
		glDrawElements(elementType, (GLsizei)(elementBuffer.GetNumberOfIndices()), indexType, elementBuffer.GetDataPointer());
		EchoCheckOpenGLError();
	}
	
	void GLRenderTarget::DrawTriangles(std::vector<u16>& indices)
	{
		glDrawElements(GL_TRIANGLES, (GLsizei)(indices.size()), GL_UNSIGNED_SHORT, &indices[0]);
	}

	void GLRenderTarget::DrawTriangleStrip(std::vector<u16>& indices)
	{
		glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)(indices.size()), GL_UNSIGNED_SHORT, &indices[0]);
	}
	
	void GLRenderTarget::DrawLines(std::vector<u16>& indices, f32 lineWidth)
	{
		glDrawElements(GL_LINES, (GLsizei)(indices.size()), GL_UNSIGNED_SHORT, &indices[0]);
	}

	void GLRenderTarget::DrawLineStrip(std::vector<u16>& indices, f32 lineWidth)
	{
		glDrawElements(GL_LINE_STRIP, (GLsizei)(indices.size()), GL_UNSIGNED_SHORT, &indices[0]);
	}

	void GLRenderTarget::DrawPoints(std::vector<u16>& indices, f32 pointSize)
	{
#ifndef ECHO_GLES_SUPPORT
		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		glPointSize(pointSize);
#else
		ECHO_LOG_WARNING("GLES does not support GL_POINT_SPRITE");
#endif
		glDrawElements(GL_POINTS, (GLsizei)(indices.size()), GL_UNSIGNED_SHORT, &indices[0]);
	}
	
	void GLRenderTarget::SetVSyncEnabled(bool)
	{
		//this is platform specific because you need an extension to access the gl function
		//GLX_SGI_swap_control under Linux and WGL_EXT_swap_control
	}

	void GLRenderTarget::SetDepthTestEnabled(bool enabled)
	{
		//SetClearDepth(1.f);
		if(enabled && !mContext->mDepthTestEnabled)
		{
			glEnable(GL_DEPTH_TEST);
			mContext->mDepthTestEnabled = true;
			mContext->mDepthTestShouldBeDisabled = false;
			if(mContext->mDepthWriteEnabled)
			{
				SetDepthFunction(mContext->mNonForceDepthFunction);
			}
		}else
		{
			if(!enabled && mContext->mDepthTestEnabled)
			{
				//GL Documentation says that depth test needs to be enabled for depth write to work.
				if(mContext->mDepthWriteEnabled)
				{
					EnableDepthTestForOnlyDepthWrite();
				}else
				{
					mContext->mDepthTestEnabled = false;
					glDisable(GL_DEPTH_TEST);
				}
			}
		}
	}

	void GLRenderTarget::EnableDepthTestForOnlyDepthWrite()
	{
		//This method exists because the documentation states:
		//		Even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not
		//		updated if the depth test is disabled. In order to unconditionally write to the depth
		//		buffer, the depth test should be enabled and set to GL_ALWAYS.

		SetDepthTestEnabled(true);
		if(mContext->mDepthFunction!=RenderPass::DepthFunctions::ALWAYS)
		{
			SetDepthFunction(RenderPass::DepthFunctions::ALWAYS);
		}
		mContext->mNonForceDepthFunction = mContext->mDepthFunction;
		mContext->mDepthTestShouldBeDisabled = true;
	}

	void GLRenderTarget::SetDepthWriteEnabled(bool enabled)
	{
		if(enabled)
		{
			//GL Documentation says that depth test needs to be enabled for depth write to work.
			if(!mContext->mDepthTestEnabled)
			{
				EnableDepthTestForOnlyDepthWrite();
			}
			glDepthMask(GL_TRUE);
			mContext->mDepthWriteEnabled = true;
		}else
		{
			glDepthMask(GL_FALSE);
			mContext->mDepthWriteEnabled = false;
			if(mContext->mDepthTestShouldBeDisabled)
			{
				SetDepthFunction(mContext->mNonForceDepthFunction);
				SetDepthTestEnabled(false);
			}
		}
	}
	
	shared_ptr<GLTexture> GLRenderTarget::GetGLTexture(Texture* texture)
	{
		mContext->mTextureLookupMutex.Lock();
		std::map<Texture*, shared_ptr<GLTexture> >::iterator it = mContext->mTextureLookup.find(texture);
		shared_ptr<GLTexture> textureGL;
		
		bool createTexture=true;
		if(it!=mContext->mTextureLookup.end())
		{
			if(texture->GetVersion()==it->second->GetVersion())
			{
				textureGL = it->second;
				createTexture=false;
			}
		}

		if(createTexture)
		{
			textureGL.reset(new GLTexture(*texture));
			mContext->mTextureLookup[texture] = textureGL;

			// We have to unlock before using the delegate to avoid dining out with some philosphers
			mContext->mTextureLookupMutex.Unlock();
			shared_ptr< ResourceDelegate<Texture> > delegate = texture->GetResourceDelegate(mTextureDelegate);
			delegate->AddListener(this);
		}else
		{
			mContext->mTextureLookupMutex.Unlock();
		}
		
		return textureGL;
	}

	shared_ptr<GLCubeMapTexture> GLRenderTarget::GetGLCubeMapTexture(CubeMapTexture* cubeMap)
	{
		mContext->mCubeMapTextureLookupMutex.Lock();
		std::map<CubeMapTexture*, shared_ptr<GLCubeMapTexture> >::iterator it = mContext->mCubeMapTextureLookup.find(cubeMap);
		shared_ptr<GLCubeMapTexture> cubeMapGL;
		
		bool create=true;
		if(it!=mContext->mCubeMapTextureLookup.end())
		{
			if(cubeMap->GetVersion()==it->second->GetVersion())
			{
				cubeMapGL = it->second;
				create=false;
			}
		}

		if(create)
		{
			cubeMapGL.reset(new GLCubeMapTexture(*cubeMap));
			mContext->mCubeMapTextureLookup[cubeMap] = cubeMapGL;

			// We have to unlock before using the delegate to avoid dining out with some philosphers
			mContext->mCubeMapTextureLookupMutex.Unlock();
			shared_ptr< ResourceDelegate<CubeMapTexture> > delegate = cubeMap->GetResourceDelegate(mCubeMapTextureDelegate);
			delegate->AddListener(this);
		}else
		{
			mContext->mCubeMapTextureLookupMutex.Unlock();
		}
		
		return cubeMapGL;
	}

	shared_ptr<GLShaderProgram> GLRenderTarget::GetGLShaderProgram(ShaderProgram* shaderProgram, bool buildIfNotFound)
	{
#ifdef ECHO_GL_SUPPORTS_FRAGMENT_SHADER
		{
			ScopedLock lock(mContext->mShaderProgramLookupMutex);
			std::map<ShaderProgram*, shared_ptr<GLShaderProgram> >::iterator it = mContext->mShaderProgramLookup.find(shaderProgram);
			if(it!=mContext->mShaderProgramLookup.end())
			{
				if(shaderProgram->GetVersion()==it->second->GetVersion())
				{
					return it->second;
				}else
				{
					// Force a rebuild
					buildIfNotFound = true;
				}
			}
		}
		
		if(buildIfNotFound)
		{
			shared_ptr<GLShaderProgram> shaderProgramGL(new GLShaderProgram());
			if(!shaderProgramGL->CompileAndLink(*shaderProgram))
			{
				ECHO_LOG_ERROR("Failed to build shader program:" << shaderProgramGL->GetErrors());
				return shared_ptr<GLShaderProgram>();
			}
			{
				// A second lock is cheaper than holding up other threads shile the shader builds
				ScopedLock lock(mContext->mShaderProgramLookupMutex);
				mContext->mShaderProgramLookup[shaderProgram] = shaderProgramGL;
			}
		}
#endif
		return shared_ptr<GLShaderProgram>();
	}

	shared_ptr<GLVertexBuffer> GLRenderTarget::GetGLVertexBuffer(VertexBuffer* vertexBuffer)
	{
		shared_ptr<GLVertexBuffer> vertexBufferGL;
		{
			ScopedLock lock(mContext->mVertexBufferLookupMutex);
			std::map<VertexBuffer*, shared_ptr<GLVertexBuffer> >::iterator it = mContext->mVertexBufferLookup.find(vertexBuffer);
			
			if(it!=mContext->mVertexBufferLookup.end())
			{
				it->second->Update(*vertexBuffer);
				return it->second;
			}

			vertexBufferGL.reset(new GLVertexBuffer(*vertexBuffer));
			mContext->mVertexBufferLookup[vertexBuffer] = vertexBufferGL;
		}

		// We have to unlock before using the delegate to avoid dining out with some philosphers

		shared_ptr< ResourceDelegate<VertexBuffer> > delegate = vertexBuffer->GetResourceDelegate(mVertexBufferDelegate);
		delegate->AddListener(this);
		return vertexBufferGL;
	}
	
	void GLRenderTarget::SetTexture(Texture* texture, u32 stage)
	{
		if(!texture->IsLoaded())
		{
			return;
		}
		
		if(!_SetActiveTextureStage(stage))
		{
			return;
		}
		
		shared_ptr<GLTexture> textureGL = GetGLTexture(texture);

		glBindTexture(GL_TEXTURE_2D, textureGL->GetGLReference());
		EchoCheckOpenGLError();
	}

	void GLRenderTarget::SetCubeMap(CubeMapTexture* cubeMapTexture, u32 stage)
	{
		if(!_SetActiveTextureStage(stage))
		{
			return;
		}

		if(!cubeMapTexture)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			EchoCheckOpenGLError();
			return;
		}

		shared_ptr<GLCubeMapTexture> cubeMapTextureGL = GetGLCubeMapTexture(cubeMapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureGL->GetGLReference());
		EchoCheckOpenGLError();
	}

	bool GLRenderTarget::ActivateProgram(shared_ptr<ShaderProgram> shaderProgram)
	{
		shared_ptr<GLShaderProgram> glShader = GetGLShaderProgram(shaderProgram.get(),true);
		if(glShader)
		{
			glShader->Activate();
			mContext->mActiveProgram = glShader;
			return true;
		}
		return false;
	}

	void GLRenderTarget::DeactivateProgram(shared_ptr<ShaderProgram> shaderProgram)
	{
		shared_ptr<GLShaderProgram> glShader = GetGLShaderProgram(shaderProgram.get(),false);
		if(glShader)
		{
			glShader->Deactivate();
		}
		mContext->mActiveProgram = nullptr;
	}

	bool GLRenderTarget::BuildProgram(shared_ptr<ShaderProgram> shaderProgram)
	{
		return (GetGLShaderProgram(shaderProgram.get(),true) != nullptr);
	}

	void GLRenderTarget::SetTexGen(const TextureUnit::TextureGenerationModeSet& texGen, u32 stage)
	{
		if(!_SetActiveTextureStage(stage))
			return;
		switch(texGen.mMode)
		{
			case TextureUnit::TextureGenerationModes::LINEAR:
			{
				const TextureUnit::TextureGenerationModeSet::PlaneData& splane = texGen.mSPlaneData;
				const TextureUnit::TextureGenerationModeSet::PlaneData& tplane = texGen.mTPlaneData;
				GLfloat szPlane[] = {splane.a, splane.b, splane.c, splane.d};
				GLfloat tzPlane[] = {tplane.a, tplane.b, tplane.c, tplane.d};
	#ifdef ECHO_GL_SUPPORTS_TEXTURE_GENERATION
				//glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				//glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				//glTexGenfv(GL_Q, GL_OBJECT_PLANE, zPlane);
				//glTexGenfv(GL_R, GL_OBJECT_PLANE, zPlane);
				glTexGenfv(GL_S, GL_OBJECT_PLANE, szPlane);
				glTexGenfv(GL_T, GL_OBJECT_PLANE, tzPlane);
				//glEnable(GL_TEXTURE_GEN_Q);
				//glEnable(GL_TEXTURE_GEN_R);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
	#else
				//Do Texture Matrix version
				//GetTextureMatrix
				Matrix4 m(	szPlane[0], tzPlane[0], 0, 0,
							szPlane[1], tzPlane[1], 0, 0,
							szPlane[2], tzPlane[2], 1, 0,
							szPlane[3], tzPlane[3], 0, 1);	//note 1's form diagonal of identity matrix

				SetTextureMatrix(m, stage);
	#endif

			}
				break;
			case TextureUnit::TextureGenerationModes::EYE_LINEAR:
			{
				const TextureUnit::TextureGenerationModeSet::PlaneData& splane = texGen.mSPlaneData;
				const TextureUnit::TextureGenerationModeSet::PlaneData& tplane = texGen.mTPlaneData;
				GLfloat szPlane[] = {splane.a, splane.b, splane.c, splane.d};
				GLfloat tzPlane[] = {tplane.a, tplane.b, tplane.c, tplane.d};
	#ifdef ECHO_GL_SUPPORTS_TEXTURE_GENERATION
				//glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
				//glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
				//glTexGenfv(GL_Q, GL_EYE_PLANE, zPlane);
				//glTexGenfv(GL_R, GL_EYE_PLANE, zPlane);
				glTexGenfv(GL_S, GL_EYE_PLANE, szPlane);
				glTexGenfv(GL_T, GL_EYE_PLANE, tzPlane);
				//glEnable(GL_TEXTURE_GEN_Q);
				//glEnable(GL_TEXTURE_GEN_R);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
	#endif
			}
				break;
			case TextureUnit::TextureGenerationModes::SPHERE:
	#ifdef ECHO_GL_SUPPORTS_TEXTURE_GENERATION
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
	#endif
				break;
			case TextureUnit::TextureGenerationModes::CUBE:
			case TextureUnit::TextureGenerationModes::NONE:
			default:
	#ifdef ECHO_GL_SUPPORTS_TEXTURE_GENERATION
				glDisable(GL_TEXTURE_GEN_Q);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
	#endif
				break;
		}
	}

	void GLRenderTarget::SetBlendMode(const BlendMode& val)
	{
		switch(val)
		{
			case BlendModes::TRANSPARENT:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_BLEND);
				break;
			case BlendModes::ADDITIVE:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glEnable(GL_BLEND);
				break;
			case BlendModes::ADDITIVE_COLOUR:
				glBlendFunc(GL_SRC_COLOR, GL_ONE);
				glEnable(GL_BLEND);
				break;
			case BlendModes::NONE:
			default:
				glDisable(GL_BLEND);
				break;
		}
	}
	
	void GLRenderTarget::SetAlphaTest(const RenderPass::AlphaTestFunction& function, float referenceValue)
	{
		typedef RenderPass::AlphaTestFunctions AlphaTestFunctions;
		
		if(function==mContext->mAlphaTestFunction && referenceValue==mContext->mAlphaTestValue)
		{
			return;
		}
		mContext->mAlphaTestFunction = function;
		mContext->mAlphaTestValue = referenceValue;
		
		GLenum functionGL;
		switch(function)
		{
			case AlphaTestFunctions::NEVER:
				functionGL=GL_NEVER;
			break;
			case AlphaTestFunctions::LESS:
				functionGL=GL_LESS;
			break;
			case AlphaTestFunctions::EQUAL:
				functionGL=GL_EQUAL;
			break;
			case AlphaTestFunctions::LESS_OR_EQUAL:
				functionGL=GL_LEQUAL;
			break;
			case AlphaTestFunctions::GREATER:
				functionGL=GL_GREATER;
			break;
			case AlphaTestFunctions::NOT_EQUAL:
				functionGL=GL_NOTEQUAL;
			break;
			case AlphaTestFunctions::GREATER_OR_EQUAL:
				functionGL=GL_GEQUAL;
			break;
			case AlphaTestFunctions::ALWAYS:
				//Fall through
			default:
			//Disable alpha test
			glDisable(GL_ALPHA_TEST);
			return;
				
		}

		glAlphaFunc(functionGL,referenceValue);
		glEnable(GL_ALPHA_TEST);
	}

	void GLRenderTarget::SetTextureStageBlendMode(const LayerBlendModeEx& bm, u32 stage)
	{
		if(!_SetActiveTextureStage(stage))
		{
			return;
		}

		// Check to see if blending is supported
		//if(!mCurrentCapabilities->hasCapability(RSC_BLENDING))
		//	return;

		GLenum src1op, src2op, cmd;
		GLfloat colourValue[4];
		colourValue[0] = bm.colour.mRed;
		colourValue[1] = bm.colour.mGreen;
		colourValue[2] = bm.colour.mBlue;
		colourValue[3] = bm.colour.mAlpha;

		switch(bm.source1)
		{
			case LayerBlendSources::CURRENT:
				src1op = GL_PREVIOUS;
				break;
			case LayerBlendSources::TEXTURE:
				src1op = GL_TEXTURE;
				break;
			case LayerBlendSources::MANUAL:
				src1op = GL_CONSTANT;
				break;
			case LayerBlendSources::DIFFUSE:
				src1op = GL_PRIMARY_COLOR;

				break;
				// XXX
			case LayerBlendSources::SPECULAR:
				src1op = GL_PRIMARY_COLOR;
				break;
			default:
				src1op = 0;
		}

		switch(bm.source2)
		{
			case LayerBlendSources::CURRENT:
				src2op = GL_PREVIOUS;
				break;
			case LayerBlendSources::TEXTURE:
				src2op = GL_TEXTURE;
				break;
			case LayerBlendSources::MANUAL:
				src2op = GL_CONSTANT;
				break;
			case LayerBlendSources::DIFFUSE:
				src2op = GL_PRIMARY_COLOR;
				break;
				// XXX
			case LayerBlendSources::SPECULAR:
				src2op = GL_PRIMARY_COLOR;
				break;
			default:
				src2op = 0;
		}

		switch(bm.operation)
		{
			case LayerBlendOperationExs::SOURCE1:
				cmd = GL_REPLACE;
				break;
			case LayerBlendOperationExs::SOURCE2:
				cmd = GL_REPLACE;
				break;
			case LayerBlendOperationExs::MODULATE:
				cmd = GL_MODULATE;
				break;
			case LayerBlendOperationExs::MODULATE_X2:
				cmd = GL_MODULATE;
				break;
			case LayerBlendOperationExs::MODULATE_X4:
				cmd = GL_MODULATE;
				break;
			case LayerBlendOperationExs::ADD:
				cmd = GL_ADD;
				break;
			case LayerBlendOperationExs::ADD_SIGNED:
				cmd = GL_ADD_SIGNED;
				break;
			case LayerBlendOperationExs::ADD_SMOOTH:
				cmd = GL_INTERPOLATE;
				break;
			case LayerBlendOperationExs::SUBTRACT:
				cmd = GL_SUBTRACT;
				break;
			case LayerBlendOperationExs::BLEND_DIFFUSE_COLOUR:
				cmd = GL_INTERPOLATE;
				break;
			case LayerBlendOperationExs::BLEND_DIFFUSE_ALPHA:
				cmd = GL_INTERPOLATE;
				break;
			case LayerBlendOperationExs::BLEND_TEXTURE_ALPHA:
				cmd = GL_INTERPOLATE;
				break;
			case LayerBlendOperationExs::BLEND_CURRENT_ALPHA:
				cmd = GL_INTERPOLATE;
				break;
			case LayerBlendOperationExs::BLEND_MANUAL:
				cmd = GL_INTERPOLATE;
				break;
			case LayerBlendOperationExs::DOTPRODUCT:
				//cmd = mCurrentCapabilities->hasCapability(RSC_DOT3) ? GL_DOT3_RGB : GL_MODULATE;
				cmd = GL_MODULATE;
				break;
			default:
				cmd = 0;
		}

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

		if(bm.blendType == LayerBlendTypes::COLOUR)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, cmd);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, src1op);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, src2op);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
		} else
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, cmd);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, src1op);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, src2op);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_CONSTANT);
		}

		float blendValue[4] = {0, 0, 0, bm.colour.mAlpha};
		switch(bm.operation)
		{
			case LayerBlendOperationExs::BLEND_DIFFUSE_COLOUR:
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_PRIMARY_COLOR);
				break;
			case LayerBlendOperationExs::BLEND_DIFFUSE_ALPHA:
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_PRIMARY_COLOR);
				break;
			case LayerBlendOperationExs::BLEND_TEXTURE_ALPHA:
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_TEXTURE);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_TEXTURE);
				break;
			case LayerBlendOperationExs::BLEND_CURRENT_ALPHA:
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PREVIOUS);
				glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_PREVIOUS);
				break;
			case LayerBlendOperationExs::BLEND_MANUAL:
				glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, blendValue);
				break;
			default:
				break;
		};

		//This is switched twice so the default case will be used fore the above configuration too.
		switch(bm.operation)
		{
			case LayerBlendOperationExs::MODULATE_X2:
				glTexEnvi(GL_TEXTURE_ENV, bm.blendType == LayerBlendTypes::COLOUR ? GL_RGB_SCALE : GL_ALPHA_SCALE, 2);
				break;
			case LayerBlendOperationExs::MODULATE_X4:
				glTexEnvi(GL_TEXTURE_ENV, bm.blendType == LayerBlendTypes::COLOUR ? GL_RGB_SCALE : GL_ALPHA_SCALE, 4);
				break;
			default:
				glTexEnvi(GL_TEXTURE_ENV, bm.blendType == LayerBlendTypes::COLOUR ? GL_RGB_SCALE : GL_ALPHA_SCALE, 1);
				break;
		}

		if(bm.blendType == LayerBlendTypes::COLOUR)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
			if(bm.operation == LayerBlendOperationExs::BLEND_DIFFUSE_COLOUR)
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
			} else
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
			}
		}

		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);
		if(bm.source1 == LayerBlendSources::MANUAL || bm.source2 == LayerBlendSources::MANUAL)
		{
			glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, colourValue);
		}
	}

	void GLRenderTarget::SetWrapModeU(const TextureUnit::TextureWrapMode& val)
	{
		switch(val)
		{
			case TextureUnit::TextureWrapModes::CLAMP:
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				break;
			case TextureUnit::TextureWrapModes::REPEAT:
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				break;
			case TextureUnit::TextureWrapModes::CLAMP_TO_EDGE:
			default:
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				break;
		}
	}

	void GLRenderTarget::SetWrapModeV(const TextureUnit::TextureWrapMode& val)
	{
		switch(val)
		{
			case TextureUnit::TextureWrapModes::CLAMP:
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			case TextureUnit::TextureWrapModes::REPEAT:
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			case TextureUnit::TextureWrapModes::CLAMP_TO_EDGE:
			default:
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
		}
	}

	void GLRenderTarget::SetMinFilter(const TextureUnit::TextureFilter& val)
	{
		if(val == TextureUnit::TextureFilters::NEAREST)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	void GLRenderTarget::SetMagFilter(const TextureUnit::TextureFilter& val)
	{
		if(val == TextureUnit::TextureFilters::NEAREST)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void GLRenderTarget::SetDepthFunction(const RenderPass::DepthFunction& val)
	{
		if(val==mContext->mDepthFunction)
		{
			return;
		}
		mContext->mDepthFunction = val;
		switch(mContext->mDepthFunction)
		{
			case RenderPass::DepthFunctions::NEVER:
				glDepthFunc(GL_NEVER);
				break;
			case RenderPass::DepthFunctions::LESS:
				glDepthFunc(GL_LESS);
				break;
			case RenderPass::DepthFunctions::EQUAL:
				glDepthFunc(GL_EQUAL);
				break;
			case RenderPass::DepthFunctions::LESS_OR_EQUAL:
				glDepthFunc(GL_LEQUAL);
				break;
			case RenderPass::DepthFunctions::GREATER:
				glDepthFunc(GL_GREATER);
				break;
			case RenderPass::DepthFunctions::NOT_EQUAL:
				glDepthFunc(GL_NOTEQUAL);
				break;
			case RenderPass::DepthFunctions::GREATER_OR_EQUAL:
				glDepthFunc(GL_GEQUAL);
				break;
			case RenderPass::DepthFunctions::ALWAYS:
				glDepthFunc(GL_ALWAYS);
				break;
		};
	}

	void GLRenderTarget::SetCullMode(const RenderPass::CullMode& val)
	{
		if(val == RenderPass::CullModes::NONE)
			glDisable(GL_CULL_FACE);
		else
		{
			glEnable(GL_CULL_FACE);
			if(val == RenderPass::CullModes::BACK)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);
		}
	}

	void GLRenderTarget::SetAmbient(const Colour& val)
	{
		GLfloat vals[4] = {val.mRed, val.mGreen, val.mBlue, val.mAlpha};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, vals);
	}

	void GLRenderTarget::SetDiffuse(const Colour& val)
	{
		GLfloat vals[4] = {val.mRed, val.mGreen, val.mBlue, val.mAlpha};
		glColor4f(val.mRed, val.mGreen, val.mBlue, val.mAlpha); //vals);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, vals);
	}

	void GLRenderTarget::SetSpecular(const Colour& val)
	{
		GLfloat vals[4] = {val.mRed, val.mGreen, val.mBlue, val.mAlpha};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, vals);
	}

	void GLRenderTarget::SetEmissive(const Colour& val)
	{
		GLfloat vals[4] = {val.mRed, val.mGreen, val.mBlue, val.mAlpha};
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, vals);
	}

	void GLRenderTarget::SetShininess(f32 val)
	{
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, val);
	}

	void GLRenderTarget::SetMaterialColours(const Colour& ambient, const Colour& diffuse, const Colour& specular,
											   const Colour& emissive, f32 shininess)
	{
		GLfloat vals[4] = {diffuse.mRed, diffuse.mGreen, diffuse.mBlue, diffuse.mAlpha};
		//glColor4f(diffuse.mR, diffuse.mG, diffuse.mB, diffuse.mA); //vals);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, vals);
		vals[0] = ambient.mRed;
		vals[1] = ambient.mGreen;
		vals[2] = ambient.mBlue;
		vals[3] = ambient.mAlpha;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, vals);
		vals[0] = specular.mRed;
		vals[1] = specular.mGreen;
		vals[2] = specular.mBlue;
		vals[3] = specular.mAlpha;
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, vals);
		vals[0] = emissive.mRed;
		vals[1] = emissive.mGreen;
		vals[2] = emissive.mBlue;
		vals[3] = emissive.mAlpha;
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, vals);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	}

	void GLRenderTarget::SetFullScreen(bool fullscreen)
	{
		//this is platform specific
	}

	void GLRenderTarget::SetClearColour(const Colour& colour)
	{
		glClearColor(colour.mRed, colour.mGreen, colour.mBlue, colour.mAlpha);
		EchoCheckOpenGLError();
	}

	void GLRenderTarget::SetClearDepth(f32 depth)
	{
		mContext->mClearDepth = depth;
	}
	
	void GLRenderTarget::SetClearMask(ClearMask clearMask)
	{
		mClearMask=clearMask;
	}

	void GLRenderTarget::Clear()
	{
		GLbitfield mask = 0;
		if(mClearMask & ClearMaskFlags::DEPTH)
		{
		#ifdef ECHO_GLES_SUPPORT
			glClearDepthf(mContext->mClearDepth);
		#else
			glClearDepth(mContext->mClearDepth);
		#endif
			EchoCheckOpenGLError();
			SetDepthWriteEnabled(true);
			mask |= GL_DEPTH_BUFFER_BIT;
		}
		if(mClearMask & ClearMaskFlags::COLOUR)
			mask |= GL_COLOR_BUFFER_BIT;
		if(mClearMask & ClearMaskFlags::STENCIL)
			mask |= GL_STENCIL_BUFFER_BIT;
		glClear(mask);
		EchoCheckOpenGLError();
	}

	void GLRenderTarget::ShowExternalCursors(bool show)
	{
	}

	void GLRenderTarget::SetLight(u32 index, Light* lt)
	{
		if(index >= mMaxFixedLights)
		{
			return;
		}

		GLenum gl_index = GL_LIGHT0 + index;

		if(!lt)
		{
			// Disable in the scene
			glDisable(gl_index);
		} else
		{
			switch(lt->GetType())
			{
				case Light::LightTypes::SPOTLIGHT:
					glLightf(gl_index, GL_SPOT_CUTOFF, 0.5f * lt->GetSpotlightOuterAngle().ValueDegrees());
					glLightf(gl_index, GL_SPOT_EXPONENT, lt->GetSpotlightFalloff());
					break;
				default:
					glLightf(gl_index, GL_SPOT_CUTOFF, 180.0);
					break;
			}

			// Color
			Colour col = lt->GetDiffuseColour();

			GLfloat f4vals[4] = {col.mRed, col.mGreen, col.mBlue, col.mAlpha};
			glLightfv(gl_index, GL_DIFFUSE, f4vals);

			col = lt->GetSpecularColour();
			f4vals[0] = col.mRed;
			f4vals[1] = col.mGreen;
			f4vals[2] = col.mBlue;
			f4vals[3] = col.mAlpha;
			glLightfv(gl_index, GL_SPECULAR, f4vals);

			// Disable ambient light for movables;
			f4vals[0] = 0;
			f4vals[1] = 0;
			f4vals[2] = 0;
			f4vals[3] = 1;
			glLightfv(gl_index, GL_AMBIENT, f4vals);

			// Set position / direction
			Vector3 vec = lt->GetDerivedPosition();
			Vector4 vec4;
			if(lt->GetType() == Light::LightTypes::DIRECTIONAL)
			{
				vec4.x = -vec.x;
				vec4.y = -vec.y;
				vec4.z = -vec.z;
				vec4.w = 0.f;
			}else
			{
				vec4.x = vec.x;
				vec4.y = vec.y;
				vec4.z = vec.z;
				vec4.w = 1.f;
			}

			glLightfv(gl_index, GL_POSITION, vec4.Ptr());
			// Set spotlight direction
			if(lt->GetType() == Light::LightTypes::SPOTLIGHT)
			{
				vec = lt->GetDerivedOrientation().ZAxis();
				glLightfv(gl_index, GL_SPOT_DIRECTION, vec.Ptr());
			}
			
			// Attenuation
			glLightf(gl_index, GL_CONSTANT_ATTENUATION, lt->GetAttenuationConstant());
			glLightf(gl_index, GL_LINEAR_ATTENUATION, lt->GetAttenuationLinear());
			glLightf(gl_index, GL_QUADRATIC_ATTENUATION, lt->GetAttenuationQuadric());
			// Enable in the scene
			glEnable(gl_index);
		}
	}

	const Matrix4& GLRenderTarget::GetProjectionMatrix()
	{
		return mContext->mProjectionMatrix;

	}
	
	void GLRenderTarget::OnResourceDestroyed(Texture* texture)
	{
		//Find the texture and add it to the textures to clean list which will be cleared on the render thread
		ScopedLock lock(mContext->mTextureLookupMutex);

		std::map<Texture*, shared_ptr<GLTexture> >::iterator it = mContext->mTextureLookup.find(texture);
		if(it!=mContext->mTextureLookup.end())
		{
			mContext->mTexturesToClean.Insert(it->second);
			mContext->mTextureLookup.erase(it);
		}
	}

	void GLRenderTarget::OnResourceUnloaded(Texture* texture)
	{
		//We do the same for unloaded textures as destroyed textures.
		OnResourceDestroyed(texture);
	}

	void GLRenderTarget::OnResourceDestroyed(CubeMapTexture* cubeMap)
	{
		//Find the texture and add it to the textures to clean list which will be cleared on the render thread
		ScopedLock lock(mContext->mCubeMapTextureLookupMutex);

		std::map<CubeMapTexture*, shared_ptr<GLCubeMapTexture> >::iterator it = mContext->mCubeMapTextureLookup.find(cubeMap);
		if(it!=mContext->mCubeMapTextureLookup.end())
		{
			mContext->mCubeMapTexturesToClean.Insert(it->second);
			mContext->mCubeMapTextureLookup.erase(it);
		}
	}

	void GLRenderTarget::OnResourceUnloaded(CubeMapTexture* cubeMap)
	{
		//We do the same for unloaded textures as destroyed textures.
		OnResourceDestroyed(cubeMap);
	}

	void GLRenderTarget::OnResourceDestroyed(VertexBuffer* vertexBuffer)
	{
		ScopedLock lock(mContext->mVertexBufferLookupMutex);
		std::map<VertexBuffer*, shared_ptr<GLVertexBuffer> >::iterator it = mContext->mVertexBufferLookup.find(vertexBuffer);
		if(it!=mContext->mVertexBufferLookup.end())
		{
			mContext->mVertexBuffersToClean.Insert(it->second);
			mContext->mVertexBufferLookup.erase(it);
		}
	}

	void GLRenderTarget::OnResourceUnloaded(VertexBuffer* vertexBuffer)
	{
		OnResourceDestroyed(vertexBuffer);
	}
}
#endif
