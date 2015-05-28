#ifndef _RHAPSODIES_PARTICLESWARM
#define _RHAPSODIES_PARTICLESWARM

#include <vector>

namespace rhapsodies {
	class Particle;
	class ParticleSwarm {
    public:
		typedef std::vector<Particle> ParticleVec;
		
		ParticleSwarm(size_t nParticles);
		~ParticleSwarm();

		ParticleVec& GetParticles();
		Particle& GetParticleBest();

		// Initialize the swarm with random perturbations of particles
		// around the "cluster-center" passed as argument.
		void InitializeAround(Particle &oCenter);

    private:
		ParticleVec m_vecParticles;
		Particle m_oParticleBest;
	};
}

#endif // _RHAPSODIES_PARTICLESWARM
