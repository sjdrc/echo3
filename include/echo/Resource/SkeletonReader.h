#ifndef _ECHOSKELETONREADER_H_
#define _ECHOSKELETONREADER_H_

namespace Echo
{
	class Skeleton;
	class File;
	bool ReadSkeletonFile(File& file, Skeleton& skeleton);
}

#endif
