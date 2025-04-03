#ifndef _ECHOSKELETON_H_
#define _ECHOSKELETON_H_

#include <echo/Types.h>
#include <echo/Animation/Bone.h>
#include <echo/Maths/Vector3.h>
#include <echo/Maths/Quaternion.h>
#include <echo/Kernel/TaskGroup.h>
#include <echo/Resource/Resource.h>
#include <set>
#include <map>

namespace Echo
{
	class SkeletonAnimation;
	class SkeletonAnimationState;

	class Skeleton : public TaskGroup, public Resource<Skeleton>
	{
	public:
		typedef std::map< Size, shared_ptr<Bone> > BoneMap;
		typedef std::pair<const Size, shared_ptr<Bone> > IndexBonePair;
	private:
		BoneMap mBones;
		std::map< std::string, shared_ptr<Bone> > mBonesByName;
		std::map< std::string, shared_ptr<SkeletonAnimation> > mAnimations;
		typedef std::pair< const std::string, shared_ptr<SkeletonAnimation> > NameAnimationPair;
		std::list< shared_ptr<SkeletonAnimationState> > mAnimationStates;
		shared_ptr<Bone> mRootBone;
		std::string mName;
		bool mTransformsOutOfDate;
		virtual bool _Unload() override;
		virtual Size OnRequestMemoryRelease() override;
	public:
		Skeleton();
		~Skeleton();
		
		/**
		 * Clones the skeleton and animations.
		 * @note Does not clone animation states.
		 * @return 
		 */
		shared_ptr<Skeleton> Clone() const;

		const std::string& GetName() const { return mName; }
		void SetName(const std::string& val) { mName = val; }

		shared_ptr<Bone> CreateBone(const std::string& name);
		shared_ptr<Bone> CreateBone(const std::string& name, const std::string& parent);
		shared_ptr<Bone> CreateBone(const std::string& name, const Vector3& position, const Quaternion& rotation=Quaternion::IDENTITY);
		shared_ptr<Bone> CreateBone(const std::string& name, const std::string& parent, const Vector3& position, const Quaternion& rotation=Quaternion::IDENTITY);
		shared_ptr<Bone> GetBone( const std::string& name ) const;
		shared_ptr<Bone> GetBone( Size index ) const;
		BoneMap& GetBoneMap() {return mBones;}
		void Reset();

		shared_ptr<SkeletonAnimation> CreateAnimation(const std::string& name);
		shared_ptr<SkeletonAnimation> GetAnimation(const std::string& name);

		/**
		 * Create an animation state.
		 * An animation state is basically a time tracker for an animation, you can have multiple
		 * animation states for the same animation and each will track time differently.
		 * After creating an AnimationState (Task) the state will be added to the Skeleton (TaskGroup).
		 * If the skeleton is added to a TaskManager when it updates all animation states will be
		 * updated too which will progress their animation time. This is a simple way to perform
		 * Skeleton animation but you can also opt not to use the Skeleton as a Task and just update
		 * the AnimationStates manually or in another task manager. This might be more appropriate in
		 * situations where you want different animations to play at different times.
		 * @param animationName The name of the animation to create an AnimationState for.
		 * @param manageUpdates If true tells the Skeleton to add the AnimationState to itself (it is
		 * a TaskGroup) for updates. If you have more than one instance of the animation state you
		 * will probably want to set this to false since you'll want to apply the animation at
		 * different times for different meshes.
		 * @return 
		 */
		shared_ptr<SkeletonAnimationState> CreateAnimationState(const std::string& animationName, bool manageUpdates = true);

		Size GetNumberOfAnimations();

		/**
		 * Update the bone transforms for the combined animations.
		 * This method resets the skeleton pose then applies each active SkeletonAnimationState with the
		 * configured weight (a setting on the animation state). The bone transforms are then updated
		 * ready for the final pose.
		 */
		void UpdateTransforms();
		
		/**
		 * Sets the current pose as the initial pose, this is the pose used when the pose is reset.
		 * Once you have built your skeleton you must call this method before applying animations, if you
		 * do not then the inverse transforms are not calculated and bone animations are not applied properly.
		 * This method calls SetBindingPose().
		 */
		void SetInitialState();
		
		/**
		 * Sets the binding pose from which any bone animations will be relative to.
		 */
		void SetBindingPose();
		
		void MarkTransformsOutOfDate();
		
		bool GetTransformsOutOfDate() const {return mTransformsOutOfDate;}
	};
}

#endif 
