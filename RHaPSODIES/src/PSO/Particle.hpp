#ifndef _RHAPSODIES_PARTICLE
#define _RHAPSODIES_PARTICLE

#include "../HandModel.hpp"

namespace rhapsodies {

	/**
	 * 
	 */
	class Particle {
    public:
		Particle();
		~Particle();
		
		HandModel &GetHandModelLeft();
		HandModel &GetHandModelRight();

		void UpdateIBest(float fPenalty);
		float GetIBestPenalty();
		HandModel &GetIBestModelLeft();
		HandModel &GetIBestModelRight();

		void ResetPenalty();
		void ResetVelocity();
		
		void Imitate(Particle &other,
					 float phi_cognitive,
					 float phi_social);

		static void StateArrayToParticle(Particle &oParticle, float *aState);
		static void ParticleToStateArray(Particle &oParticle, float *aState);		

    private:
		HandModel m_oModelLeft;
		HandModel m_oModelRight;
		
		float     m_fIBestPenalty;
		HandModel m_oIBestModelLeft;
		HandModel m_oIBestModelRight;

		float m_aVelocity[64];
	};
}

#endif // _RHAPSODIES_PARTICLE
