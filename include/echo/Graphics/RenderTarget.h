#ifndef _ECHORENDERTARGET_H_
#define _ECHORENDERTARGET_H_

#include <echo/Kernel/Kernel.h>
#include <echo/Graphics/BlendMode.h>
#include <echo/Graphics/TextureUnit.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/Graphics/PrimitiveTypes.h>
#include <echo/Graphics/Viewport.h>
#include <echo/Maths/Length.h>
#include <echo/Maths/Vector2.h>
#include <echo/Maths/Vector3.h>
#include <map>
#include <vector>

namespace Echo
{
	class Camera;
	class Texture;
	class CubeMapTexture;
	class Material;
	class Light;
	class VertexBuffer;
	class ElementBuffer;

	class RenderTarget
	{
	public:
		/**
		 * The clear mask is a bitfield of the following flags. When the bit is on
		 * it means that option will be cleared.
		 * To combine the options bitwise OR them together. For example:
		 *	ClearMask myMask = ClearMaskFlags::COLOUR | ClearMaskFlags::DEPTH;
		 * Sets the clear options to clear colour and depth buffers when Clear() is called.
		 * Using ClearMaskFlags::NONE in the field does nothing because to bitwise OR 0
		 * with something has no effect. ClearMaskFlags::NONE is provided so you can easily
		 * disable clearing everying.
		 *	ClearMask myMask = ClearMaskFlags::NONE;
		 * Use Renderer::SetClearMask() to apply the setting.
		 */
		struct ClearMaskFlags
		{
			enum _
			{
				NONE = 0,
				COLOUR = 1,
				DEPTH = 2,
				STENCIL = 4
			};
		};
		typedef size_t ClearMask;
	protected:
		Viewport* mCurrentViewport;
		Camera* mCurrentCamera;
		bool mIsPrimary;
		bool mIsFullScreen;
		f32 mDisplayXDPI;		//Display X DPI, 0 if unknown
		f32 mDisplayYDPI;		//Display Y DPI, 0 if unknown
	public:
		RenderTarget();
		virtual ~RenderTarget();

		virtual void SetPrimary(bool isPimary)
		{
			mIsPrimary = isPimary;
		}

		virtual bool Activate() = 0;
		virtual void Deactivate() = 0;
		
		virtual u32 GetWidth() const = 0;
		virtual u32 GetHeight() const = 0;
		virtual u32 GetBytesPerPixel() const = 0;
		
		virtual f32 GetAspectRatio() const;
		
		/**
		 * Used for APIs that have an external context that may be lost.
		 * This allows a render target to prevent rendering.
		 */
		virtual void ContextLost() = 0;

		/**
		 * Used for APIs that have an external context that may be restored.
		 * This allows a render target to continue rendering.
		 */
		virtual void ContextRestored() = 0;

		/**
		 * Get the projection matrix last set by a call to SetProjectionMatrix().
		*/
		virtual const Matrix4& GetProjectionMatrix() = 0;

		/**
		 * Set the projection matrix.
		 * It is recommended that render targets cache the untransformed projection matrix
		 * for any calls to GetProjectionMatrix().
		 */
		virtual void SetProjectionMatrix(const Matrix4& matrix, bool isOrtho) = 0;

		/**
		 * Set the model/view matrix for rendering.
		 * Some platforms may support independent model and view matrices but others do not.
		 * To work around the varying platform features we pretend that all platforms do not
		 * support setting multiple matrices for model and view. This just means that we
		 * combine the model and view matrices before rendering and set a single matrix with
		 * the rendering API.
		 */
		virtual void SetModelViewMatrix(const Matrix4& matrix) = 0;
		
		/**
		 * Set the texture coordinate transform matrix.
		 * @note Not all platforms will support texture coordinate transforms.
		 * @param matrix The transform matrix.
		 * @param stage which texture stage to apply this to.
		 */
		virtual void SetTextureMatrix(const Matrix4& matrix, u32 stage) = 0;
		
		/**
		 * Set the rendering viewport to the specified viewport.
		 * The render target will need to convert the viewport parameters to what is appropriate
		 * for the platform.
		 * @param vp the viewport.
		 */
		virtual void SetViewport(Viewport& vp) = 0;
		
		/**
		 * Set the scissor bounds for the frame buffer.
		 * Anything outside of the rectangle will be discarded when rendering.
		 * @param rectangle The rectangle in percent render target space.
		 */
		virtual void SetScissor(const Viewport::Rectangle& rectangle) = 0;
		
