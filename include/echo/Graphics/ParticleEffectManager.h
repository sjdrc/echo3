#ifndef _PARTICLEEFFECTMANAGER_H_
#define _PARTICLEEFFECTMANAGER_H_

#include <echo/Graphics/ParticleSystems.h>

namespace Echo
{
	/**
	 * Stores multiple particle systems by name which is also used to identify particle types.
	 * A simple example setup would be if you have 10 different particle types that are
	 * essentially just different materials to be emitted in different ways then you might
	 * set up a ParticleEffectManager with 10 SimpleStandardParticleSystems, each essentially
	 * corresponding to a pool of the particles for each material (since only one Material can
	 * be used for any one ParticleSystem). Using the Emit() method you can emit a particle of
	 * a given type by name.
	 * @note At this stage I am using "Effect" rather than "Particle" with the plan to eventually
	 * add features to this class to make it a more comprehensive effect system.
	 * @note Currently the particles all need to use the same type since emitting requires a
	 * particle to be passed in. In the future this is likely to be data driven with some common
	 * parameters used when calling Emit().
	 */
	template< class ParticleType >
	class ParticleEffectManager
	{
	public:
		/**
		 * Add an effect.
		 * An effect in this context is a configured particle system that processes
		 * particles in a particular way or, if you're using SimpleStandardParticleSystems,
		 * just different materials.
		 * @param effectName The name to assign to the effect.
		 * @param particleSystem The particle system, essentially the pool for this type of
		 * effect.
		 */
		void AddEffect(const std::string& effectName, shared_ptr< ParticleEmitterInterface<ParticleType> > particleSystem)
		{
			mParticleSystems.insert(NamedParticleSystemPair(effectName,particleSystem));
		}
		
		/**
		 * Remove an effect by name.
		 * @param effectName The name of the effect.
		 */
		void RemoveEffect(const std::string& effectName)
		{
			mParticleSystems.erase(effectName);
		}

		/**
		 * Emit a particle effect.
		 * @param effectName The name of the effect you want to emit.
		 * @param particle The particle details.
		 */
		void Emit(const std::string& effectName, ParticleType particle)
		{
			typename NamedParticleSystemMap::iterator it = mParticleSystems.find(effectName);
			if(it!=mParticleSystems.end() && it->second)
			{
				it->second->Emit(particle);
			}else
			{
				ECHO_LOG_ERROR("Unable to emit effect. No effect named \"" << effectName << "\"");
			}
		}
	private:
		typedef std::pair< std::string, shared_ptr< ParticleEmitterInterface<ParticleType> > > NamedParticleSystemPair;
		typedef std::map< std::string, shared_ptr< ParticleEmitterInterface<ParticleType> > > NamedParticleSystemMap;
		NamedParticleSystemMap mParticleSystems;
	};
}
#endif 
