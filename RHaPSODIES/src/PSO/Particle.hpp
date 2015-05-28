#ifndef _RHAPSODIES_PARTICLE
#define _RHAPSODIES_PARTICLE

#include <vector>
#include "../HandModel.hpp"

namespace rhapsodies {

	/**
	 * 
	 */
	class Particle {
    public:
		Particle();
		~Particle();
		
		HandModel *GetHandModelLeft();
		HandModel *GetHandModelRight();

		HandModel *GetIBestModelLeft();
		HandModel *GetIBestModelRight();

		float GetIBestPenalty();
		void SetIBestPenalty(float fPenalty);
		
		std::vector<float>& GetVelocity();
		void SetVelocity(std::vector<float> &vecVelocity);
		void ResetVelocity();
		
		void InitializeAround(Particle &oCenter);

		static void StateArrayToParticle(Particle *pParticle, float *aState);
		static void ParticleToStateArray(Particle *pParticle, float *aState);		

    private:
		HandModel m_oModelLeft;
		HandModel m_oModelRight;
		
		HandModel m_oIBestModelLeft;
		HandModel m_oIBestModelRight;

		float     m_fIBestPenalty;

		std::vector<float> m_vecVelocity;
	};
}

#endif // _RHAPSODIES_PARTICLE
