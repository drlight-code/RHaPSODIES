#include <cassert>

#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>

#include <VistaTools/VistaRandomNumberGenerator.h>

#include "Particle.hpp"
#include "ParticleSwarm.hpp"

namespace rhapsodies {
	ParticleSwarm::ParticleSwarm(size_t nParticles) :
		m_vecParticles(nParticles) {
	}
	
	ParticleSwarm::~ParticleSwarm() {
	}

	ParticleSwarm::ParticleVec& ParticleSwarm::GetParticles() {
		return m_vecParticles;
	}

	Particle& ParticleSwarm::GetParticleBest() {
		return m_oParticleBest;
	}
	
	void ParticleSwarm::InitializeAroundBest(int iKeepKBest) {
		// sort by ibest score, keep best k entries, next-worst is set
		// to best particle, rest is reset.
		
		std::sort(
			m_vecParticles.begin(), m_vecParticles.end(),
			[](Particle pA, Particle pB) {
				return pA.GetIBestPenalty() > pB.GetIBestPenalty();   
			});

		auto it=m_vecParticles.begin();
		for(; it != m_vecParticles.begin()+iKeepKBest; ++it) {
			// keep state and velocity (do nothing) OR
			// reset to ibest solution and reset velocity
			*(it->GetHandModelLeft()) = *(it->GetIBestModelLeft());
			it->ResetVelocity();
		}

		*it = m_oParticleBest;
		++it;

		for(; it != m_vecParticles.end(); ++it) {
			it->InitializeAround(m_oParticleBest);
		}
	}
}
