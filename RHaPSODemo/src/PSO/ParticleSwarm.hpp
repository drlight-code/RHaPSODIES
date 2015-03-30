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
		
    private:
		ParticleVec m_vecParticles;
	};
}

#endif // _RHAPSODIES_PARTICLESWARM
