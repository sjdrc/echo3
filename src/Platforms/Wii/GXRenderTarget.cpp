#include <echo/Platforms/Wii/GXRenderTarget.h>
#include <echo/Platforms/Wii/GXTexture.h>

#include <echo/Graphics/Viewport.h>
#include <echo/Graphics/Camera.h>
#include <echo/Graphics/Scene.h>
#include <echo/Graphics/VertexBuffer.h>
#include <echo/Graphics/ElementBuffer.h>
#include <ogc/gx.h>
#include <malloc.h>

#define DEFAULT_FIFO_SIZE	(256*1024)
namespace Echo
{
	GXRenderTarget::GXRenderTarget(Kernel& kernel) : mKernel(kernel),
			mNumActiveTextures(0),
			mNumTexturesSources(0),
			mNumTextureUnits(0),
			mActiveTexture(0)
	{
		ClearSources();
		mZEnabled=GX_TRUE;
		mZWriteEnabled=GX_TRUE;
		mDepthFunction=GX_LEQUAL;
		mScissorRectangle = Viewport::Rectangle(0,0,1,1);
		mScissorEnabled = false;
		mVertexSize = 0;

		mAlphaTestValue = 0;
		mAlphaTestFunction = RenderPass::AlphaTestFunctions::ALWAYS;
		mVertexColourEnabled = false;

		VIDEO_Init();

		mRMode = VIDEO_GetPreferredMode(NULL);

		mExternalFrameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(mRMode));
		mExternalFrameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(mRMode));
		mCurrentBuffer=0;

		VIDEO_Configure(mRMode);
		VIDEO_SetNextFramebuffer(mExternalFrameBuffer[mCurrentBuffer]);
		//console_init(mExternalFrameBuffer[mCurrentBuffer],20,20,mRMode->fbWidth,mRMode->xfbHeight,mRMode->fbWidth*VI_DISPLAY_PIX_SZ);
		//VIDEO_SetPostRetraceCallback(copy_to_xfb);
		VIDEO_SetBlack(FALSE);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		if(mRMode->viTVMode&VI_NON_INTERLACE)
			VIDEO_WaitVSync();

		//GX needs to be setup first - The texture objects can't be created otherwise and we get a hang..
		mGraphicsFIFO = MEM_K0_TO_K1(memalign(32,DEFAULT_FIFO_SIZE));
		memset(mGraphicsFIFO,0,DEFAULT_FIFO_SIZE);
		GX_Init(mGraphicsFIFO,DEFAULT_FIFO_SIZE);

		// clears the bg to colour and clears the z buffer
		mBackgroundColour.r=0;
		mBackgroundColour.g=0;
		mBackgroundColour.b=0;
		mBackgroundColour.a=0xFF;
		GXRenderTarget::SetClearDepth(GX_MAX_Z24);	//Calls Set Copy Clear so we just set the background colour here
		GXRenderTarget::SetSize(mRMode->fbWidth,mRMode->efbHeight);

		// other gx setup
		GX_SetViewport(0,0,mRMode->fbWidth,mRMode->efbHeight,0,1);
		GX_SetDispCopyYScale((f32)mRMode->xfbHeight/(f32)mRMode->efbHeight);
		GX_SetScissor(0,0,mRMode->fbWidth,mRMode->efbHeight);
		GX_SetDispCopySrc(0,0,mRMode->fbWidth,mRMode->efbHeight);
		GX_SetDispCopyDst(mRMode->fbWidth,mRMode->xfbHeight);
		GX_SetCopyFilter(mRMode->aa,mRMode->sample_pattern,GX_TRUE,mRMode->vfilter);
		GX_SetFieldMode(mRMode->field_rendering,((mRMode->viHeight==2*mRMode->xfbHeight)?GX_ENABLE:GX_DISABLE));

		if (mRMode->aa)
			GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
		else
			GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
		
		GX_CopyDisp(mExternalFrameBuffer[mCurrentBuffer],GX_TRUE);
		GX_SetDispCopyGamma(GX_GM_1_0);

		GX_SetNumChans(1); // default, colour = vertex colour
		GX_InvalidateTexAll();

		GXColor black={128,128,128,255};
		GXColor white={255,255,255,255};

		GX_SetChanAmbColor(GX_COLOR0, black);
		GX_SetChanMatColor(GX_COLOR0, white);

		GX_SetBlendMode(GX_BM_BLEND,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA,0);
		GX_SetColorUpdate(GX_ENABLE);
		GX_SetAlphaUpdate(GX_ENABLE);
	}

	GXRenderTarget::~GXRenderTarget()
	{
	}

	bool GXRenderTarget::Activate()
	{
		GX_InvVtxCache();			//This can be done more intelligently, such as when the vertex indexed source changes - see documentation.
		//GX_InvalidateTexAll();
		GX_SetColorUpdate(GX_TRUE);
		GX_SetAlphaUpdate(GX_TRUE);
		return true;
	}

	void GXRenderTarget::Deactivate()
	{
		GX_Flush();
	}

	u32 GXRenderTarget::GetWidth() const
	{
		return mRMode->fbWidth;
	}
	u32 GXRenderTarget::GetHeight() const
	{
		return mRMode->efbHeight;
	}

	u32 GXRenderTarget::GetBytesPerPixel() const
	{
		return 4;
	}

	void GXRenderTarget::ContextLost()
	{
		//No such thing as a context.
	}

	void GXRenderTarget::ContextRestored()
	{
		//No such thing as a context.
	}

	const Matrix4& GXRenderTarget::GetProjectionMatrix()
	{
		return mProjectionMatrix;
	}

	void GXRenderTarget::SetProjectionMatrix(const Matrix4& matrix, bool isOrtho)
	{
		mProjectionMatrix=matrix;
		if(isOrtho)
		{
			GX_LoadProjectionMtx((f32(*)[4])mProjectionMatrix[0], GX_ORTHOGRAPHIC);
		}else
		{
			GX_LoadProjectionMtx((f32(*)[4])mProjectionMatrix[0], GX_PERSPECTIVE);
		}
		
	}

	void GXRenderTarget::SetModelViewMatrix(const Matrix4& matrix)
	{
		GX_LoadPosMtxImm((f32(*)[4])matrix[0], GX_PNMTX0);
	}
	
	void GXRenderTarget::SetTextureMatrix(const Matrix4& matrix, u32 stage)
	{
		//GX_LoadTexMtxImm((f32(*)[4])matrix[0], GX_MTX3x4, GX_TEXMTX0+stage);
	}

	void GXRenderTarget::SetViewport(Viewport& vp)
	{
		//TODO: Set Near and Far distances
		f32 width=GetWidth();
		f32 height=GetHeight();

		Viewport::Rectangle rectangle = vp.GetRectangle(width/height);
		
		f32 left= rectangle.mLeft * width;
		f32 top= rectangle.mTop * height;
		f32 right= rectangle.mRight * width;
		f32 bottom= rectangle.mBottom * height;
		GX_SetViewport(left,top,right-left,bottom-top,0.01f,500.f);
	}

	void GXRenderTarget::SetScissor(const Viewport::Rectangle& rectangle)
	{
		if(mScissorEnabled && mScissorRectangle==rectangle)
		{
			return;
		}
		f32 width=GetWidth();
		f32 height=GetHeight();

		f32 left= rectangle.mLeft * width;
		f32 top= rectangle.mTop * height;
		f32 right= rectangle.mRight * width;
		f32 bottom= rectangle.mBottom * height;
		GX_SetScissor(left,top,right-left,bottom-top);

		mScissorEnabled = true;
		mScissorRectangle=rectangle;
	}
	
	void GXRenderTarget::ResetScissor()
	{
		if(mScissorEnabled)
		{
			GX_SetScissor(0,0,GetWidth(),GetHeight());
			mScissorEnabled = false;
			mScissorRectangle=Viewport::Rectangle(0,0,1,1);
		}
	}
		
	void GXRenderTarget::SetVSyncEnabled(bool enabled)
	{
		//Always is
	}

	void GXRenderTarget::SetFullScreen(bool fullscreen)
	{
		//Always is
	}

	void GXRenderTarget::SetClearColour(const Colour& colour)
	{
		mBackgroundColour.r=255*colour.mRed;
		mBackgroundColour.g=255*colour.mGreen;
		mBackgroundColour.b=255*colour.mBlue;
		mBackgroundColour.a=255*colour.mAlpha;
	}

	void GXRenderTarget::SetClearDepth(f32 depth)
	{
		//TODO: Set the clear depth
		mClearDepth=GX_MAX_Z24;
		GX_SetCopyClear(mBackgroundColour, mClearDepth);
	}

	void GXRenderTarget::SetClearMask(ClearMask clearMask)
	{
		GX_SetCopyClear(mBackgroundColour, mClearDepth);
		//TODO: Clearmask
	}

	void GXRenderTarget::Clear()
	{
		//TODO: Clear
	}

	void GXRenderTarget::ShowExternalCursors(bool show)
	{
		//TODO: ShowExternalCursors
	}

