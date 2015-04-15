#include "Particle.hpp"

namespace rhapsodies {

	Particle::Particle() :
		m_fIBestPenalty(std::numeric_limits<float>::max()) {
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

	void Particle::UpdateIBest(float fPenalty) {
		if(fPenalty <= m_fIBestPenalty) {
			m_fIBestPenalty    = fPenalty;
			m_oIBestModelLeft  = m_oModelLeft;
			m_oIBestModelRight = m_oModelRight;
		}
	}
	
	float Particle::GetIBestPenalty() {
		return m_fIBestPenalty;
	}

	void Particle::Imitate(Particle &other,
						   float phi_cognitive,
						   float phi_social) {
		

		
	}	
}
