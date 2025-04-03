#include <echo/Physics/PhysicsBody.h>
#include <boost/foreach.hpp>

namespace Echo
{
	PhysicsBody::PhysicsBody(shared_ptr<PhysicsWorld> world) : mWorld(world)
	{
	}
	
	PhysicsBody::~PhysicsBody()
	{
	}

	void PhysicsBody::OnCollide(PhysicsBody& other, const CollisionResult& collisionResult)
	{
		ObjectCollideCallbacksMap::iterator it = mPerObjectCollideCallbacks.find(other.GetName());
		if(it!=mPerObjectCollideCallbacks.end())
		{
			BOOST_FOREACH(NamedCallbacksPair& cp, it->second)
			{
				BOOST_FOREACH(CollideCallback& callback, cp.second)
				{
					callback(*this, other,collisionResult);
				}
			}
		}
		BOOST_FOREACH(NamedCallbacksPair& cp, mGeneralCollideCallbacks)
		{
			BOOST_FOREACH(CollideCallback& callback, cp.second)
			{
				callback(*this, other,collisionResult);
			}
		}
	}
	
	void PhysicsBody::RegisterGeneralCollideCallback(const std::string& callbackID, CollideCallback callback)
	{
		mGeneralCollideCallbacks[callbackID].push_back(callback);
	}
	
	void PhysicsBody::RegisterObjectCollideCallback(const std::string& id, const std::string& objectName, CollideCallback callback)
	{
		mPerObjectCollideCallbacks[objectName][id].push_back(callback);
	}
	
	void PhysicsBody::DeregisterObjectCollideCallbacks(const std::string& id, const std::string& objectName)
	{
		ObjectCollideCallbacksMap::iterator it = mPerObjectCollideCallbacks.find(objectName);
		if(it!=mPerObjectCollideCallbacks.end())
		{
			it->second.erase(id);
		}
	}
	
	void PhysicsBody::DeregisterGeneralCollideCallbacks(const std::string& id)
	{
		mGeneralCollideCallbacks.erase(id);
	}
	
	void PhysicsBody::DeregisterAllCollideCallbacks(const std::string& id)
	{
		typedef std::pair< const std::string, NamedCallbackLists > NamedMapPair;
		BOOST_FOREACH(NamedMapPair& ncl, mPerObjectCollideCallbacks)
		{
			ncl.second.erase(id);
		}
		mGeneralCollideCallbacks.erase(id);
	}
	
	void PhysicsBody::DeregisterAllCollideCallbacks()
	{
		mGeneralCollideCallbacks.clear();
		mPerObjectCollideCallbacks.clear();
	}
}
