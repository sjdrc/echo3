#ifndef _GXRENDERTARGET_H_
#define _GXRENDERTARGET_H_

#include <gccore.h>
#include <ogcsys.h>
#include <echo/Graphics/RenderTarget.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/Graphics/TextureUnit.h>
#include <echo/Graphics/Viewport.h>

namespace Echo
{
	class GXTexture;

	class GXRenderTarget : public RenderTarget
	{
	private:
		void *mExternalFrameBuffer[2];
		u32 mCurrentBuffer;
		GXRModeObj *mRMode;
		u32 mClearDepth;
		void *mGraphicsFIFO;
		GXColor mBackgroundColour;
		Matrix4 mProjectionMatrix;
		int mZEnabled;
		int mDepthFunction;
		int mZWriteEnabled;
		std::string mName;
		Kernel& mKernel;
		static const u32 MAX_TEXTURE_UNITS=1;
		
		//These are copies of the various source values when set.
		Vector2* mVertex2Source;
		Vector3* mVertex3Source;
		Vector3* mNormalSource;
		VertexColour* mVertexColourSource;
		Colour* mColourSource;
		TextureUV* mTextureUVSource[8];
		size_t mNumActiveTextures;
		size_t mNumTexturesSources;
		size_t mNumTextureUnits;
		bool mScissorEnabled;
		Viewport::Rectangle mScissorRectangle;
		
		RenderPass::AlphaTestFunction mAlphaTestFunction;
		float mAlphaTestValue;
		bool mVertexColourEnabled;
		u32 mVertexSize;
		
		std::map<Texture*, GXTexture*> mTextureLookup;

		void SetUpVertexDescriptor();
		inline void SendDirectCommands(u32 i);
		GXTexture* GetGXTexture(Texture* texture);
		GXTexture* mActiveTexture;		/// The "active" texture object to perform modifications to.
	public:
		GXRenderTarget(Kernel& kernel);
		virtual ~GXRenderTarget();
		
		Kernel& GetKernel(){return mKernel;}

		virtual bool Activate() override;
		virtual void Deactivate() override;
		
		virtual u32 GetWidth() const override;
		virtual u32 GetHeight() const override;
		virtual u32 GetBytesPerPixel() const override;
		
		virtual void ContextLost() override;

		virtual void ContextRestored() override;

		virtual const Matrix4& GetProjectionMatrix() override;

		virtual void SetProjectionMatrix(const Matrix4& matrix, bool isOrtho) override;
		virtual void SetModelViewMatrix(const Matrix4& matrix) override;
		
		virtual void SetTextureMatrix(const Matrix4& matrix, u32 stage) override;
		
		virtual void SetViewport(Viewport& vp) override;
		
		virtual void SetScissor(const Viewport::Rectangle& rectangle) override;
		virtual void ResetScissor() override;
		
		virtual void SetVSyncEnabled(bool enabled) override;
		
		virtual void SetFullScreen(bool fullscreen) override;
		virtual void SetClearColour(const Colour& colour) override;
		virtual void SetClearDepth(f32 depth) override;

		virtual void SetClearMask(ClearMask clearMask) override;
		virtual void Clear() override;
		virtual void ShowExternalCursors(bool show) override;
		virtual void SwapBuffers() override;
		
		virtual void SetName(const std::string& name) override;
		virtual std::string GetName() override;

		virtual void SetLightingEnabled(bool enabled) override;
		virtual void SetMaterialColourEnabled(bool enabled) override;
		virtual void SetTexture2DEnabled(bool enabled, u32 stage) override;

		virtual void ResetTextureUnits() override;
		virtual void ClearSources() override;
		virtual void SetVertexSource(Vector2* source) override;
		virtual void SetVertexSource(Vector3* source) override;
		virtual void SetNormalSource(Vector3* source) override;
		virtual void SetColourSource(VertexColour* source) override;
		virtual void SetColourSource(Colour* source) override;
		virtual void SetTextureCoordinateSource(TextureUV* source, u32 set) override;
		
		virtual void DrawTriangles(std::vector<u16>& indices) override;
		virtual void DrawTriangleStrip(std::vector<u16>& indices) override;
		virtual void DrawLines(std::vector<u16>& indices, f32 lineWidth) override;
		virtual void DrawLineStrip(std::vector<u16>& indices, f32 lineWidth) override;
		virtual void DrawPoints(std::vector<u16>& indices, f32 pointSize) override;

		virtual void SetDepthTestEnabled(bool enabled) override;
		virtual void SetDepthWriteEnabled(bool enabled) override;
		virtual void SetBlendMode(const BlendMode& val) override;
		virtual void SetAlphaTest(const RenderPass::AlphaTestFunction& function, float referenceValue) override;
		virtual void SetTextureStageBlendMode(const LayerBlendModeEx& val, u32 stage) override;
		virtual void SetWrapModeU(const TextureUnit::TextureWrapMode& val) override;
		virtual void SetWrapModeV(const TextureUnit::TextureWrapMode& val) override;
		virtual void SetMinFilter(const TextureUnit::TextureFilter& val) override;
		virtual void SetMagFilter(const TextureUnit::TextureFilter& val) override;
		virtual void SetDepthFunction(const RenderPass::DepthFunction& val) override;
		virtual void SetCullMode(const RenderPass::CullMode& val) override;
		virtual void SetAmbient(const Colour& val) override;
		virtual void SetDiffuse(const Colour& val) override;
		virtual void SetSpecular(const Colour& val) override;
		virtual void SetEmissive(const Colour& val) override;
		virtual void SetShininess(f32 val) override;
		virtual void SetTexture(Texture* texture, u32 stage) override;
		virtual void SetMaterialColours(const Colour& ambient, const Colour& diffuse, const Colour& specular,
										const Colour& emissive, f32 shininess) override;
		virtual void SetTexGen(const TextureUnit::TextureGenerationModeSet& texGen, u32 stage) override;

		virtual void SetLight(u32 lightindex, Light* light) override;

		virtual void SetSize(u32 width, u32 height) override;

		virtual bool ActivateProgram(shared_ptr<ShaderProgram> shaderProgram) override;
		virtual void DeactivateProgram(shared_ptr<ShaderProgram> shaderProgram) override;
		virtual bool BuildProgram(shared_ptr<ShaderProgram> shaderProgram) override;

		virtual void SetVertexColourEnabled(bool enabled) override;
		virtual void SetVertexBuffer(shared_ptr<VertexBuffer> vertexBuffer) override;
		virtual void SetTextureCoordinateSourceIndex(Size set, Size index) override;
		virtual void DrawElements(const ElementBuffer& elementBuffer) override;
		virtual void SetPointAndLineSize(f32 pointAndLineSize)  override;
		virtual void SetCubeMap(CubeMapTexture* cubeMapTexture, u32 stage)  override;
	};
}

#endif 
