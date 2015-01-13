#include "HandModel.hpp"

namespace rhapsodies {
	HandModel::HandModel() :
		m_vecJointAngles(20) {

	}

	float HandModel::GetJointAngle(JointDOF dof) {
		return m_vecJointAngles[dof];
	}
}
