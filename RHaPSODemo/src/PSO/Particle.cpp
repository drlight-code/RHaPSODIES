#include "Particle.hpp"

namespace rhapsodies {

	Particle::Particle() {
		m_oModelLeft.SetType(HandModel::LEFT_HAND);
		m_oModelRight.SetType(HandModel::RIGHT_HAND);

	}

	Particle::~Particle() {
	}


	HandModel& Particle::GetHandModelLeft() {
		return m_oModelLeft;
	}

	HandModel& Particle::GetHandModelRight() {
		return m_oModelRight;
	}
	
	
	float Particle::GetIBest() {
		return m_fIBest;
	}		
}