//#define DEBUG_BUFFER_SWITCH
	void GXRenderTarget::SwapBuffers()
	{
#ifdef DEBUG_BUFFER_SWITCH
		//Double buffer handling
		static bool flip=false;
		if(flip)
		{
			SetClearColour(Colours::BLUE_VIOLET);
		}else
		{
			SetClearColour(Colours::GREEN);
		}
		flip=!flip;
		GX_SetCopyClear(mBackgroundColour, mClearDepth);
#endif
		GX_DrawDone();
		
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(mExternalFrameBuffer[mCurrentBuffer],GX_TRUE);
		GX_Flush();
		VIDEO_SetNextFramebuffer(mExternalFrameBuffer[mCurrentBuffer]);
		VIDEO_Flush();
		++mCurrentBuffer;
		if(mCurrentBuffer>1)
			mCurrentBuffer=0;
		VIDEO_WaitVSync();
	}

	void GXRenderTarget::SetName(const std::string& name)
	{
		mName=name;
	}

	std::string GXRenderTarget::GetName()
	{
		return mName;
	}

	void GXRenderTarget::SetLightingEnabled(bool enabled)
	{
	}

	void GXRenderTarget::SetMaterialColourEnabled(bool enabled)
	{

	}

	void GXRenderTarget::SetTexture2DEnabled(bool enabled, u32 stage)
	{
	}

	void GXRenderTarget::SetUpVertexDescriptor()
	{
		mNumTextureUnits = 1;//std::min(mNumTexturesSources,mNumActiveTextures);
		
		GX_ClearVtxDesc();
		if(mVertex3Source)
		{
			GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		}else if(mVertex2Source)
		{
			GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
		}else
		{
			GX_SetVtxDesc(GX_VA_POS, GX_NONE);
		}
		
//		if(mNormalSource)
//		{
//			GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
//			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
//		}else
//		{
//			GX_SetVtxDesc(GX_VA_NRM, GX_NONE);
//		}
		
		if(mVertexColourSource || mColourSource)
		{
			GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GX_SetNumChans(1); //Colour = vertex colour
		}else
		{
			GX_SetVtxDesc(GX_VA_CLR0,GX_NONE);
			GX_SetNumChans(0);
		}
		GX_SetNumTexGens(mNumTextureUnits);
		GX_SetNumTevStages(mNumTextureUnits);
	
		if(mNumTextureUnits>0)
		{
			//TODO: Check the texture stages are being set properly.
			for(u32 i=0;i<mNumTextureUnits;++i)
			{
				if(mTextureUVSource[i])
				{
					GX_SetVtxDesc(GX_VA_TEX0+i, GX_DIRECT);
					GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0+i, GX_TEX_ST, GX_F32, 0);
					GX_SetTexCoordGen(GX_TEXCOORD0+i, GX_TG_MTX2x4, GX_TG_TEX0+i, GX_IDENTITY);
					GX_SetTevOp(GX_TEVSTAGE0+i, GX_MODULATE);
					GX_SetTevOrder(GX_TEVSTAGE0+i, GX_TEXCOORD0+i, GX_TEXMAP0+i, GX_COLOR0A0);
				}else
				{
					break;
				}
			}
		}else
		{
			//Colour only
			GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		}
	}
	
	inline void GXRenderTarget::SendDirectCommands(u32 i)
	{
		if(mVertex3Source)
		{
			Vector3& vertex=mVertex3Source[i];
			GX_Position3f32(vertex.x,vertex.y,vertex.z);
		}else if(mVertex2Source)
		{
			Vector2& vertex=mVertex2Source[i];
			GX_Position2f32(vertex.x,vertex.y);
		}
		
//		if(mNormalSource)
//		{
//			Vector3& normal=mNormalSource[i];
//			GX_Normal3f32(normal.x,normal.y,normal.z);
//		}
		
		if(mVertexColourSource)
		{
			GX_Color1u32(mVertexColourSource[i].mRGBA);
		}else if(mColourSource)
		{
			GX_Color1u32(static_cast<VertexColour>(mColourSource[i]).mRGBA);
		}
		
		for(u32 t=0;t<mNumTextureUnits;++t)
		{
			if(mTextureUVSource[t])
			{
				TextureUV& uv=mTextureUVSource[t][i];
				GX_TexCoord2f32(uv.u,uv.v);
			}else
			{
				//Texture coordinates must be sequential.
				break;
			}
		}
	}
	
	void GXRenderTarget::ResetTextureUnits()
	{
		//TODO
	}
	
	void GXRenderTarget::ClearSources()
	{
		mVertex3Source=0;
		mVertex2Source=0;
		mNormalSource=0;
		mVertexColourSource=0;
		mColourSource=0;
		mNumActiveTextures=0;
		mNumTexturesSources=0;
		mNumTextureUnits=0;
		
		for(u32 t=0;t<MAX_TEXTURE_UNITS;++t)
		{
			mTextureUVSource[t]=0;
		}
	}
	
	void GXRenderTarget::SetVertexSource(Vector2* source)
	{
		mVertex2Source = source;
	}

	void GXRenderTarget::SetVertexSource(Vector3* source)
	{
		mVertex3Source = source;
	}

	void GXRenderTarget::SetNormalSource(Vector3* source)
	{
		mNormalSource=source;
	}

	void GXRenderTarget::SetColourSource(VertexColour* source)
	{
		mVertexColourSource=source;
	}

	void GXRenderTarget::SetColourSource(Colour* source)
	{
		mColourSource=source;
	}

	void GXRenderTarget::SetTextureCoordinateSource(TextureUV* source, u32 set)
	{
		if(set>=MAX_TEXTURE_UNITS)
		{
			return;
		}
		mNumTexturesSources=std::max(mNumTexturesSources,set+1);
		mTextureUVSource[set]=source;
	}

	void GXRenderTarget::DrawTriangles(std::vector<u16>& indices)
	{
		size_t numIndices=indices.size();
		SetUpVertexDescriptor();

		GX_Begin(GX_TRIANGLES,GX_VTXFMT0,numIndices);
			for(size_t i=0;i<numIndices;++i)
			{
				SendDirectCommands(indices[i]);
			}
		GX_End();
	}

	void GXRenderTarget::DrawTriangleStrip(std::vector<u16>& indices)
	{
		size_t numIndices=indices.size();
		SetUpVertexDescriptor();
		GX_Begin(GX_TRIANGLESTRIP, GX_VTXFMT0,numIndices);
			for(size_t i=0;i<numIndices;++i)
			{
				SendDirectCommands(indices[i]);
			}
		GX_End();
	}

	void GXRenderTarget::DrawLines(std::vector<u16>& indices, f32 lineWidth)
	{
		GX_SetLineWidth(static_cast<u8>(lineWidth*16.f),0);
		SetUpVertexDescriptor();
		size_t numIndices=indices.size();
		GX_Begin(GX_LINES, GX_VTXFMT0,numIndices);
			for(size_t i=0;i<numIndices;++i)
			{
				SendDirectCommands(indices[i]);
			}
		GX_End();
	}

	void GXRenderTarget::DrawLineStrip(std::vector<u16>& indices, f32 lineWidth)
	{
		GX_SetLineWidth(static_cast<u8>(lineWidth*16.f),0);
		SetUpVertexDescriptor();
		size_t numIndices=indices.size();
		GX_Begin(GX_LINESTRIP, GX_VTXFMT0,numIndices);
			for(size_t i=0;i<numIndices;++i)
			{
				SendDirectCommands(indices[i]);
			}
		GX_End();
	}

	void GXRenderTarget::DrawPoints(std::vector<u16>& indices, f32 pointSize)
	{
		GX_SetPointSize(static_cast<u8>(pointSize*16.f),0);
		SetUpVertexDescriptor();
		size_t numIndices=indices.size();
		GX_Begin(GX_POINTS, GX_VTXFMT0,numIndices);
			for(size_t i=0;i<numIndices;++i)
			{
				SendDirectCommands(indices[i]);
			}
		GX_End();
	}

	void GXRenderTarget::SetDepthTestEnabled(bool enabled)
	{
		mZEnabled=enabled ? GX_TRUE : GX_FALSE;
		GX_SetZMode(mZEnabled, mDepthFunction, mZWriteEnabled);
	}

	void GXRenderTarget::SetDepthWriteEnabled(bool enabled)
	{
		mZWriteEnabled=enabled ? GX_TRUE : GX_FALSE;
		GX_SetZMode(mZEnabled, mDepthFunction, mZWriteEnabled);
	}

	void GXRenderTarget::SetBlendMode(const BlendMode& val)
	{
		//TODO: SetBlendMode.
		GX_SetBlendMode(GX_BM_BLEND,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA,0);
	}
	
	void GXRenderTarget::SetAlphaTest(const RenderPass::AlphaTestFunction& function, float referenceValue)
	{
		typedef RenderPass::AlphaTestFunctions AlphaTestFunctions;
		
		if(function==mAlphaTestFunction && referenceValue==mAlphaTestValue)
		{
			return;
		}
		u8 gxCompareFunction;
		switch(function)
		{
			case AlphaTestFunctions::NEVER:
				gxCompareFunction=GX_NEVER;
				break;
			case AlphaTestFunctions::LESS:
				gxCompareFunction=GX_LESS;
				break;
			case AlphaTestFunctions::EQUAL:
				gxCompareFunction=GX_EQUAL;
				break;
			case AlphaTestFunctions::LESS_OR_EQUAL:
				gxCompareFunction=GX_LEQUAL;
				break;
			case AlphaTestFunctions::GREATER:
				gxCompareFunction=GX_GREATER;
				break;
			case AlphaTestFunctions::NOT_EQUAL:
				gxCompareFunction=GX_NEQUAL;
				break;
			case AlphaTestFunctions::GREATER_OR_EQUAL:
				gxCompareFunction=GX_GEQUAL;
				break;
			case AlphaTestFunctions::ALWAYS:	//Fall through.
			default:
				gxCompareFunction=GX_ALWAYS;
				break;
		};
		u8 gxReferenceValue = static_cast<u8>(referenceValue*255.f);	//Convert 0-1 to 0-255
		
		// GX can perform two alpha comparison functions but Echo doesn't normally support it. If someone
		// specifically wants this functionality then it can be added to GXRenderTarget as a SetGXAlphaTest()
		// or something and the programmer can use RenderTarget directly in that case. This would obviously
		// break code portability.
		
		GX_SetAlphaCompare(gxCompareFunction,gxReferenceValue,GX_LO_AND,GX_ALWAYS,255);
		mAlphaTestValue = referenceValue;
		mAlphaTestFunction = function;
	}

	void GXRenderTarget::SetTextureStageBlendMode(const LayerBlendModeEx& val, u32 stage)
	{
		//TODO: SetTextureStageBlendMode.
		if(stage==0)
		{
			//GX_SetTevOp(GX_TEVSTAGE0+stage, GX_REPLACE);
		}else
		{
			//GX_SetTevOp(GX_TEVSTAGE0+stage, GX_BLEND);
		}
	}

	void GXRenderTarget::SetWrapModeU(const TextureUnit::TextureWrapMode& val)
	{
		if(mActiveTexture)
		{
			mActiveTexture->SetWrapModeU(val);
		}
	}

	void GXRenderTarget::SetWrapModeV(const TextureUnit::TextureWrapMode& val)
	{
		if(mActiveTexture)
		{
			mActiveTexture->SetWrapModeV(val);
		}
	}

	void GXRenderTarget::SetMinFilter(const TextureUnit::TextureFilter& val)
	{
		if(mActiveTexture)
		{
			mActiveTexture->SetMinFilter(val);
		}
	}

	void GXRenderTarget::SetMagFilter(const TextureUnit::TextureFilter& val)
	{
		if(mActiveTexture)
		{
			mActiveTexture->SetMagFilter(val);
		}
	}

	void GXRenderTarget::SetDepthFunction(const RenderPass::DepthFunction& val)
	{
		switch(val)
		{
			case RenderPass::DepthFunctions::NEVER:				mDepthFunction=GX_NEVER; break;
			case RenderPass::DepthFunctions::LESS:				mDepthFunction=GX_LESS; break;
			case RenderPass::DepthFunctions::EQUAL:				mDepthFunction=GX_EQUAL; break;
			case RenderPass::DepthFunctions::LESS_OR_EQUAL:		mDepthFunction=GX_LEQUAL; break;
			case RenderPass::DepthFunctions::GREATER:			mDepthFunction=GX_GREATER; break;
			case RenderPass::DepthFunctions::NOT_EQUAL:			mDepthFunction=GX_NEQUAL; break;
			case RenderPass::DepthFunctions::GREATER_OR_EQUAL:	mDepthFunction=GX_GEQUAL; break;
			case RenderPass::DepthFunctions::ALWAYS:
			default:
				mDepthFunction=GX_ALWAYS;
			break;
		}
		GX_SetZMode(mZEnabled, mDepthFunction, mZWriteEnabled);
	}

	void GXRenderTarget::SetCullMode(const RenderPass::CullMode& val)
	{
		//Because of winding direction from a LH to RH system the back and front are inverted.
		switch(val)
		{
			case RenderPass::CullModes::NONE: GX_SetCullMode(GX_CULL_NONE); break;
			case RenderPass::CullModes::BACK: GX_SetCullMode(GX_CULL_FRONT); break;
			case RenderPass::CullModes::FRONT: GX_SetCullMode(GX_CULL_BACK); break;
		}
	}

	void GXRenderTarget::SetAmbient(const Colour& val)
	{
		VertexColour vertColour = val;
		GX_SetChanAmbColor(GX_COLOR0, *reinterpret_cast<GXColor*>(&vertColour));
	}

	void GXRenderTarget::SetDiffuse(const Colour& val)
	{
		//TODO: SetDiffuse
		VertexColour vertColour = val;
		GX_SetChanMatColor(GX_COLOR0, *reinterpret_cast<GXColor*>(&vertColour));
	}

	void GXRenderTarget::SetSpecular(const Colour& val)
	{
		//TODO: SetSpecular
	}

	void GXRenderTarget::SetEmissive(const Colour& val)
	{
		//TODO: SetEmissive
	}

	void GXRenderTarget::SetShininess(f32 val)
	{
		//TODO: SetShininess
	}

	void GXRenderTarget::SetTexture(Texture* texture, u32 stage)
	{
		if(stage>=MAX_TEXTURE_UNITS)
		{
			return;
		}
		mActiveTexture = GetGXTexture(texture);
		mNumActiveTextures=std::max(mNumActiveTextures,stage+1);
		
		//TODO: Add support for Texture lookup tables.
		//if(textureGX->GetIsIndexed())
		//{
		//	GX_LoadTlut(textureGX->GetTextureLookupObject(),);
		//}
		assert(mActiveTexture->GetTextureObject() && "SetTexture() uninitialised texture object. It might be because of a unsupported format.");
		
		if(mActiveTexture->GetTextureObject())
		{
			GX_LoadTexObj(mActiveTexture->GetTextureObject(), GX_TEXMAP0+stage);
		}
	}

	void GXRenderTarget::SetMaterialColours(const Colour& ambient, const Colour& diffuse, const Colour& specular,
							const Colour& emissive, f32 shininess)
	{
		//TODO: SetMaterialColours
	}

	void GXRenderTarget::SetTexGen(const TextureUnit::TextureGenerationModeSet& texGen, u32 stage)
	{
		//TODO: SetTexGen
	}

	void GXRenderTarget::SetLight(u32 lightindex, Light* light)
	{
		//TODO: SetLight
	}

	void GXRenderTarget::SetSize(u32 width, u32 height)
	{
		//TODO: SetSize
	}
	
	GXTexture* GXRenderTarget::GetGXTexture(Texture* texture)
	{
		std::map<Texture*, GXTexture*>::iterator it = mTextureLookup.find(texture);
		GXTexture* textureGX;
		if(it==mTextureLookup.end())
		{
			textureGX = new GXTexture(*texture);
			mTextureLookup.insert(std::make_pair(texture,textureGX));
		}else
		{
			textureGX = it->second;
		}
		return textureGX;
	}

	bool GXRenderTarget::ActivateProgram(shared_ptr<ShaderProgram> shaderProgram)
	{
		ECHO_LOG_WARNING("GXRenderTarget does not support shaders.");
		return false;
	}
	
	void GXRenderTarget::DeactivateProgram(shared_ptr<ShaderProgram> shaderProgram)
	{
		
	}
	
	bool GXRenderTarget::BuildProgram(shared_ptr<ShaderProgram> shaderProgram)
	{
		ECHO_LOG_WARNING("GXRenderTarget does not support shaders.");
		return false;
	}

	void GXRenderTarget::SetVertexColourEnabled(bool enabled)
	{
		mVertexColourEnabled = enabled;
	}

	void GXRenderTarget::SetVertexBuffer(shared_ptr<VertexBuffer> vertexBuffer)
	{
		GX_ClearVtxDesc();
		if(!vertexBuffer)
		{
			return;
		}

		u32 vertexAttributeFormatIndex = GX_VTXFMT0;
		mVertexSize = 0;

		VertexBuffer::Accessor<Vector3> vertices = vertexBuffer->GetAccessor<Vector3>("Position");
		if(vertices)
		{
			GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
			GX_SetArray(GX_VA_POS, &(vertices[0]), vertexBuffer->GetStride());
			GX_SetVtxAttrFmt(vertexAttributeFormatIndex,GX_VA_POS,GX_POS_XYZ,GX_F32,0);
			mVertexSize++;
		}

		VertexBuffer::Accessor<Vector3> normals = vertexBuffer->GetAccessor<Vector3>("Normal");
		if(normals)
		{
			GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
			GX_SetArray(GX_VA_NRM, &(normals[0]), vertexBuffer->GetStride());
			GX_SetVtxAttrFmt(vertexAttributeFormatIndex,GX_VA_NRM,GX_NRM_XYZ,GX_F32,0);
			mVertexSize++;
		}

		u32 numChannels = 0;
		if(mVertexColourEnabled)
		{
			VertexBuffer::Accessor<VertexColour> colours = vertexBuffer->GetAccessor<VertexColour>("Colour");
			if(colours)
			{
				GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
				GX_SetArray(GX_VA_CLR0, &(colours[0]), vertexBuffer->GetStride());
				GX_SetVtxAttrFmt(vertexAttributeFormatIndex,GX_VA_CLR0,GX_CLR_RGBA,GX_U8,0);
				mVertexSize++;
				++numChannels;
			}else
			{
				GX_SetVtxDesc(GX_VA_CLR0,GX_NONE);
			}
		}
		GX_SetNumChans(numChannels);
		GX_SetNumTexGens(0);//mNumTextureUnits);
		GX_SetNumTevStages(0);//mNumTextureUnits);

		// Size maxStage = std::min();
		// VertexBuffer::Accessor<TextureUV> uvs = vertexBuffer->GetAccessor<TextureUV>("UV0");
		// if(uvs)
		// {
		// 	GX_SetVtxDesc(GX_VA_TEX0 GX_INDEX16);
		// 	GX_SetArray(GX_VA_TEX0, &(uvs[0]), vertexBuffer->GetStride());
		// 	GX_SetVtxAttrFmt(vertexAttributeFormatIndex,GX_VA_TEX0,GX_TEX_ST,GX_F32,0);
		//	mVertexSize++;
		// }
	}

	void GXRenderTarget::SetTextureCoordinateSourceIndex(Size /*set*/, Size /*index*/)
	{

	}

	void GXRenderTarget::DrawElements(const ElementBuffer& elementBuffer)
	{
		u8 type = GX_POINTS;
		switch(elementBuffer.GetElementType())
		{
			case ElementBuffer::ElementTypes::UNDEFINED:
				ECHO_LOG_ERROR("Element type undefined");
				return;
			case ElementBuffer::ElementTypes::POINTS:
				type = GX_POINTS;
			break;
			case ElementBuffer::ElementTypes::LINES:
				type = GX_LINES;
			break;
			case ElementBuffer::ElementTypes::LINE_STRIP:
				type = GX_LINESTRIP;
			break;
			case ElementBuffer::ElementTypes::LINE_LOOP:
				ECHO_LOG_ERROR("ElementTypes::LINE_LOOP is not supported");
				return;
			case ElementBuffer::ElementTypes::TRIANGLE:
				type = GX_TRIANGLES;
			break;
			case ElementBuffer::ElementTypes::TRIANGLE_STRIP:
				type = GX_TRIANGLESTRIP;
			break;
			case ElementBuffer::ElementTypes::TRIANGLE_FAN:
				type = GX_TRIANGLEFAN;
			break;
		}
		
		switch(elementBuffer.GetIndexType())
		{
			case ElementBuffer::IndexTypes::UNSIGNED_16BIT:
			{
				const ElementBuffer::Accessor<u16> indices = elementBuffer.GetAccessor<u16>();
				u16 numIndices = elementBuffer.GetNumberOfIndices();
				GX_Begin(type,GX_VTXFMT0,numIndices);
					for(size_t i=0;i<numIndices;++i)
					{
						for(size_t v=0;v<mVertexSize;++v)
						{
							// The functions:
							// 	GX_Position1x16(0);
							// 	GX_Normal1x16(0);
							//	GX_Color1x16(0);
							//	GX_TexCoord1x16(0);
							// And the x8 varients all do the same thing - send an index. The vertex descriptor defines
							// what the index corresponds to. So we use mVertexSize to send that many * num indices
							GX_Position1x16(indices[i]);
						}
					}
				GX_End();
			}
			break;
			case ElementBuffer::IndexTypes::UNSIGNED_32BIT:
			{
				ECHO_LOG_ERROR("Only 16 bit indices are supported");
				return;
			}
			case ElementBuffer::IndexTypes::UNDEFINED:
			{
				ECHO_LOG_ERROR("Index type is not defined");
			}return;
		}
	}

	void GXRenderTarget::SetPointAndLineSize(f32 pointAndLineSize)
	{
		GX_SetPointSize(static_cast<u8>(pointAndLineSize*16.f),0);
		GX_SetLineWidth(static_cast<u8>(pointAndLineSize*16.f),0);
	}

	void GXRenderTarget::SetCubeMap(CubeMapTexture* /*cubeMapTexture*/, u32 /*stage*/)
	{
		ECHO_LOG_WARNING("GXRenderTarget::SetCubeMap() unimplemented");
	}


}
