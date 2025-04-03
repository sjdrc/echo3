#include <echo/Platforms/Ogre/OgreSkeletonFileFormat.h>
#include <echo/Resource/3dsReader.h>
#include <echo/Animation/Skeleton.h>
#include <echo/Animation/BoneAnimationTrack.h>
#include <echo/Animation/SkeletonAnimation.h>
#include <echo/Animation/BoneKeyFrame.h>
#include <echo/FileSystem/FileSystem.h>
#include <echo/Maths/Quaternion.h>
#include <echo/Animation/Bone.h>
#include <iostream>
#include <vector>

namespace Echo
{
	struct BoneData
	{
		std::string mName;
		u16 mIndex;
		s32 mParent;
		Vector3 mPosition;
		Quaternion mOrientation;
		Vector3 mScale;
		std::vector< u16 > mChildren;
	};

	struct BoneKeyFrameData
	{
		Vector3 mPosition;
		Quaternion mOrientation;
		Vector3 mScale;
	};

	struct AnimationData
	{
		std::string mName;
		float mLength;
		std::map< int, std::map< f32, BoneKeyFrameData > > mBoneKeyFrames;
	};

	struct SkeletonData
	{
		std::vector< AnimationData > mAnimations;
		std::map< u16, BoneData > mBones;
	};

	void _ReadSkeletonAnimation(File& file, SkeletonData& skeleton,Chunk3DS** inOutChunk)
	{
		AnimationData animation;

		u32 bytesSkipped=0;
		char tempChar=1;
		while(tempChar!=0x0A && tempChar!=0x0)
		{
			file.Read(&tempChar,1,1);
			if(tempChar!=0x0A && tempChar!=0x0)
				animation.mName+=tempChar;
			++bytesSkipped;
		}
		bytesSkipped+=file.Read(&animation.mLength,4,1);
		ECHO_LOG_DEBUG("Found animation: " << animation.mName);

		*inOutChunk=(*inOutChunk)->NextSubChunk(bytesSkipped);

		while((*inOutChunk)->GetID()==Ogre::SKELETON_ANIMATION_TRACK)
		{
			u16 bone=0;
			// unsigned short boneIndex	 : Index of bone to apply to
			file.Read(&bone,2,1);
			*inOutChunk=(*inOutChunk)->NextSubChunk(2);
			while((*inOutChunk)->GetID() == Ogre::SKELETON_ANIMATION_TRACK_KEYFRAME)
			{
				// float time					: The time position (seconds)
				// Quaternion rotate			: Rotation to apply at this keyframe
				// Vector3 translate			: Translation to apply at this keyframe
				// Vector3 scale				: Scale to apply at this keyframe
				BoneKeyFrameData keyFrame;
				keyFrame.mScale=Vector3(1,1,1);
				u32 currentChunkSize=0;
				f32 keyFrameTime;
				currentChunkSize+=file.Read(&keyFrameTime,4,1);
				f32 v=0;
				currentChunkSize+=file.Read(&v,4,1);
				keyFrame.mOrientation.SetX(v);
				currentChunkSize+=file.Read(&v,4,1);
				keyFrame.mOrientation.SetY(v);
				currentChunkSize+=file.Read(&v,4,1);
				keyFrame.mOrientation.SetZ(v);
				currentChunkSize+=file.Read(&v,4,1);
				keyFrame.mOrientation.SetW(v);

				currentChunkSize+=file.Read(&keyFrame.mPosition.x,4,1);
				currentChunkSize+=file.Read(&keyFrame.mPosition.y,4,1);
				currentChunkSize+=file.Read(&keyFrame.mPosition.z,4,1);

				if((*inOutChunk)->GetNext() > currentChunkSize + 6)
				{
					file.Read(&keyFrame.mScale,4,3);
				}
				animation.mBoneKeyFrames[bone][keyFrameTime]=keyFrame;
				*inOutChunk=(*inOutChunk)->NextChunk();
			}
		}
		skeleton.mAnimations.push_back(animation);
	}

