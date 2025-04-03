#ifndef VECTORUTILS_H
#define VECTORUTILS_H
#include <echo/Maths/Quaternion.h>

namespace Echo
{
	/** Generates a new random vector which deviates from this vector by a
		given angle in a random direction.
		@remarks
			This method assumes that the random number generator has already
			been seeded appropriately.
		@param
			angle The angle at which to deviate
		@param
			up Any vector perpendicular to this one (which could generated
			by cross-product of this vector and any other non-colinear
			vector). If you choose not to provide this the function will
			derive one on it's own, however if you provide one yourself the
			function will be faster (this allows you to reuse up vectors if
			you call this method more than once)
		@returns
			A random vector which deviates from this vector by angle. This
			vector will not be normalised, Normalise it if you wish
			afterwards.
	 */
	template<typename T>
	inline Vector3Generic<T> RandomVector3Deviant(const Radian& angle, const Vector3Generic<T>& from, const Vector3Generic<T>& up = Vector3Generic<T>::ZERO) const
	{
		Vector3Generic<T> newUp;

		if(up == Vector3Generic<T>::ZERO)
		{
			// Generate an up vector
			newUp = from.Perpendicular();
		}
		else
		{
			newUp = up;
		}

		// Rotate up vector by random amount around this
		Quaternion q;
		q.FromAngleAxis(Radian(Maths::UnitRandom() * Maths::TWO_PI), from);
		newUp = q * newUp;

		// Finally rotate this by given angle around randomised up
		q.FromAngleAxis( angle, newUp );
		return q * from;
	}
}

#endif /* VECTORUTILS_H */

