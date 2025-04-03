#include <echo/Animation/Bone.h>
#include <echo/Animation/Skeleton.h>
#include <echo/Animation/SkeletonAnimation.h>
#include <echo/Animation/SkeletonAnimationState.h>
#include <boost/foreach.hpp>
#include <iostream>

namespace Echo
{
	Skeleton::Skeleton() : Resource<Skeleton>(true), mRootBone(), mTransformsOutOfDate(false)
	{
	}

	Skeleton::~Skeleton()
	{
	}
	
	shared_ptr<Skeleton> Skeleton::Clone() const
	{
		shared_ptr<Skeleton> clone(new Skeleton());
		
		//Copy all of the bones
		Size newBoneIndex = 0;
		BOOST_FOREACH(const IndexBonePair& bonePair, mBones)
		{
			shared_ptr<Bone> existingBone = bonePair.second;
			shared_ptr<Bone> newBone = clone->CreateBone(existingBone->GetName());
			newBone->SetInitialPosition(existingBone->GetInitialPosition());
			newBone->SetInitialOrientation(existingBone->GetInitialOrientation());
			newBone->SetInitialScale(existingBone->GetInitialScale());
			newBone->SetPosition(existingBone->GetPosition());
			newBone->SetOrientation(existingBone->GetOrientation());
			newBone->SetScale(existingBone->GetScale());
			newBoneIndex++;
		}
		
		//Set the root bone
		if(mRootBone)
		{
			clone->mRootBone = clone->GetBone(mRootBone->GetName());
		}
		
		// Create the hierarchy. To do this we look at each bone in the skeleton and look get the parent bone name.
		BOOST_FOREACH(const IndexBonePair& bonePair, mBones)
		{
			Bone* parent = dynamic_cast<Bone*>(bonePair.second->GetParent());
			if(parent)
			{
				shared_ptr<Bone> cloneBone = clone->GetBone(bonePair.second->GetName());
				shared_ptr<Bone> cloneParent = clone->GetBone(parent->GetName());
				if(cloneBone && cloneParent)
				{
					cloneBone->SetParent(cloneParent.get());
				}
			}
		}
		
		//Clone animations
		BOOST_FOREACH(const NameAnimationPair& namedAnimation, mAnimations)
		{
			clone->mAnimations[namedAnimation.first] = namedAnimation.second->Clone(*clone);
		}
		clone->SetBindingPose();
		return clone;
	}

	shared_ptr<Bone> Skeleton::CreateBone( const std::string& name )
	{
		if( GetBone(name) )
		{
			ECHO_LOG_ERROR("" << GetName() << " SkeletonCreateBone(): bone with name " << name << " already exists.");
			return shared_ptr<Bone>();
		}

		shared_ptr<Bone> bone(new Bone(*this, mBones.size()));
		bone->SetName(name);

		if(!mRootBone)
		{
			mRootBone=bone;
		}else
		{
			mRootBone->AddChild(bone);
		}
		mBonesByName[name]=bone;
		mBones[bone->GetIndex()]=bone;

		//For each animation create a track for this bone
		BOOST_FOREACH(NameAnimationPair& animationPair, mAnimations)
		{
			animationPair.second->CreateAnimationTrack(bone);
		}

		return bone;
	}

	shared_ptr<Bone> Skeleton::CreateBone( const std::string& name, const std::string& parent )
	{
		if( GetBone(name) )
		{
			ECHO_LOG_ERROR("" << GetName() << " SkeletonCreateBone(): bone with name \"" << name << "\" already exists.");
			return shared_ptr<Bone>();
		}

		shared_ptr<Bone> parentBone=GetBone(parent);
		if(!parentBone)
		{
			ECHO_LOG_ERROR("" << GetName() << " SkeletonCreateBone(): could not find parent bone \"" << name << "\".");
			return shared_ptr<Bone>();
		}

		shared_ptr<Bone> bone(new Bone(*this, mBones.size()));
		bone->SetName(name);
		parentBone->AddChild(bone);
		mBones[bone->GetIndex()]=bone;
		mBonesByName[name]=bone;

		//For each animation create a track for this bone
		BOOST_FOREACH(NameAnimationPair& animationPair, mAnimations)
		{
			animationPair.second->CreateAnimationTrack(bone);
		}

		return bone;
	}