	//Lets get into it
	bool ReadSkeletonFile(File& file, Skeleton& targetSkeleton)
	{
		//--------------------
		//				START EXTRACTION CODE
		// This code is all the work done for you - unless you want something more
		// from the file. The guts of this program loops and finds the relating chunks
		// to vertices, faces and mapping data and extracts it into three lists (just below)
		//
		// You can scroll all the way down until you see a nice note that its the
		// end of the extraction code, where after you can use the lists and save your own
		// format.
		//--------------------

		//3DS Files are stored in Little endian modes
	#ifdef ECHO_BIG_ENDIAN
		file.SetEndianConversionEnabled(true);
	#endif

		//Create the first Chunk (will be the main file chunk)
		//and check for error
		Chunk3DS* currentChunk = new Chunk3DS(file);
		currentChunk->ProcessFirstChunk();
		Chunk3DS* currentChunkTemp=0;

		//Make sure the first chunk is the Ogre::M_HEADER chunk
		if(!(currentChunk->GetID()==Ogre::SKELETON_HEADER))
		{//If not then report an error
			ECHO_LOG_ERROR("File is not a valid .skeleton file.");
			return false;
		}

		SkeletonData skeleton;

		//Display Chunks
		while(currentChunk)
		{
			//I only want to see 3D Edit chunk, main chunk and Objects
			switch(currentChunk->GetID())
			{
				case Ogre::SKELETON_HEADER:
				{
					//This is a special chunk. It does not have a length (beats me why)
					char tempChar=1;
					u32 chunkSize=0;
					std::string name;
					file.Seek(2);
					while(tempChar!=0x0A && tempChar!=0x0)
					{
						file.Read(&tempChar,1,1);
						if(tempChar!=0x0A && tempChar!=0x0)
							name+=tempChar;
						++chunkSize;
					}
					ECHO_LOG_DEBUG(".skeleton version:" << name);
					currentChunk->SetNext(chunkSize+2);
					currentChunk=currentChunk->NextChunk();
				}break;
				case Ogre::SKELETON_BONE:
					{
						// char* name					   : name of the bone
						// unsigned short handle			: handle of the bone, should be contiguous & start at 0
						// Vector3 position				 : position of this bone relative to parent 
						// Quaternion orientation		   : orientation of this bone relative to parent 
						// Vector3 scale					: scale of this bone relative to parent 
						u32 currentChunkSize=0;
						BoneData bone;
						bone.mIndex=0;
						bone.mParent=-1;
						bone.mPosition=Vector3::ZERO;
						bone.mOrientation=Quaternion::IDENTITY;
						bone.mScale=Vector3(1.0f,1.0f,1.0f);

						char tempChar=1;
						while(tempChar!=0x0A && tempChar!=0x0)
						{
							file.Read(&tempChar,1,1);
							if(tempChar!=0x0A && tempChar!=0x0)
								bone.mName+=tempChar;
							++currentChunkSize;
						}

						//There is a bug in Ogre's skeleton serialiser. It does not support writing bone names correctly
						currentChunk->SetNext(currentChunk->GetNext() + currentChunkSize);

						currentChunkSize+=file.Read(&bone.mIndex,2,1);
						currentChunkSize+=file.Read(&bone.mPosition.x,4,1);
						currentChunkSize+=file.Read(&bone.mPosition.y,4,1);
						currentChunkSize+=file.Read(&bone.mPosition.z,4,1);
						f32 v=0.0f;
						currentChunkSize+=file.Read(&v,4,1);
						bone.mOrientation.SetX(v);
						currentChunkSize+=file.Read(&v,4,1);
						bone.mOrientation.SetY(v);
						currentChunkSize+=file.Read(&v,4,1);
						bone.mOrientation.SetZ(v);
						currentChunkSize+=file.Read(&v,4,1);
						bone.mOrientation.SetW(v);

						if(currentChunk->GetNext() > currentChunkSize + 6)
						{
							file.Read(&bone.mScale,4,3);
						}
						ECHO_LOG_DEBUG("Bone found: " << bone.mName);

						if(skeleton.mBones.find(bone.mIndex)!=skeleton.mBones.end())
						{
							ECHO_LOG_WARNING("Duplicate handle found in skeleton file. Skeleton may be incorrect");
						}
						skeleton.mBones[bone.mIndex]=bone;
					}

					currentChunk=currentChunk->NextChunk();	//Get the Next Chunk
				break;
				case Ogre::SKELETON_BONE_PARENT:
					{
						// unsigned short handle			 : child bone
						// unsigned short parentHandle   : parent bone
						u16 bone=0;
						u16 boneParent=0;
						file.Read(&bone,2,1);
						file.Read(&boneParent,2,1);
						std::map< u16, BoneData >::iterator bit=skeleton.mBones.find(bone);
						std::map< u16, BoneData >::iterator pit=skeleton.mBones.find(boneParent);
						if(bit!=skeleton.mBones.end() && pit!=skeleton.mBones.end())
						{
							bit->second.mParent=boneParent;
							pit->second.mChildren.push_back(bone);
						}else
						{
							ECHO_LOG_DEBUG("Bone handle(" << bone << ") or Parent handle(" << boneParent << ") invalid.");
						}
						currentChunk=currentChunk->NextChunk();	//Get the Next Chunk
					}
				break;
				case Ogre::SKELETON_ANIMATION:
					_ReadSkeletonAnimation(file,skeleton,&currentChunk);
				break;
				default:
					currentChunkTemp=currentChunk;					//This is for cleanup later - The escape
																	//out of this loop occurs when the file
																	//can't be read anymore and currentChunk
																	//is NULL, so we need to preserve currentChunk
																	//for when it's NextChunk is NULL.
					currentChunk=currentChunk->NextChunk();	//Get the Next Chunk
				break;
			}
		}
		ECHO_LOG_DEBUG("End skeleton file");
	//--------------------
	//				END EXTRACTION CODE
	//--------------------

		//Finish up and clean up the chunks
		if(currentChunkTemp)
		{
			//Clean up all our chunks in memory	   \del/
			currentChunk=Walk(Ogre::SKELETON_HEADER,CHUNK_WALK_BACK,true,currentChunkTemp);
			delete currentChunk;
		}
		file.Close();

		std::map< u16, BoneData >::iterator it=skeleton.mBones.begin();
		std::map< u16, BoneData >::iterator itEnd=skeleton.mBones.end();

		//Create all the bones
		while(it!=itEnd)
		{
			shared_ptr<Bone> bone=targetSkeleton.CreateBone(it->second.mName);
			bone->SetPosition(it->second.mPosition);
			bone->SetOrientation(it->second.mOrientation);
			bone->SetScale(it->second.mScale);
			//bone->Rotate(it->second.mOrientation);
			//Quaternion q=;
			//bone->SetOrientation(q);
			//bone->SetInitialScale(it->second.mScale);
			++it;
		}

		it=skeleton.mBones.begin();
		itEnd=skeleton.mBones.end();
		//Build the tree by setting all the parents
		while(it!=itEnd)
		{
			shared_ptr<Bone> bone=targetSkeleton.GetBone(it->second.mName);
			if(it->second.mParent!=-1)
			{
				std::map< u16, BoneData >::iterator pit=skeleton.mBones.find((u16)it->second.mParent);
				if(pit!=skeleton.mBones.end())
				{
					shared_ptr<Bone> parent=targetSkeleton.GetBone(pit->second.mName);
					if(bone!=parent)
					{
						bone->SetParent(parent.get());
					}
				}
			}
			++it;
		}

		for(u32 a=0; a < skeleton.mAnimations.size(); ++a)
		{
			shared_ptr<SkeletonAnimation> animation=targetSkeleton.CreateAnimation(skeleton.mAnimations[a].mName);

			std::map< int, std::map< f32, BoneKeyFrameData > >::iterator bkfIt=skeleton.mAnimations[a].mBoneKeyFrames.begin();
			std::map< int, std::map< f32, BoneKeyFrameData > >::iterator bkfItEnd=skeleton.mAnimations[a].mBoneKeyFrames.end();
			while(bkfIt!=bkfItEnd)
			{
				shared_ptr<Bone> bone=targetSkeleton.GetBone(bkfIt->first);
				if(!bone)
				{
					ECHO_LOG_WARNING("Bone not found for index " << bkfIt->first << ". This could mean the skeleton data is incorrect.");
				}else
				{
					shared_ptr<BoneAnimationTrack> animationTrack=animation->GetAnimationTrack(bone);
					if(animationTrack)
					{
						std::map< f32, BoneKeyFrameData >::iterator kfIt=bkfIt->second.begin();
						std::map< f32, BoneKeyFrameData >::iterator kfItEnd=bkfIt->second.end();
						if (kfIt==kfItEnd)
						{
							BoneKeyFrame* keyframe=static_cast<BoneKeyFrame*>(animationTrack->CreateKeyFrame(Seconds(kfIt->first)).get());
							keyframe->mOrientation=bone->GetOrientation();
							keyframe->mScale=bone->GetScale();
							keyframe->mPosition=bone->GetPosition();
						}else
						{
							while (kfIt!=kfItEnd)
							{
								BoneKeyFrame* keyframe=static_cast<BoneKeyFrame*>(animationTrack->CreateKeyFrame(Seconds(kfIt->first)).get());
								keyframe->mOrientation=kfIt->second.mOrientation;
								keyframe->mScale=kfIt->second.mScale;
								keyframe->mPosition=kfIt->second.mPosition;
								++kfIt;
							}
						}
					}else
					{

						ECHO_LOG_WARNING("No animation track found for bone.");
					}
				}
				++bkfIt;
			}
		}
		targetSkeleton.SetInitialState();
		targetSkeleton.SetBindingPose();

		ECHO_LOG_DEBUG("Skeleton loading complete");
		return true;
	}
}