		/**
		 * Reset the scissor bounds for the frame buffer.
		 */
		virtual void ResetScissor() = 0;

		/**
		 * Request whether the render target should be synchronised to screen vertical refresh.
		 * @note It is platform specific as to whether this is possible.
		 * @note In some cases it may not even make sense to synchronise rendering of a render target
		 *		 to a screen vertical refresh, such as render to texture.
		 * @param enabled
		 */
		virtual void SetVSyncEnabled(bool enabled) = 0;
		
		/**
		 * Requests the target be made full screen or windowed.
		 * @note Not all render targets support this feature. Some platforms only operate in full screen mode.
		 * @param fullscreen
		 */
		virtual void SetFullScreen(bool fullscreen) = 0;
		/**
		 * Set the clear colour for the render target.
		 * @param colour
		 */
		virtual void SetClearColour(const Colour& colour) = 0;	
		/**
		 * Set the clear depth value for the depth buffer.
		 * The derived RenderTarget should perform appropriate range conversion.
		 * @param depth value clamped to range 0.f - 1.f.
		 */
		virtual void SetClearDepth(f32 depth) = 0;

		/**
		 * Set the clear mask to determine what buffers will be cleared when Clear() is called.
		 * @see ClearMaskFlags for more information on setting clearMask.
		 * @param clearMask see ClearMask.
		 */
		virtual void SetClearMask(ClearMask clearMask) = 0;

		virtual void Clear() = 0;
		virtual void ShowExternalCursors(bool show) = 0;
		virtual void SwapBuffers() = 0;
		
		virtual void SetName(const std::string& name) = 0;
		virtual std::string GetName() = 0;

		/////////// Functions for setting renderer state//////////////////////////////
		//It is important to use these when switching render system options to ensure
		//any underlying System/API maintains the correct state. For example, Echo's
		//GLRenderTarget maintains the internal state of OpenGL. If you change GL's
		//state directly then Echo's RenderTargets will be out of sync with OpenGL.
		
		virtual void SetLightingEnabled(bool enabled) = 0;
		virtual void SetVertexColourEnabled(bool enabled) = 0;
		virtual void SetMaterialColourEnabled(bool enabled) = 0;
		virtual void SetTexture2DEnabled(bool enabled, u32 stage) = 0;

		virtual void ResetTextureUnits() = 0;
		virtual void SetTextureUnit(TextureUnit& textureUnit, u32 stage);
		virtual void SetVertexBuffer(shared_ptr<VertexBuffer> vertexBuffer) = 0;
		virtual void ClearSources() = 0;
		virtual void SetVertexSource(Vector2* source) = 0;
		virtual void SetVertexSource(Vector3* source) = 0;
		virtual void SetNormalSource(Vector3* source) = 0;
		virtual void SetColourSource(VertexColour* source) = 0;
		virtual void SetColourSource(Colour* source) = 0;
		virtual void SetTextureCoordinateSource(TextureUV* source, u32 set) = 0;
		virtual void SetTextureCoordinateSourceIndex(Size set, Size index) = 0;
		virtual void SetPointAndLineSize(f32 pointAndLineSize) = 0;
		
		virtual void DrawElements(const ElementBuffer& elementBuffer) = 0;
		virtual void DrawTriangles(std::vector<u16>& indices) = 0;
		virtual void DrawTriangleStrip(std::vector<u16>& indices) = 0;
		virtual void DrawLines(std::vector<u16>& indices, f32 lineWidth) = 0;
		virtual void DrawLineStrip(std::vector<u16>& indices, f32 lineWidth) = 0;
		virtual void DrawPoints(std::vector<u16>& indices, f32 pointSize) = 0;

		virtual void SetDepthTestEnabled(bool enabled) = 0;
		virtual void SetDepthWriteEnabled(bool enabled) = 0;
		virtual void SetBlendMode(const BlendMode& val) = 0;
		virtual void SetAlphaTest(const RenderPass::AlphaTestFunction& function, float referenceValue) = 0;
		virtual void SetTextureStageBlendMode(const LayerBlendModeEx& val, u32 stage = 0) = 0;
		virtual void SetWrapModeU(const TextureUnit::TextureWrapMode& val) = 0;
		virtual void SetWrapModeV(const TextureUnit::TextureWrapMode& val) = 0;
		virtual void SetMinFilter(const TextureUnit::TextureFilter& val) = 0;
		virtual void SetMagFilter(const TextureUnit::TextureFilter& val) = 0;
		virtual void SetDepthFunction(const RenderPass::DepthFunction& val) = 0;
		virtual void SetCullMode(const RenderPass::CullMode& val) = 0;
		virtual void SetAmbient(const Colour& val) = 0;
		virtual void SetDiffuse(const Colour& val) = 0;
		virtual void SetSpecular(const Colour& val) = 0;
		virtual void SetEmissive(const Colour& val) = 0;
		virtual void SetShininess(f32 val) = 0;
		virtual void SetTexture(Texture* texture, u32 stage) = 0;
		virtual void SetMaterialColours(const Colour& ambient, const Colour& diffuse, const Colour& specular,
										const Colour& emissive, f32 shininess) = 0;
		virtual void SetTexGen(const TextureUnit::TextureGenerationModeSet& texGen, u32 stage = 0) = 0;
		virtual void SetCubeMap(CubeMapTexture* cubeMapTexture, u32 stage) = 0;

