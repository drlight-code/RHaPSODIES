#include <set>

#include <VistaTools/VistaRandomNumberGenerator.h>

#include "Particle.hpp"

namespace {
	void GetBoundsByJointIndex(size_t index,
							   float &fMin, float &fMax) {
		
		index %= 32;

		bool bThumb = (index / 4 == 0);
		index %= 4;

		if(bThumb) {
			if(index == 1) {
				// adduction abduction dof
				fMin =  0;
				fMax = 70;
			}
			else {
				// flexion dof
				fMin =  0;
				fMax = 90;
			}
		}
		else {
			if(index == 1) {
				// adduction abduction dof
				fMin = -30;
				fMax =  30;
			}
			else {
				// flexion dof
				fMin = 0;
				fMax = 60;
			}
		}
	}
}

namespace rhapsodies {

	Particle::Particle() {
		ResetPenalty();
		ResetVelocity();
		
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

	void Particle::ResetPenalty() {
		m_fIBestPenalty = std::numeric_limits<float>::max();
	}

	void Particle::ResetVelocity() {
		for(size_t dim = 0; dim < 64; ++dim) {
			m_aVelocity[dim] = 0;
		}
	}
	
	void Particle::Imitate(Particle &oParticleGBest,
						   float phi_cognitive,
						   float phi_social) {
		VistaRandomNumberGenerator *pRNG =
			VistaRandomNumberGenerator::GetStandardRNG();
		
		float aCurrent[64];
		float aIBest[64];
		float aGBest[64];

		float w = 0.72984f;

		Particle oParticleIBest;
		oParticleIBest.m_oModelLeft  = m_oIBestModelLeft;
		oParticleIBest.m_oModelRight = m_oIBestModelRight;

		ParticleToStateArray(*this, aCurrent);
		ParticleToStateArray(oParticleIBest, aIBest);
		ParticleToStateArray(oParticleGBest, aGBest);

		for(size_t dim = 0; dim < 64; ++dim) {
			if(dim%32 >= 28)
				continue;				
			
			float r1 = pRNG->GenerateFloat2();
			float r2 = pRNG->GenerateFloat2();

			m_aVelocity[dim] = w*(m_aVelocity[dim] +
								  phi_cognitive*r1*(aIBest[dim]-aCurrent[dim]) +
								  phi_social*r2*(aGBest[dim]-aCurrent[dim]));

			if((dim%32 >= 0  && dim%32 < 20)) {
				float fMinAngle = 0;
				float fMaxAngle = 0;

				GetBoundsByJointIndex(dim, fMinAngle, fMaxAngle);
				
				if(aCurrent[dim] < fMinAngle && m_aVelocity[dim] < 0) {
					m_aVelocity[dim] = 0;
				}
				if(aCurrent[dim] > fMaxAngle && m_aVelocity[dim] > 0) {
					m_aVelocity[dim] = 0;
				}
			}

			aCurrent[dim] += m_aVelocity[dim];								
		}

		StateArrayToParticle(*this, aCurrent);
	}

	void Particle::ParticleToStateArray(Particle &oParticle, float *aState) {
		HandModel::HandModelToStateArray(oParticle.m_oModelLeft,  aState);
		HandModel::HandModelToStateArray(oParticle.m_oModelRight, aState+32);

		aState[31] = oParticle.m_fIBestPenalty;
	}

	void Particle::StateArrayToParticle(Particle &oParticle, float *aState) {
		HandModel::StateArrayToHandModel(oParticle.m_oModelLeft,  aState);
		HandModel::StateArrayToHandModel(oParticle.m_oModelRight, aState+32);

		oParticle.m_fIBestPenalty = aState[31];
	}
}
