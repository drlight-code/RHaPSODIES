#include <set>

#include <VistaTools/VistaRandomNumberGenerator.h>

#include "Particle.hpp"

namespace {
	float RandomizeOffset(float fMaxOffset) {
		VistaRandomNumberGenerator *pRNG =
			VistaRandomNumberGenerator::GetStandardRNG();
			
		return pRNG->GenerateFloat(-fMaxOffset,
								    fMaxOffset);
	}
}

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

	void Particle::InitializeAround(Particle &oCenter) {
		VistaVector3D vecCenterPosL;
		VistaQuaternion qCenterOriL;
		VistaVector3D vecCenterPosR;
		VistaQuaternion qCenterOriR;
		float fCenterVal;

		float fMaxAngOffset = 1.0f;
		float fMaxPosOffset = 0.02f;
		float fMaxOriOffset = 0.05f;

		Particle p = oCenter;

		// randomize angular dofs
		for(int dof = 0 ; dof < HandModel::JOINTDOF_LAST ; ++dof) {
			// randomize left hand angular dofs
			fCenterVal = p.GetHandModelLeft()->GetJointAngle(dof);
			fCenterVal += RandomizeOffset(fMaxAngOffset);
			p.GetHandModelLeft()->SetJointAngle(dof, fCenterVal);

			// randomize right hand angular dofs
			fCenterVal = p.GetHandModelRight()->GetJointAngle(dof);
			fCenterVal += RandomizeOffset(fMaxAngOffset);
			p.GetHandModelRight()->SetJointAngle(dof, fCenterVal);
		}

		// randomize positions
		vecCenterPosL = p.GetHandModelLeft()->GetPosition();
		vecCenterPosR = p.GetHandModelRight()->GetPosition();
		for(int dim = 0; dim < 3; ++dim) {
			vecCenterPosL[dim] += RandomizeOffset(fMaxPosOffset);
			vecCenterPosR[dim] += RandomizeOffset(fMaxPosOffset);
		}
		p.GetHandModelLeft()->SetPosition(vecCenterPosL);
		p.GetHandModelRight()->SetPosition(vecCenterPosR);

		// randomize orientations
		qCenterOriL = p.GetHandModelLeft()->GetOrientation();
		qCenterOriR = p.GetHandModelRight()->GetOrientation();
		for(int dim = 0; dim < 4; ++dim) {
			qCenterOriL[dim] += RandomizeOffset(fMaxOriOffset);
			qCenterOriR[dim] += RandomizeOffset(fMaxOriOffset);
		}
		// re-normalize quaternions
		qCenterOriL.Normalize();
		qCenterOriR.Normalize();
			
		p.GetHandModelLeft()->SetOrientation(qCenterOriL);
		p.GetHandModelRight()->SetOrientation(qCenterOriR);

		*this = p;
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
