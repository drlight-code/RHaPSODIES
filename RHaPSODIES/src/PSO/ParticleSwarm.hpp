#ifndef _RHAPSODIES_PARTICLESWARM
#define _RHAPSODIES_PARTICLESWARM

#include <vector>

namespace rhapsodies {

	class Particle;
	
	/**
	 * updating and score propagation
	 * different neighborhood topologies
	 * constriction coefficients choice
	 * 
	 */
	class ParticleSwarm {
    public:
		typedef std::vector<Particle> ParticleVec;
		
		ParticleSwarm(size_t nParticles);
		~ParticleSwarm();

		ParticleVec& GetParticles();

		// Initialize the swarm with random perturbations of particles
		// around the "cluster-center" passed as argument.
		void InitializeAround(Particle &oCenter);

		void Evolve();

		Particle GetBestMatch();
		
    private:
		ParticleVec m_vecParticles;
	};
}

#endif // _RHAPSODIES_PARTICLESWARM
