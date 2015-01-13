#include "HandModel.hpp"

HandModel::HandModel() {
	m_vecJointAngles
}

HandModel::GetJointAngle(JointDOF dof) {
	return m_vecJointAngles[dof];
}
