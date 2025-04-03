#ifndef _ECHOMATERIALMANAGER_H_
#define _ECHOMATERIALMANAGER_H_

#include <echo/Resource/ResourceManager.h>
#include <echo/Graphics/RenderPass.h>
#include <echo/Graphics/TextureUnit.h>
#include <echo/Graphics/Material.h>
#include <echo/Schema/Material.capnp.h>

namespace Echo
{
	namespace Resources
	{
		class Material;
	}
	class ShaderManager;
	class FileSystem;

	class MaterialManager : public ResourceManager<Material>
	{
	public:
		MaterialManager(FileSystem& fileSystem, TextureManager& textureManager,
					ShaderManager& geometryShaderManager,
					ShaderManager& vertexShaderManager,
					ShaderManager& fragmentShaderManager);
		~MaterialManager();

		/**
		 * Create a material resource that is managed by the material manager.
		 * Creating a material this way allows other objects to aquire the resource by name. You can create
		 * material objects yourself if you plan on keeping them private.
		 * @param resourceName the name of the material resource, this can be used later to acquire the resource with GetResource().
		 * @return The new material.
		 */
		shared_ptr<Material> CreateMaterial(const std::string& resourceName);

		/**
		 * Create a default material using an image.
		 * @param imageName the name of the image file, this also becomes the resource name.
		 * @return If the file cannot be found then method returns a null pointer, otherwise the pointer points to a valid
		 * material object.
		 */
		shared_ptr<Material> CreateMaterialUsingImage(const std::string& imageFileName);

		/**
		 * Load a material resource implementation.
		 * @note Providing resourceName does not make the resource available from GetResource(), use AddResource()
		 * followed by GetResource() instead and this method will be called automatically.
		 * @param resourceFile The file to load
		 * @param resourceName The name of the resource once loaded. This name is given to the resource.
		 * @return If there is an error the pointer returned will be null.
		 */
		shared_ptr<Material> LoadResource(const std::string& resourceFile, const std::string& resourceName) override;

		bool LoadIntoResource(const std::string& resourceName, Material& materialToLoadInto) override;

		/**
		 * Adds to a material from resource data.
		 * @param md The material data
		 * @param materialName The name of the Material if the material data does not contain a name.
		 * @param materialFile The file that this data came from, used for resolving relative file names.
		 * @param material The material to build into.
		 */
		void ParseMaterial(Resources::Material::Reader md, const std::string& materialName, const std::string& materialFile, Material& material);

		FileSystem* GetFileSystem() const override;

		/**
		 * Helper methods for converting resource data.
		 */
		static TextureUnit::TextureFilter ConvertTextureFilter(Resources::Material::TextureFilter textureFilterData, TextureUnit::TextureFilter defaultTextureFilter);
		static TextureUnit::TextureWrapMode ConvertTextureWrapMode(Resources::Material::TextureWrapMode mode, TextureUnit::TextureWrapMode defaultTextureWrapMode) ;
		static TextureUnit::TextureGenerationMode ConvertTextureGenerationMode(Resources::Material::TextureGeneration::Mode mode, TextureUnit::TextureGenerationMode defaultTextureGenerationMode);
		static TextureUnit::TextureGenerationModeSet::PlaneData ConvertPlaneData(Resources::Material::TextureGeneration::Plane::Reader plane);
		static LayerBlendSource ConvertLayerBlendSource(Resources::Material::Blend::BlendSource source, LayerBlendSource defaultLayerBlendSource);
		static LayerBlendOperationEx ConvertLayerBlendOperationEx(Resources::Material::Blend::BlendOperation operation, LayerBlendOperationEx defaultLayerBlendOperationEx);
		static Colour ConvertColour(Resources::Material::Colour::Reader colour);
		static BlendMode ConvertBlendMode(Resources::Material::BlendMode mode, BlendMode defaultBlendMode);
		static RenderPass::DepthFunction ConvertDepthFunction(Resources::Material::DepthFunction depthFunction, RenderPass::DepthFunction defaultDepthFunction);
		static RenderPass::CullMode ConvertCullMode(Resources::Material::CullMode mode, RenderPass::CullMode defaultCullMode);
		static RenderPass::AlphaTestFunction ConvertAlphatestFunction(Resources::Material::AlphaTestFunction alphaTestFunction, RenderPass::AlphaTestFunction defaultAlphaTestFunction);
	private:
		bool ProcessListEntry(const std::string& key, const std::string& resourceFile, const std::vector<std::string>& params) override;
		FileSystem& mFileSystem;
		TextureManager& mTextureManager;
		ShaderManager& mGeometryShaderManager;
		ShaderManager& mVertexShaderManager;
		ShaderManager& mFragmentShaderManager;
	};
}
#endif 
