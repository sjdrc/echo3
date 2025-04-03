#include <echo/Resource/3dsReader.h>
#include <echo/Resource/MeshManager.h>
#include <echo/Resource/MeshReader.h>
#include <echo/Util/StringUtils.h>
#include <echo/Graphics/SubMesh.h>
#include <echo/Graphics/Mesh.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/cpp/functional>

namespace Echo
{
	MeshManager::MeshManager(FileSystem& fileSystem, MaterialManager& materialManager, SkeletonManager& skeletonManager) :
		ResourceManager<Mesh>("mesh"),
		mFileSystem(fileSystem),
		mMaterialManager(materialManager),
		mSkeletonManager(skeletonManager),
		mMeshNames(0)
	{
		RegisterLoader("3ds", bind(&Load3DS,placeholders::_1,placeholders::_2,placeholders::_3));
		RegisterLoader("mesh", bind(&LoadMesh,placeholders::_1,placeholders::_2, ref(mMaterialManager),ref(mSkeletonManager),placeholders::_3));
	}

	MeshManager::~MeshManager()
	{

	}

	bool MeshManager::RegisterLoader(const std::string& fileExtension, MeshLoaderFunc loaderFunction)
	{
		if(mMeshLoaders.find(fileExtension)!=mMeshLoaders.end())
		{
			ECHO_LOG_ERROR("MeshManager::RegisterLoader() loader for extension \"" << fileExtension << "\" already registered");
			return false;
		}
		mMeshLoaders[fileExtension]=loaderFunction;
		return true;
	}

	bool MeshManager::LoadIntoResource(const std::string& resourceName, Mesh& mesh)
	{
		//We need to find the resource file name.
		std::string resourceFile = GetResourceFileName(resourceName);
		if(resourceFile.empty())
		{
			return false;
		}
				
		//Determine the extension and load accordingly
		Size lastDot=0;
		Size extensionLength=0;
		std::string extension;
		lastDot=resourceFile.find_last_of(".");
		lastDot++;			//Want the first pos past the dot
		extensionLength=resourceFile.size()-lastDot;
		extension=resourceFile.substr(lastDot,extensionLength);

		//Must load
		std::map<std::string, MeshLoaderFunc >::iterator eit=mMeshLoaders.find(extension);
		if(eit==mMeshLoaders.end())
		{
			ECHO_LOG_ERROR("MeshManager::LoadResource() No Loader found for extension \"" << extension << "\" for file \"" << resourceFile << "\"");
		}else
		{
			if((eit->second)(resourceFile,mFileSystem,mesh))
			{
				return true;
			}
			ECHO_LOG_ERROR("MeshManager::LoadResource() Failed to load resource \"" << resourceFile << "\"");
		}
		return false;
	}

	shared_ptr<Mesh> MeshManager::LoadResource(const std::string& resourceFile, const std::string& resourceName)
	{
		//Determine the extension and load accordingly
		Size lastDot=0;
		Size extensionLength=0;
		std::string extension;
		lastDot=resourceFile.find_last_of(".");
		lastDot++;			//Want the first pos past the dot
		extensionLength=resourceFile.size()-lastDot;
		extension=resourceFile.substr(lastDot,extensionLength);

		//Must load
		std::map<std::string, MeshLoaderFunc >::iterator eit=mMeshLoaders.find(extension);
		if(eit==mMeshLoaders.end())
		{
			ECHO_LOG_ERROR("MeshManager::LoadResource() No Loader found for extension \"" << extension << "\" for file \"" << resourceFile << "\"");
		}else
		{
			shared_ptr<Mesh> resource(new Mesh());
			if((eit->second)(resourceFile,mFileSystem,*resource))
			{
				resource->SetName(resourceName);
				return resource;
			}
			ECHO_LOG_ERROR("MeshManager::LoadResource() Failed to load resource \"" << resourceFile << "\"");
		}
		return nullptr;
	}