		virtual void SetLight(u32 lightindex, Light* light) = 0;

		void SetClearColourEnabled(bool enabled);
		void SetClearDepthEnabled(bool enabled);
		void SetClearStencilEnabled(bool enabled);
		
		void SetWrapMode(const TextureUnit::TextureWrapMode& val)
		{
			SetWrapModeU(val);
			SetWrapModeV(val);
		}

		void SetFilter(const TextureUnit::TextureFilter& val)
		{
			SetMinFilter(val);
			SetMagFilter(val);
		}

		virtual bool ActivateProgram(shared_ptr<ShaderProgram> shaderProgram) = 0;
		virtual void DeactivateProgram(shared_ptr<ShaderProgram> shaderProgram) = 0;
		virtual bool BuildProgram(shared_ptr<ShaderProgram> shaderProgram) = 0;
		///////////////////////////////////////////////////

		void SetDisplayXDPI(f32 displayXDPI)
		{
			mDisplayXDPI = displayXDPI;
		}

		void SetDisplayYDPI(f32 displayYDPI)
		{
			mDisplayYDPI = displayYDPI;
		}

		f32 GetDisplayXDPI() const
		{
			return mDisplayXDPI;
		}

		f32 GetDisplayYDPI() const
		{
			return mDisplayYDPI;
		}
		
		/**
		 * Get the physical width of the display.
		 * The physical width can only be calculated when the DPI is known. On some
		 * platforms the DPI is only an estimate or a reasonable default for one
		 * reason other another. Because of this, this method may only return an
		 * estimate.
		 * @return The physical width of the display if known, otherwise Metre(0).
		 */
		Metre GetDisplayPhysicalWidth() const;

		/**
		 * Get the physical height of the display.
		 * The physical height can only be calculated when the DPI is known. On some
		 * platforms the DPI is only an estimate or a reasonable default for one
		 * reason other another. Because of this, this method may only return an
		 * estimate.
		 * @return The physical height of the display if known, otherwise Metre(0).
		 */
		Metre GetDisplayPhysicalHeight() const;

		/**
		 * Get the physical diagonal size of the display.
		 * Display sizes are often expressed in a single measurement such as "7 inches",
		 * this refers to the diagonal measurement of the screen. This method uses
		 * GetDisplayPhysicalWidth() and GetDisplayPhysicalHeight() so may only return
		 * an estimate, see the documentation for these methods for more information.
		 * @see GetDisplayPhysicalWidth().
		 * @see GetDisplayPhysicalHeight().
		 * @return The physical diagonal size of the screen, Metre(0) if unknown.
		 */
		Metre GetDisplayPhysicalDiagonalSize() const;

		virtual void SetSize(u32 widthInPixels, u32 heightInPixels) = 0;
		
		void SetColourDepth(u32 colourDepth);

		Viewport* GetCurrentViewport() const
		{
			return mCurrentViewport;
		}

		/**
		 * Set the current camera for reference to any renderable that might need it.
		 * This is usually called by a Camera when it renders a Scene, but if you are
		 * manually preparing a render target to render you may need to call it yourself.
		 * Since we are using a raw pointer here, when setting the current camera you
		 * should also remember to set the current camera back to nullptr when you
		 * have finished with the render target.
		 * @param camera The camera to 
		 */
		void SetCurrentCamera(Camera* camera)
		{
			mCurrentCamera = camera;
		}
		
		Camera* GetCurrentCamera() const
		{
			return mCurrentCamera;
		}

		bool IsFullScreen() const
		{
			return mIsFullScreen;
		}

		bool IsPrimary() const
		{
			return mIsPrimary;
		}
	};
}
#endif 
