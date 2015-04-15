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

		Particle oCenterReset = oCenter;
		oCenterReset.ResetPenalty();
		
		m_vecParticles[0] = oCenterReset;
		
		// we start with 1 so the first particle is the unmodified
		// center position.
		for(size_t particle = 1; particle < m_vecParticles.size(); ++particle) {
			Particle p = oCenterReset;

			// randomize angular dofs
			for(int dof = 0 ; dof < HandModel::JOINTDOF_LAST ; ++dof) {
				// randomize left hand angular dofs
				fCenterVal = p.GetHandModelLeft().GetJointAngle(dof);
				fCenterVal += RandomizeOffset(fMaxAngOffset);
				p.GetHandModelLeft().SetJointAngle(dof, fCenterVal);

				// randomize right hand angular dofs
				fCenterVal = p.GetHandModelRight().GetJointAngle(dof);
				fCenterVal += RandomizeOffset(fMaxAngOffset);
				p.GetHandModelRight().SetJointAngle(dof, fCenterVal);
			}

			// randomize positions
			vecCenterPosL = p.GetHandModelLeft().GetPosition();
			vecCenterPosR = p.GetHandModelRight().GetPosition();
			for(int dim = 0; dim < 3; ++dim) {
				vecCenterPosL[dim] += RandomizeOffset(fMaxPosOffset);
				vecCenterPosR[dim] += RandomizeOffset(fMaxPosOffset);
			}
			p.GetHandModelLeft().SetPosition(vecCenterPosL);
			p.GetHandModelRight().SetPosition(vecCenterPosR);

			// randomize orientations
			qCenterOriL = p.GetHandModelLeft().GetOrientation();
			qCenterOriR = p.GetHandModelRight().GetOrientation();
			for(int dim = 0; dim < 4; ++dim) {
				qCenterOriL[dim] += RandomizeOffset(fMaxOriOffset);
				qCenterOriR[dim] += RandomizeOffset(fMaxOriOffset);
			}
			// re-normalize quaternions
			qCenterOriL.Normalize();
			qCenterOriR.Normalize();
			
			p.GetHandModelLeft().SetOrientation(qCenterOriL);
			p.GetHandModelRight().SetOrientation(qCenterOriR);

			m_vecParticles[particle] = p;
		}
	}

	void ParticleSwarm::Evolve() {
		// for now we try a fully meshed topology, i.e. there is only
		// one global maximum.

		Particle oParticleBest = GetBestMatch();

		// static constriction coefficient and behavioral parameters for now
		for(size_t particle = 0; particle < m_vecParticles.size(); ++particle) {
			m_vecParticles[particle].Imitate(oParticleBest, 2.8f, 1.3f);				
		}		
	}

	Particle ParticleSwarm::GetBestMatch() {
		float fPenaltyBest = std::numeric_limits<float>::max();
		int iIndexBest = 0;
		for(size_t particle = 0; particle < m_vecParticles.size(); ++particle) {
			// vstr::out() << "particle " << particle << " penalty: "
			// 			<< m_vecParticles[particle].GetIBestPenalty() << std::endl;
			
			if(m_vecParticles[particle].GetIBestPenalty() < fPenaltyBest) {
				fPenaltyBest = m_vecParticles[particle].GetIBestPenalty();
				iIndexBest = particle;
			}
		}

//		vstr::out() << "best particle: " << iIndexBest << std::endl;

		return m_vecParticles[iIndexBest];
	}
}
