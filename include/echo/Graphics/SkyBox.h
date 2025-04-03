#ifndef ECHO_SKYBOX_H
#define ECHO_SKYBOX_H

#include <echo/Graphics/SceneEntity.h>

namespace Echo
{
	class MeshManager;
	class TextureManager;
	class Texture;
	class Mesh;
	class Material;
	class MaterialManager;
	
	class SkyBox : public SceneEntity
	{
	public:
		SkyBox(const std::string& name, MeshManager& meshManager, TextureManager& textureManager, MaterialManager& materialManager);
		~SkyBox();
		
		void SetTexture(const std::string& textureName);
		
		void SetTextureLeft(const std::string& textureName);
		void SetTextureRight(const std::string& textureName);
		void SetTextureFront(const std::string& textureName);
		void SetTextureBack(const std::string& textureName);
		void SetTextureTop(const std::string& textureName);
		void SetTextureBottom(const std::string& textureName);
		
		/**
		 * Update the internal mesh.
		 */
		void UpdateMesh();
		
		void SetDistance(f32 distance);
		
		virtual void Render(RenderContext& renderContext, Colour compoundDiffuse) override;
	private:
		MeshManager& mMeshManager;
		TextureManager& mTextureManager;
		MaterialManager& mMaterialManager;
		bool mMeshOutOfDate;
		f32 mDistance;
		shared_ptr<Material> mMaterial;
		shared_ptr<Material> mMaterialLeft;
		shared_ptr<Material> mMaterialRight;
		shared_ptr<Material> mMaterialFront;
		shared_ptr<Material> mMaterialBack;
		shared_ptr<Material> mMaterialTop;
		shared_ptr<Material> mMaterialBottom;
		shared_ptr<Texture> mTexture;
		shared_ptr<Texture> mTextureLeft;
		shared_ptr<Texture> mTextureRight;
		shared_ptr<Texture> mTextureFront;
		shared_ptr<Texture> mTextureBack;
		shared_ptr<Texture> mTextureTop;
		shared_ptr<Texture> mTextureBottom;
	};
}
#endif 
