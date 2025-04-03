#include <echo/Platforms/GL/GLSupport.h>

#ifdef ECHO_RENDERER_GL
#ifndef _ECHOGLRENDERTARGET_H_
#define _ECHOGLRENDERTARGET_H_

#include <echo/Graphics/RenderTarget.h>
#include <echo/Graphics/BlendMode.h>
#include <echo/Platforms/GL/GLContext.h>
#include <echo/Resource/ResourceDelegate.h>

namespace Echo
{
	class GLTexture;
	
	class GLRenderTarget : public RenderTarget, public ResourceListener<Texture>, public ResourceListener<CubeMapTexture>, public ResourceListener<VertexBuffer>
	{
	public:
		GLRenderTarget(shared_ptr<GLContext> context);
		virtual ~GLRenderTarget();

		const u32& GetMaxTextureStages() const
		{
			return mMaxTextureStages;
		}
		void DetermineCapabilities();

		virtual bool Activate() override;
		virtual void Deactivate() override;
		
		virtual u32 GetWidth() const override;
		virtual u32 GetHeight() const override;
		virtual u32 GetBytesPerPixel() const override;
		
		virtual void ContextLost() override;
		virtual void ContextRestored() override;
		
		virtual void SetLightingEnabled(bool enabled) override;
		virtual void SetVertexColourEnabled(bool enabled) override;
		virtual void SetMaterialColourEnabled(bool enabled) override;
		virtual void SetTexture2DEnabled(bool enabled, u32 stage) override;
		
		virtual void ResetTextureUnits() override;
		virtual void ClearSources() override;
		virtual void SetVertexBuffer(shared_ptr<VertexBuffer> vertexBuffer) override;
		virtual void SetVertexSource(Vector2* source) override;
		virtual void SetVertexSource(Vector3* source) override;
		virtual void SetNormalSource(Vector3* source) override;
		virtual void SetColourSource(VertexColour* source) override;
		virtual void SetColourSource(Colour* source) override;
		virtual void SetTextureCoordinateSource(TextureUV* source, u32 set) override;
		virtual void SetTextureCoordinateSourceIndex(Size set, Size index) override;
		virtual void SetPointAndLineSize(f32 pointAndLineSize) override;
		
		virtual void DrawElements(const ElementBuffer& elementBuffer) override;
		virtual void DrawTriangles(std::vector<u16>& indices) override;
		virtual void DrawTriangleStrip(std::vector<u16>& indices) override;
		virtual void DrawLines(std::vector<u16>& indices, f32 lineWidth) override;
		virtual void DrawLineStrip(std::vector<u16>& indices, f32 lineWidth) override;
		virtual void DrawPoints(std::vector<u16>& indices, f32 pointSize) override;
		
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

		virtual void SetDepthTestEnabled(bool enabled) override;
		virtual void SetDepthWriteEnabled(bool enabled) override;
		virtual void SetTexture(Texture* texture, u32 stage) override;
		virtual void SetCubeMap(CubeMapTexture* cubeMapTexture, u32 stage) override;
		virtual void SetBlendMode(const BlendMode& val) override;
		virtual void SetAlphaTest(const RenderPass::AlphaTestFunction& function, float referenceValue) override;
		virtual void SetTextureStageBlendMode(const LayerBlendModeEx& bm, u32 stage = 0) override;
		virtual void SetTexGen(const TextureUnit::TextureGenerationModeSet& texGen, u32 stage = 0) override;
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
		virtual void SetMaterialColours(const Colour& ambient, const Colour& diffuse, const Colour& specular, const Colour& emissive, f32 shininess) override;

		virtual void SwapBuffers() override;
		
		virtual bool ActivateProgram(shared_ptr<ShaderProgram> shaderProgram) override;
		virtual void DeactivateProgram(shared_ptr<ShaderProgram> shaderProgram) override;
		virtual bool BuildProgram(shared_ptr<ShaderProgram> shaderProgram) override;

		void SetLight(u32 lightindex, Light* light) override;
		
		/**
		 * ResourceListener overrides
		 */
		virtual void OnResourceDestroyed(Texture* texture) override;
		virtual void OnResourceUnloaded(Texture* texture) override;

		virtual void OnResourceDestroyed(CubeMapTexture* cubeMapTexture) override;
		virtual void OnResourceUnloaded(CubeMapTexture* cubeMapTexture) override;

		virtual void OnResourceDestroyed(VertexBuffer* vertexBuffer) override;
		virtual void OnResourceUnloaded(VertexBuffer* vertexBuffer) override;
	protected:
		u32 mWidth;
		u32 mHeight;
		u32 mBytesPerPixel;
		u32 mMaxTextureStages;
		u32 mMaxFixedLights;
		u32 mClearMask;
		f32 mPointAndLineSize;

		shared_ptr<GLContext> mContext;
		shared_ptr< ResourceDelegate<Texture> > mTextureDelegate;
		shared_ptr< ResourceDelegate<CubeMapTexture> > mCubeMapTextureDelegate;
		shared_ptr< ResourceDelegate<VertexBuffer> > mVertexBufferDelegate;
		
		/**
		 * Get the GLTexture object associated with the specified Texture.
		 * Method always succeeds with either a new GLTexture or an existing one.
		 */
		shared_ptr<GLTexture> GetGLTexture(Texture* texture);
		shared_ptr<GLCubeMapTexture> GetGLCubeMapTexture(CubeMapTexture* cubeMap);
		shared_ptr<GLShaderProgram> GetGLShaderProgram(ShaderProgram* shaderProgram, bool buildIfNotFound);
		shared_ptr<GLVertexBuffer> GetGLVertexBuffer(VertexBuffer* vertexBuffer);
		
		static Matrix4 mProjectionMatrix;
		Colour mManualBlendColours[16][2];

		void _SetMaxTextureStages(const u32& val)
		{
			mMaxTextureStages = val;
		}
		bool _SetActiveTextureStage(u32 stage);
		void EnableDepthTestForOnlyDepthWrite();
	};
}
#endif 
#endif
