#ifndef _ECHOMESHREADER_H_
#define _ECHOMESHREADER_H_
namespace Echo
{
	class Mesh;
	class File;
	class MaterialManager;
	class SkeletonManager;
	bool ReadMeshFile(File& file, MaterialManager& materialManager, SkeletonManager& skeletonManager, Mesh& targetMesh);
}

#endif
