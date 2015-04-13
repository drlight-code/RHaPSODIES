#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>

#include <VistaTools/VistaRandomNumberGenerator.h>

#include <PSO/Particle.hpp>

#include "ParticleSwarm.hpp"

namespace {
	float RandomizeOffset(float fMaxOffset) {
		VistaRandomNumberGenerator *pRNG =
			VistaRandomNumberGenerator::GetStandardRNG();
			
		return pRNG->GenerateFloat(-fMaxOffset,
								    fMaxOffset);
	}
}

namespace rhapsodies {
	ParticleSwarm::ParticleSwarm(size_t nParticles) :
		m_vecParticles(nParticles) {
	}
	
	ParticleSwarm::~ParticleSwarm() {
	}

	ParticleSwarm::ParticleVec& ParticleSwarm::GetParticles() {
		return m_vecParticles;
	}

	void ParticleSwarm::InitializeAround(Particle &oCenter) {
		VistaVector3D vecCenterPosL;
		VistaQuaternion qCenterOriL;
		VistaVector3D vecCenterPosR;
		VistaQuaternion qCenterOriR;
		float fCenterVal;

		float fMaxAngOffset = 5.0f;
		float fMaxPosOffset = 0.02f;
		float fMaxOriOffset = 0.03f;

		m_vecParticles[0] = oCenter;
		
		// we start with 1 so the first particle is the unmodified
		// center position.
		for(size_t i = 1; i < m_vecParticles.size(); ++i) {
			Particle p = oCenter;

			// randomize angular dofs
			for(int i = 0 ; i < HandModel::JOINTDOF_LAST ; ++i) {
				// randomize left hand angular dofs
				fCenterVal = p.GetHandModelLeft().GetJointAngle(i);
				fCenterVal += RandomizeOffset(fMaxAngOffset);
				p.GetHandModelLeft().SetJointAngle(i, fCenterVal);

				// randomize right hand angular dofs
				fCenterVal = p.GetHandModelRight().GetJointAngle(i);
				fCenterVal += RandomizeOffset(fMaxAngOffset);
				p.GetHandModelRight().SetJointAngle(i, fCenterVal);
			}

			// randomize positions
			vecCenterPosL = p.GetHandModelLeft().GetPosition();
			vecCenterPosR = p.GetHandModelRight().GetPosition();
			for(int i = 0; i < 3; ++i) {
				vecCenterPosL[i] += RandomizeOffset(fMaxPosOffset);
				vecCenterPosR[i] += RandomizeOffset(fMaxPosOffset);
			}
			p.GetHandModelLeft().SetPosition(vecCenterPosL);
			p.GetHandModelRight().SetPosition(vecCenterPosR);

			// randomize orientations
			qCenterOriL = p.GetHandModelLeft().GetOrientation();
			qCenterOriR = p.GetHandModelRight().GetOrientation();
			for(int i = 0; i < 3; ++i) {
				qCenterOriL[i] += RandomizeOffset(fMaxOriOffset);
				qCenterOriR[i] += RandomizeOffset(fMaxOriOffset);
			}
			// re-normalize quaternions
			qCenterOriL.Normalize();
			qCenterOriR.Normalize();
			
			p.GetHandModelLeft().SetOrientation(qCenterOriL);
			p.GetHandModelRight().SetOrientation(qCenterOriR);

			m_vecParticles[i] = p;
		}
	}
}
