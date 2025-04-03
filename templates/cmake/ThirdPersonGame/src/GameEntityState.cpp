#include <GameEntityState.h>
#include <echo/Animation/AnimationState.h>
#include <echo/Physics/MotionState.h>
#include <echo/Input/Input.h>

#include <boost/foreach.hpp>

namespace MyProject
{
	GameEntityState::GameEntityState(const std::string& stateName, GameEntity& gameEntity) : TaskGroup(stateName),
		mGameEntity(gameEntity),
		mActive(false)
	{
	}

	GameEntityState::~GameEntityState()
	{
	}

	void GameEntityState::AddMotionState(shared_ptr<MotionState> state)
	{
		if(std::find(mPhysicsBindings.begin(), mPhysicsBindings.end(), state) == mPhysicsBindings.end())
		{
			mPhysicsBindings.push_back(state);
		}
	}

	bool GameEntityState::AddAnimation(shared_ptr<AnimationState> animation)
	{
		if(mAnimations.find(animation->GetName())!=mAnimations.end())
		{
			return false;
		}
		AddTask(animation);
		animation->SetEnabled(mActive);
		if(mActive)
		{
			animation->Resume();
		}else
		{
			animation->Pause();
		}
		mAnimations[animation->GetName()]=animation;
		return true;
	}

	shared_ptr<AnimationState> GameEntityState::GetAnimation(const std::string& animationName)
	{
		AnimationNameMap::iterator it=mAnimations.find(animationName);
		if(it!=mAnimations.end())
		{
			return it->second;
		}
		return nullptr;
	}

	void GameEntityState::Activate()
	{
		if(mActive)
		{
			return;
		}
		mActive = true;
		Resume();
		BOOST_FOREACH(shared_ptr<MotionState>& state, mPhysicsBindings)
		{
			state->Activate();
		}
		BOOST_FOREACH(AnimationNameMapPair& animationPair, mAnimations)
		{
			animationPair.second->SetEnabled(true);	// Applies the animation to the mesh
			animationPair.second->Resume();			// Resumes the animation
		}
	}

	void GameEntityState::Deactivate()
	{
		if(!mActive)
		{
			return;
		}
		mActive = false;
		Pause();
		BOOST_FOREACH(shared_ptr<MotionState>& state, mPhysicsBindings)
		{
			state->Deactivate();
		}
		BOOST_FOREACH(AnimationNameMapPair& animationPair, mAnimations)
		{
			animationPair.second->Pause();				// Pause animation time progression
			animationPair.second->SetEnabled(false);	// Remove the animation to the mesh
		}

	}

//	void GameEntityState::Collide(const EObjectManifoldResult& contactManifold)
//	{
//		if(contactManifold.getBody0Internal()->getUserPointer() && contactManifold.getBody1Internal()->getUserPointer())
//		{
//			EMotionState* eobjectYou = reinterpret_cast<EMotionState*>(contactManifold.getBody0Internal()->getUserPointer());
//			EMotionState* eobjectThem = reinterpret_cast<EMotionState*>(contactManifold.getBody1Internal()->getUserPointer());
//			EHash::EHashToken triggerName = "Collision:";
//			triggerName += eobjectYou->GetPhysicsName().mHashString;
//			triggerName += ":";
//			triggerName += eobjectThem->GetPhysicsName().mHashString;
//			if(HasTrigger(triggerName))
//			{
//				ECollisionArgs args(contactManifold);
//				TriggerTrigger(triggerName, args);
//			}
//		}
//		//int numContacts = contactManifold->getNumContacts();   
//		//for (int j=0;j<numContacts;j++)
//		//{
//		//	btManifoldPoint& pt = contactManifold->getContactPoint(j);
//		//	btVector3 ptA = pt.getPositionWorldOnA();
//		//	btVector3 ptB = pt.getPositionWorldOnB();
//		//}
//	}

	void GameEntityState::SetInputManager(shared_ptr<InputManager> inputManager)
	{
		if(mInputManager)
		{
			RemoveTask(mInputManager);
		}
		mInputManager = inputManager;
		AddTask(mInputManager);
	}

//	void GameEntityState::SetFunctionBinder(shared_ptr<FunctionBinder> functionBinder)
//	{
//		mFunctionBinder = functionBinder;
//	}
}
