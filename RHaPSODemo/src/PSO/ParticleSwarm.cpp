#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>

#include <PSO/Particle.hpp>

#include "ParticleSwarm.hpp"

namespace {

}

namespace rhapsodies {
	ParticleSwarm::ParticleSwarm(size_t nParticles) :
		m_vecParticles(nParticles) {
	}
	
	ParticleSwarm::~ParticleSwarm() {
		for(auto p: m_vecParticles) {
			//delete p;
		}
	}

	ParticleSwarm::ParticleVec& ParticleSwarm::GetParticles() {
		return m_vecParticles;
	}
}