	shared_ptr<Mesh> MeshManager::CreateMesh(const std::string& resourceName)
	{
		shared_ptr<Mesh > mesh(new Mesh());
		mesh->SetName(resourceName);
		ResourceManager<Mesh>::AddResource(resourceName,mesh);
		return ResourceManager<Mesh>::GetResource(resourceName);
	}

	bool MeshManager::Load3DS( const std::string& resourceFile, FileSystem& fileSystem, Mesh& mesh)
	{
		File file = fileSystem.Open(resourceFile);
		if(!file.IsOpen())
		{
			return false;
		}

		shared_ptr< std::vector<Vector3> > vertices(new std::vector<Vector3>);
		std::vector<u16> indices;
		std::vector< std::vector<TextureUV> > textureCoordinates;
		
		if(!Read3DSFile(file,*vertices,indices,textureCoordinates))
		{
			ECHO_LOG_ERROR("Load3DS: Failed to read 3DS file.");
			return false;
		}

		shared_ptr<SubMesh> subMesh=mesh.CreateSubMesh();
		shared_ptr<VertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
		vertexBuffer->AddVertexAttribute("Position",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3, 1));	//Position
		vertexBuffer->AddVertexAttribute("Normal",VertexAttribute(VertexAttribute::ComponentTypes::VECTOR3, 1));	//Normal
		vertexBuffer->AddVertexAttribute("Colour",VertexAttribute(VertexAttribute::ComponentTypes::COLOUR_8, 1));	//Colour

		std::vector< VertexBuffer::Accessor<TextureUV> > uvs;

		for(u32 t=0;t<textureCoordinates.size();++t)
		{
			std::stringstream ss;
			ss << "UV" << t;
			vertexBuffer->AddVertexAttribute(ss.str(),VertexAttribute(VertexAttribute::ComponentTypes::TEXTUREUV, 1));
		}


		Size numberOfVertices = vertices->size();
		vertexBuffer->Allocate(numberOfVertices);

		for(u32 t=0;t<textureCoordinates.size();++t)
		{
			std::stringstream ss;
			ss << "UV" << t;
			uvs.push_back(vertexBuffer->GetAccessor<TextureUV>(ss.str()));
		}

		// TODO: We're copying this now because I'm trying to get a version one of general vertex buffer usage
		// but eventually we should pass the buffer in and have the loader determine how many vertices we need.
		
		VertexBuffer::Accessor<Vector3> destVertices = subMesh->GetComponents<Vector3>("Position");
		VertexBuffer::Accessor<VertexColour> destColours = subMesh->GetComponents<VertexColour>("Colour");
		for(Size i=0;i < numberOfVertices;++i)
		{
			destVertices[i] = (*vertices)[i];
			destColours[i] = Colour();			// Apparently we don't load colours.
			for(Size j=0;j<uvs.size();++j)
			{
				uvs[j][i] = textureCoordinates[j][i];
			}
		}
		
		// Copy the indices - NOTE: Indices will be 16bit
		subMesh->SetElementBuffer(ElementBuffer::Types::STATIC,ElementBuffer::IndexTypes::UNSIGNED_16BIT,ElementBuffer::ElementTypes::TRIANGLE,indices.size()/3);
		shared_ptr<ElementBuffer> elementBuffer=subMesh->GetElementBuffer();
		auto targetIndices = elementBuffer->GetIndexAccessor<u16>();
		for(Size i=0; i< indices.size(); ++i)
		{
			targetIndices[i] = indices[i];
		}
		
		// Apparently we also don't load normals..
		subMesh->GenerateNormals();
		mesh.GetAxisAlignedBox();
		return true;
	}

	bool MeshManager::LoadMesh( const std::string& resourceFile, FileSystem& fileSystem, MaterialManager& materialManager, SkeletonManager& skeletonManager, Mesh& mesh)
	{
		File file = fileSystem.Open(resourceFile);
		if(!file.IsOpen())
		{
			return false;
		}
		return ReadMeshFile(file, materialManager,skeletonManager, mesh);
	}

	FileSystem* MeshManager::GetFileSystem() const
	{
		return &mFileSystem;
	}
}
