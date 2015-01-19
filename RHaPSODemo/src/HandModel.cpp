#include "HandModel.hpp"

namespace rhapsodies {
	HandModel::HandModel() :
		m_vecJointAngles(20, 0) {

	}

	float HandModel::GetJointAngle(JointDOF dof) {
		return m_vecJointAngles[dof];
	}

	void HandModel::SetJointAngle(JointDOF dof, float angle) {
		m_vecJointAngles[dof] = angle;
	}
}
