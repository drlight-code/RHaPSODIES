#ifndef _RHAPSODIES_PARTICLE
#define _RHAPSODIES_PARTICLE

#include <HandModel.hpp>

namespace rhapsodies {

	/**
	 * 
	 */
	class Particle {
    public:
		Particle();
		~Particle();
		
		void Imitate(Particle *other,
					 float phi_individual,
					 float phi_social);

		HandModel &GetHandModelLeft();
		HandModel &GetHandModelRight();

		void UpdateIBest(float fPenalty);
		float GetIBestPenalty();
		HandModel &GetIBestModelLeft();
		HandModel &GetIBestModelRight();
		
    private:
		HandModel m_oModelLeft;
		HandModel m_oModelRight;
		
		float     m_fIBestPenalty;
		HandModel m_oIBestModelLeft;
		HandModel m_oIBestModelRight;
	};
}

#endif // _RHAPSODIES_PARTICLE
