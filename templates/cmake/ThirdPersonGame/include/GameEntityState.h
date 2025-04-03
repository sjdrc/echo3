#ifndef _GAMEENTITYSTATE_H_
#define _GAMEENTITYSTATE_H_

#include <echo/Kernel/TaskGroup.h>
#include <list>
#include <map>

using namespace Echo;

namespace Echo
{
	class MotionState;
	class InputManager;
	class AnimationState;
}

namespace MyProject
{
	class GameEntity;

	class GameEntityState : public TaskGroup
	{
	public:
		GameEntityState(const std::string& name, GameEntity& gameEntity);
		virtual ~GameEntityState();

		void AddMotionState(shared_ptr<MotionState> state);
		
		bool AddAnimation(shared_ptr<AnimationState> animation);
		shared_ptr<AnimationState> GetAnimation(const std::string& animationName);

		shared_ptr<InputManager> GetInputManager() const
		{
			return mInputManager;
		}
		void SetInputManager(shared_ptr<InputManager> inputManager);

		const bool& IsActive() const
		{
			return mActive;
		}
		virtual void Activate();
		virtual void Deactivate();
	private:
		bool mActive;
		GameEntity& mGameEntity;
		typedef std::map< std::string, shared_ptr<AnimationState> > AnimationNameMap;
		typedef std::pair< const std::string, shared_ptr<AnimationState> > AnimationNameMapPair;
		typedef std::list< shared_ptr<MotionState> > MotionStateList;
		shared_ptr<InputManager> mInputManager;
		std::list< shared_ptr<MotionState> > mPhysicsBindings;
		AnimationNameMap mAnimations;
	};
}
#endif 
