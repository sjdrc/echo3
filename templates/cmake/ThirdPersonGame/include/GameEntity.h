#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include <echo/Kernel/TaskGroup.h>
#include <echo/Graphics/SceneEntity.h>
#include <map>

using namespace Echo;

namespace Echo
{
	class MotionState;
	class AnimationState;
	class PhysicsBody;
	class InputManager;
}

namespace MyProject
{
	class GameEntityState;
	
	class GameEntity : public TaskGroup, public SceneEntity
	{
	public:
		GameEntity(const std::string& name);
		~GameEntity();
		shared_ptr<MotionState> GetPhysicsBinding(shared_ptr<PhysicsBody> body);
		shared_ptr<GameEntityState> CreateState(const std::string& state);
		shared_ptr<GameEntityState> CreateState(const std::string& stateName, shared_ptr<Skeleton> skeleton);
		shared_ptr<GameEntityState> CreateState(const std::string& stateName, shared_ptr<AnimationState> animationState);
		shared_ptr<GameEntityState> GetState(const std::string& state);
		void RemoveState(const std::string& stateName);
		void RemoveAllStates();
		bool SetState(const std::string& state);
		bool ActivateState(const std::string& state);
		void DeactivateState(const std::string& state);
		Size DeactivateAllStates();
		const bool& IsActive() const
		{
			return mActive;
		}
		shared_ptr<InputManager> GetInputManager() const
		{
			return mInputManager;
		}
		void SetInputManager(shared_ptr<InputManager> inputManager);

		//Interfaces
		void Activate();
		void Deactivate();
		
	private:
		shared_ptr<InputManager> mInputManager;
		bool mProcessingActive;
		std::map< shared_ptr<PhysicsBody>, shared_ptr<MotionState> > mPhysicsBindings;
		std::map< shared_ptr<PhysicsBody>, shared_ptr<MotionState> > mKinematicObjectBindings;
		bool mActive;

		void ProcessDeactivatingStates();

		typedef std::map< std::string, shared_ptr<GameEntityState> > StateNameMap;
		typedef std::list< shared_ptr<GameEntityState> > StateList;
		StateNameMap mStates;
		StateList mActiveStates;
		StateList mPreviouslyActiveStates;
		StateList mDeactivatingStates;
		std::list< shared_ptr<AnimationState> > mAnimations;
		std::list< shared_ptr<MotionState> > mPhysicsMotionsStates;
	};
}

#endif

