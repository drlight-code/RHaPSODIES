#include <set>

#include <VistaTools/VistaRandomNumberGenerator.h>

#include "Particle.hpp"

namespace rhapsodies {

	Particle::Particle() {
		m_fIBestPenalty = 1e20;

		m_vecVelocity.resize(64);
		ResetVelocity();
		
		m_oModelLeft.SetType(HandModel::LEFT_HAND);
		m_oModelRight.SetType(HandModel::RIGHT_HAND);

		m_oIBestModelLeft.SetType(HandModel::LEFT_HAND);
		m_oIBestModelRight.SetType(HandModel::RIGHT_HAND);
	}

	Particle::~Particle() {
	}

	HandModel* Particle::GetHandModelLeft() {
		return &m_oModelLeft;
	}
	
	HandModel* Particle::GetHandModelRight() {
		return &m_oModelRight;
	}

	HandModel *Particle::GetIBestModelLeft() {
		return &m_oIBestModelLeft;
	}
	
	HandModel *Particle::GetIBestModelRight() {
		return &m_oIBestModelRight;
	}

	float Particle::GetIBestPenalty() {
		return m_fIBestPenalty;
	}
	
	void Particle::SetIBestPenalty(float fPenalty) {
		m_fIBestPenalty = fPenalty;
	}

	std::vector<float>& Particle::GetVelocity() {
		return m_vecVelocity;
	}
	
	void Particle::SetVelocity(std::vector<float> &vecVelocity) {
		m_vecVelocity = vecVelocity;
	}
	
	void Particle::ResetVelocity() {
		for(float &v : m_vecVelocity) {
			v = 0;
		}
	}
	
	void Particle::ParticleToStateArray(Particle *pParticle, float *aState) {
		HandModel::HandModelToStateArray(&pParticle->m_oModelLeft,  aState);
		HandModel::HandModelToStateArray(&pParticle->m_oModelRight, aState+32);

		aState[31] = pParticle->m_fIBestPenalty;
	}

	void Particle::StateArrayToParticle(Particle *pParticle, float *aState) {
		HandModel::StateArrayToHandModel(&pParticle->m_oModelLeft,  aState);
		HandModel::StateArrayToHandModel(&pParticle->m_oModelRight, aState+32);

		pParticle->m_fIBestPenalty = aState[31];
	}
}