	shared_ptr<Bone> Skeleton::CreateBone( const std::string& name, const Vector3& position, const Quaternion& rotation)
	{
		shared_ptr<Bone> bone=CreateBone(name);
		if(bone)
		{
			bone->SetInitialPosition(position);
			bone->SetInitialOrientation(rotation);
		}
		return bone;
	}

	shared_ptr<Bone> Skeleton::CreateBone( const std::string& name, const std::string& parent, const Vector3& position/*=Vector3::ZERO*/, const Quaternion& rotation)
	{
		shared_ptr<Bone> bone=CreateBone(name,parent);
		if(bone)
		{
			bone->SetInitialPosition(position);
			bone->SetInitialOrientation(rotation);
		}
		return bone;
	}

	shared_ptr<Bone> Skeleton::GetBone( const std::string& name ) const
	{
		std::map< std::string, shared_ptr<Bone> >::const_iterator it=mBonesByName.find(name);
		if (it!=mBonesByName.end())
		{
			return it->second;
		}
		return shared_ptr<Bone>();
	}

	shared_ptr<Bone> Skeleton::GetBone( Size index ) const
	{
		BoneMap::const_iterator it=mBones.find(index);
		if (it!=mBones.end())
		{
			return it->second;
		}
		return shared_ptr<Bone>();
	}

	void Skeleton::Reset()
	{
		BoneMap::iterator it=mBones.begin();
		BoneMap::iterator itEnd=mBones.end();
		while(it!=itEnd)
		{
			it->second->Reset();
			++it;
		}
	}

	shared_ptr<SkeletonAnimation> Skeleton::CreateAnimation(const std::string& name)
	{
		if(GetAnimation(name))
			return shared_ptr<SkeletonAnimation>();
		shared_ptr<SkeletonAnimation> animation(new SkeletonAnimation(name,*this));
		mAnimations[name]=animation;
		//For each bone create an animation track
		BOOST_FOREACH(const IndexBonePair& bonePair, mBones)
		{
			animation->CreateAnimationTrack(bonePair.second);
		}
		return animation;
	}

	shared_ptr<SkeletonAnimation> Skeleton::GetAnimation(const std::string& name)
	{
		std::map<std::string, shared_ptr<SkeletonAnimation> >::iterator it=mAnimations.find(name);
		if(it!=mAnimations.end())
		{
			return it->second;
		}
		return shared_ptr<SkeletonAnimation>();
	}

	shared_ptr<SkeletonAnimationState> Skeleton::CreateAnimationState(const std::string& animationName, bool manageUpdates)
	{
		shared_ptr<SkeletonAnimation> animation=GetAnimation(animationName);
		if(animation)
		{
			shared_ptr<SkeletonAnimationState> animationState(new SkeletonAnimationState(animationName,animation,*this));
			if(manageUpdates)
			{
				AddTask(animationState);
			}
			mAnimationStates.push_back(animationState);
			return animationState;
		}
		return shared_ptr<SkeletonAnimationState>();
	}
	
	void Skeleton::UpdateTransforms()
	{
		if(!mTransformsOutOfDate)
		{
			return;
		}
		Reset();
		BOOST_FOREACH(shared_ptr<SkeletonAnimationState>& animationState, mAnimationStates)
		{
			animationState->Apply();
		}
		BOOST_FOREACH(const IndexBonePair& bonePair, mBones)
		{
			bonePair.second->_Update(true,false);
		}
		mTransformsOutOfDate = false;
	}

	void Skeleton::SetInitialState()
	{
		BOOST_FOREACH(const IndexBonePair& bonePair, mBones)
		{
			bonePair.second->_Update(true,false);
			bonePair.second->SetInitialState();
		}
		SetBindingPose();
	}

	void Skeleton::SetBindingPose()
	{
		BOOST_FOREACH(const IndexBonePair& bonePair, mBones)
		{
			bonePair.second->SetBindingPose();
		}
	}

	Size Skeleton::GetNumberOfAnimations()
	{
		return mAnimations.size();
	}
	
	void Skeleton::MarkTransformsOutOfDate()
	{
		mTransformsOutOfDate = true;
	}

	bool Skeleton::_Unload()
	{
		return false;
	}

	Size Skeleton::OnRequestMemoryRelease()
	{
		return 0;
	}

}
