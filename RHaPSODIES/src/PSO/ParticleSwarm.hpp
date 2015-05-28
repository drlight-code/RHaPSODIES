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

		void InitializeAroundBest(int iKeepKBest);

    private:
		ParticleVec m_vecParticles;
		Particle m_oParticleBest;
	};
}

#endif // _RHAPSODIES_PARTICLESWARM
