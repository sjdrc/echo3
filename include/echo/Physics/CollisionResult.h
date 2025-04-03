#ifndef ECHOCOLLISIONRESULT_H
#define ECHOCOLLISIONRESULT_H
#include <echo/Types.h>
#include <echo/Maths/Vector3.h>

namespace Echo
{
	class CollisionResult
	{
	public:
		CollisionResult(){}
		virtual ~CollisionResult(){}
		virtual Size GetNumberOfContactPoints() const = 0;
		virtual Vector3 GetLastContactPoint() const = 0;
	};

	class SimpleCollisionResult : public CollisionResult
	{
	public:
		SimpleCollisionResult(){}
		virtual ~SimpleCollisionResult(){}
		virtual Size GetNumberOfContactPoints() const override {return mNumberOfContactPoints;}
		virtual Vector3 GetLastContactPoint() const override { return mLastContactPoint;}

		void AddContactPoint(const Vector3& pointInObject)
		{
			mNumberOfContactPoints++;
			mLastContactPoint = pointInObject;
		}
	private:
		Size mNumberOfContactPoints;
		Vector3 mLastContactPoint;
		
	};
}
#endif
