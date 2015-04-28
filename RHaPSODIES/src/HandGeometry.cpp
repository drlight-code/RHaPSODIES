#include "HandGeometry.hpp"

namespace rhapsodies {
	
	HandGeometry::HandGeometry() :
		m_vecExtents(19) {

		m_vecExtents[T_MC] = 51;
		m_vecExtents[T_PP] = 38;
		m_vecExtents[T_DP] = 29;

		m_vecExtents[I_MC] = 92;
		m_vecExtents[I_PP] = 54;
		m_vecExtents[I_MP] = 30;
		m_vecExtents[I_DP] = 24;

		m_vecExtents[M_MC] = 91;
		m_vecExtents[M_PP] = 59;
		m_vecExtents[M_MP] = 38;
		m_vecExtents[M_DP] = 25;

		m_vecExtents[R_MC] = 88;
		m_vecExtents[R_PP] = 54;
		m_vecExtents[R_MP] = 36;
		m_vecExtents[R_DP] = 25;

		m_vecExtents[L_MC] = 87;
		m_vecExtents[L_PP] = 43;
		m_vecExtents[L_MP] = 26;
		m_vecExtents[L_DP] = 23;
	}

	float HandGeometry::GetExtent(size_t eExt) {
		return m_vecExtents[eExt];
	}

	std::vector<float> &HandGeometry::GetExtents() {
		return m_vecExtents;
	}
	
	void HandGeometry::SetExtent(size_t eExt,
								 float fLengthMm) {
		m_vecExtents[eExt] = fLengthMm;
	}
}
