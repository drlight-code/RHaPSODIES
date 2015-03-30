#include "Particle.hpp"

namespace rhapsodies {

	Particle::Particle() {
		// m_pModelLeft  = new HandModel;
		// m_pModelRight = new HandModel;
	}

	Particle::~Particle() {
		// delete m_pModelLeft;
		// delete m_pModelRight;
	}


	HandModel& Particle::GetModelLeft() {
		return m_oModelLeft;
	}

	HandModel& Particle::GetModelRight() {
		return m_oModelRight;
	}
	
	
	float Particle::GetIBest() {
		return m_fIBest;
	}		
}
