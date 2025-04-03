#ifndef ECHO_TERRAIN_H
#define ECHO_TERRAIN_H

#include <echo/Graphics/SceneEntity.h>

namespace Echo
{
	class MeshManager;
	class TextureManager;
	class Texture;
	class Mesh;
	
	class Terrain : public SceneEntity
	{
	public:
		struct PixelModes
		{
			enum _
			{
				PIXEL_IS_VERTEX,		///Each pixel is a vertex. 
				PIXEL_IS_QUAD			///Each pixel represents the centre of a quad.
										///mResolution determines the size of each quad.
			};
		};
		typedef PixelModes::_ PixelMode;
		Terrain(const std::string& terrainName, MeshManager& meshManager, TextureManager& textureManager);
		~Terrain();
		
		void SetTexture(const std::string& textureName);
		
		/**
		 * Set the pixel mode for the terrain.
		 * @see PixelModes.
		 * @param pixelMode
		 */
		void SetPixelMode(PixelMode pixelMode);
		
		/**
		 * Set the resolution that each pixel represents.
		 * @param resolution
		 */
		void SetResolution(const Vector3& resolution);
		
		/**
		 * Update the internal mesh.
		 */
		void UpdateMesh();
		
		
		virtual void Render(RenderContext& renderContext, Colour compoundDiffuse) override;
	private:
		/**
		 * Build the mesh using vertex mode.
		 */
		void BuildVertexMesh();
		
		/**
		 * Build the mesh using quad mode.
		 */
		void BuildQuadMesh();
		MeshManager& mMeshManager;
		TextureManager& mTextureManager;
		bool mMeshOutOfDate;
		PixelMode mPixelMode;
		Vector3 mResolution;
		shared_ptr<Texture> mTexture;
	};
}
#endif 
