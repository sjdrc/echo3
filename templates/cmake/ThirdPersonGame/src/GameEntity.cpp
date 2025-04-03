#include <GameEntity.h>
#include <GameEntityState.h>
#include <echo/Animation/Skeleton.h>
#include <echo/Animation/SkeletonAnimationState.h>
#include <echo/Physics/MotionState.h>
#include <echo/Physics/PhysicsBody.h>
#include <echo/Input/Input.h>

#include <boost/foreach.hpp>

namespace MyProject
{
	GameEntity::GameEntity(const std::string& name) : TaskGroup(name),
		mActive(false),
		mProcessingActive(false)
	{
	}

	GameEntity::~GameEntity()
	{
		DeactivateAllStates();
	}

	shared_ptr<GameEntityState> GameEntity::CreateState(const std::string& stateName)
	{
		if(mStates.find(stateName) != mStates.end())
		{
			return nullptr;
		}
		shared_ptr<GameEntityState> newState(new GameEntityState(stateName, *this));
		mStates[stateName] = newState;
		AddTask(newState);
		newState->Pause();
		return newState;
	}

	shared_ptr<GameEntityState> GameEntity::CreateState(const std::string& stateName, shared_ptr<Skeleton> skeleton)
	{
		if(!skeleton)
		{
			std::cout << "Error: GameEntity::CreateState() - will not create state with null skeleton. Use overload instead." << std::endl;
			return nullptr;
		}
		return CreateState(stateName,skeleton->CreateAnimationState(stateName,false));
	}
		
	shared_ptr<GameEntityState> GameEntity::CreateState(const std::string& stateName, shared_ptr<AnimationState> animationState)
	{
		if(!animationState)
		{
			std::cout << "Error: GameEntity::CreateState() - will not create state with null animationState. Use overload instead." << std::endl;
			return nullptr;
		}
		shared_ptr<GameEntityState> newState = CreateState(stateName);
		if(newState)
		{
			newState->AddAnimation(animationState);
		}
		return newState;
	}
	
	shared_ptr<GameEntityState> GameEntity::GetState(const std::string& state)
	{
		StateNameMap::iterator it = mStates.find(state);
		if(it == mStates.end())
		{
			return 0;
		}
		return it->second;
	}

	void GameEntity::RemoveState(const std::string& state)
	{
		StateNameMap::iterator it = mStates.find(state);
		if(it != mStates.end())
		{
			if(it->second->IsActive())
			{
				it->second->Deactivate();
				mActiveStates.remove(it->second);
			}
			mStates.erase(it);
		}
	}

	void GameEntity::RemoveAllStates()
	{
		while(!mStates.empty())
		{
			StateNameMap::iterator it = mStates.begin();
			if(it->second->IsActive())
			{
				it->second->Deactivate();
				mActiveStates.remove(it->second);
			}
			mStates.erase(it);
		}
		mActiveStates.clear();
	}

	bool GameEntity::SetState(const std::string& stateName)
	{
		StateNameMap::iterator it = mStates.find(stateName);
		if(it == mStates.end())
		{
			return false;
		}
		BOOST_FOREACH(shared_ptr<GameEntityState>& state, mActiveStates)
		{
			state->Deactivate();
		}
		mActiveStates.clear();
		it->second->Activate();
		mActiveStates.push_back(it->second);
		return true;
	}
	
	bool GameEntity::ActivateState(const std::string& stateName)
	{
		StateNameMap::iterator it = mStates.find(stateName);
		if(it != mStates.end())
		{
			if(it->second)
			{
				it->second->Activate();
				mActiveStates.push_back(it->second);
				return true;
			}
		}
		return false;
	}

	void GameEntity::DeactivateState(const std::string& stateName)
	{
		StateNameMap::iterator it = mStates.find(stateName);
		if(it != mStates.end())
		{
			if(it->second->IsActive())
			{
				if(mProcessingActive)
				{
					mDeactivatingStates.push_back(it->second);
				}else
				{
					it->second->Deactivate();
					mActiveStates.remove(it->second);
				}
			}
		}
	}

	Size GameEntity::DeactivateAllStates()
	{
		StateList::iterator it = mActiveStates.begin();
		StateList::iterator itEnd = mActiveStates.end();
		while(it != itEnd)
		{
			(*it)->Deactivate();
			it++;
		}
		Size statesDeactivated = mActiveStates.size();
		mActiveStates.clear();
		return statesDeactivated;
	}

	void GameEntity::ProcessDeactivatingStates()
	{
		std::list< shared_ptr<GameEntityState> >::iterator it = mDeactivatingStates.begin();
		std::list< shared_ptr<GameEntityState> >::iterator itEnd = mDeactivatingStates.end();
		while(it != itEnd)
		{
			(*it)->Deactivate();
			mActiveStates.remove(*it);
			it++;
		}
		mDeactivatingStates.clear();
	}

//	void GameEntity::Collide(const GameEntityManifoldResult& contactManifold)
//	{
//		std::list<GameEntityState*>::iterator it = mActiveStates.begin();
//		;
//		std::list<GameEntityState*>::iterator itEnd = mActiveStates.end();
//		mProcessingActive = true;
//		while(it != itEnd)
//		{
//			GameEntityState* state = (*it);
//			state->Collide(contactManifold);
//			it++;
//		}
//		mProcessingActive = false;
//		ProcessDeactivatingStates();
//	}

	void GameEntity::Activate()
	{
		if(mActive)
		{
			return;
		}
		mActive = true;
		BOOST_FOREACH(shared_ptr<GameEntityState>& state,mPreviouslyActiveStates)
		{
			state->Activate();
			mActiveStates.push_back(state);
		}
		mPreviouslyActiveStates.clear();
	}

	void GameEntity::Deactivate()
	{
		if(!mActive)
		{
			return;
		}
		mActive = false;
		mPreviouslyActiveStates = mActiveStates;
		DeactivateAllStates();
	}

	//void GameEntity::AnimationEnded(const std::string& animationName) { }

	void GameEntity::SetInputManager(shared_ptr<InputManager> inputManager)
	{
		if(mInputManager)
		{
			RemoveTask(mInputManager);
		}
		mInputManager = inputManager;
		AddTask(mInputManager);
	}

}
