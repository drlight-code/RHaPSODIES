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
		*m_vecParticles.begin() = m_oParticleBest;

		for(auto it = m_vecParticles.begin()+1;
			it != m_vecParticles.end(); ++it) {
			it->InitializeAround(m_oParticleBest);
		}
	}
}
