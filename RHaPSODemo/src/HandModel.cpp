#include "HandModel.hpp"

namespace rhapsodies {
	HandModel::HandModel() :
		m_vecJointAngles(20, 0),
		m_vecExtents(19) {

		m_vecExtents[T_MC] = 47;
		m_vecExtents[T_PP] = 44;
		m_vecExtents[T_DP] = 37;

		m_vecExtents[I_MC] = 103;
		m_vecExtents[I_PP] = 55;
		m_vecExtents[I_MP] = 35;
		m_vecExtents[I_DP] = 30;

		m_vecExtents[M_MC] = 94;
		m_vecExtents[M_PP] = 65;
		m_vecExtents[M_MP] = 43;
		m_vecExtents[M_DP] = 32;

		m_vecExtents[R_MC] = 86;
		m_vecExtents[R_PP] = 60;
		m_vecExtents[R_MP] = 37;
		m_vecExtents[R_DP] = 30;

		m_vecExtents[L_MC] = 77;
		m_vecExtents[L_PP] = 47;
		m_vecExtents[L_MP] = 28;
		m_vecExtents[L_DP] = 27;
	}

	float HandModel::GetJointAngle(JointDOF dof) {
		return m_vecJointAngles[dof];
	}

	void HandModel::SetJointAngle(JointDOF dof, float angle) {
		m_vecJointAngles[dof] = angle;
	}
}
