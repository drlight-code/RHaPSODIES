#include <set>

#include <VistaTools/VistaRandomNumberGenerator.h>

#include "Particle.hpp"

namespace {
	void GetBoundsByJointIndex(size_t index,
							   float &fMin, float &fMax) {
		
		index %= 20; // see JointDOF definition in HandModel.hpp
		index %= 4;
		
		if(index == 1) {
			// adduction abduction dof
			fMin = -30;
			fMax =  30;
		}
		else {
			// flexion dof
			fMin = -10;
			fMax = 100;
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
		for(size_t dim = 0; dim < 54; ++dim) {
			m_aVelocity[dim] = 0;
		}
	}
	
	void Particle::Imitate(Particle &oParticleGBest,
						   float phi_cognitive,
						   float phi_social) {
		VistaRandomNumberGenerator *pRNG =
			VistaRandomNumberGenerator::GetStandardRNG();
		
		float aCurrent[54];
		float aIBest[54];
		float aGBest[54];

		float w = 0.72984f;

		Particle oParticleIBest;
		oParticleIBest.m_oModelLeft  = m_oIBestModelLeft;
		oParticleIBest.m_oModelRight = m_oIBestModelRight;

		ParticleToStateArray(*this, aCurrent);
		ParticleToStateArray(oParticleIBest, aIBest);
		ParticleToStateArray(oParticleGBest, aGBest);

		for(size_t dim = 0; dim < 54; ++dim) {
			float r1 = pRNG->GenerateFloat2();
			float r2 = pRNG->GenerateFloat2();

			m_aVelocity[dim] = w*(m_aVelocity[dim] +
								  phi_cognitive*r1*(aIBest[dim]-aCurrent[dim]) +
								  phi_social*r2*(aGBest[dim]-aCurrent[dim]));

			if(dim < 40) {
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
		for(size_t dof = 0; dof < 20; ++dof) {
			aState[ 0+dof] = oParticle.m_oModelLeft.GetJointAngle(dof);
			aState[20+dof] = oParticle.m_oModelRight.GetJointAngle(dof);
		}

		VistaVector3D vPosL = oParticle.m_oModelLeft.GetPosition();
		VistaVector3D vPosR = oParticle.m_oModelRight.GetPosition();
		for(size_t dim = 0; dim < 3; ++dim) {
			aState[40+dim] = vPosL[dim];
			aState[47+dim] = vPosR[dim];
		}

		VistaQuaternion qOriL = oParticle.m_oModelLeft.GetOrientation();
		VistaQuaternion qOriR = oParticle.m_oModelRight.GetOrientation();
		for(size_t dim = 0; dim < 4; ++dim) {
			aState[43+dim] = qOriL[dim];
			aState[50+dim] = qOriR[dim];
		}
	}

	void Particle::StateArrayToParticle(Particle &oParticle, float *aState) {
		for(size_t dof = 0; dof < 20; ++dof) {
			oParticle.m_oModelLeft.SetJointAngle (dof, aState[ 0+dof]);
			oParticle.m_oModelRight.SetJointAngle(dof, aState[20+dof]);
		}

		VistaVector3D vPosL;
		VistaVector3D vPosR;
		for(size_t dim = 0; dim < 3; ++dim) {
			vPosL[dim] = aState[40+dim];
			vPosR[dim] = aState[47+dim];
		}
		oParticle.m_oModelLeft.SetPosition(vPosL);
		oParticle.m_oModelRight.SetPosition(vPosR);

		VistaQuaternion qOriL;
		VistaQuaternion qOriR;
		for(size_t dim = 0; dim < 4; ++dim) {
			qOriL[dim] = aState[43+dim];
			qOriR[dim] = aState[50+dim];
		}
		qOriL.Normalize();
		qOriR.Normalize();	
		oParticle.m_oModelLeft.SetOrientation(qOriL);
		oParticle.m_oModelRight.SetOrientation(qOriR);
	}
}
