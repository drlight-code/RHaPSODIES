#include <array>

#include <VistaTools/VistaRandomNumberGenerator.h>

#include "HandModel.hpp"

namespace rhapsodies {
	const int iRandomAngleMin = 0;
	const int iRandomAngleMax = 40;
	const int iRandomAngleAbdMinMax = 10;
	
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

	void HandModel::SetPosition(const VistaVector3D &vPos) {
		m_vPosition = vPos;
	}
	
	VistaVector3D HandModel::GetPosition() {
		return m_vPosition;
	}
	  
	void HandModel::SetOrientation(const VistaQuaternion &qOri) {
		m_qOrientation = qOri;
	}
	
	VistaQuaternion HandModel::GetOrientation() {
		return m_qOrientation;
	}

	void HandModel::SetType(HandType eType) {
		m_eType = eType;
	}

	// @todo why is return type qualifier needed?
	HandModel::HandType HandModel::GetType() {
		return m_eType;
	}
	
	float HandModel::GetJointAngle(size_t eDOF) {
		return m_vecJointAngles[eDOF];
	}

	void HandModel::SetJointAngle(size_t eDOF, float fAngleDegrees) {
		m_vecJointAngles[eDOF] = fAngleDegrees;
	}

	float HandModel::GetExtent(size_t eExt) {
		return m_vecExtents[eExt];
	}

	void HandModel::SetExtent(size_t eExt, float fLengthMm) {
		m_vecExtents[eExt] = fLengthMm;
	}

	void HandModel::Randomize() {
		VistaRandomNumberGenerator *pRNG =
			VistaRandomNumberGenerator::GetStandardRNG();
		for(size_t index = 0 ; index < HandModel::JOINTDOF_LAST ; index++ ) {
			SetJointAngle(index, pRNG->GenerateInt32()%iRandomAngleMax);
		}

		std::array<size_t, 5> arDOFAbduct = {
			T_CMC_A, I_MCP_A, M_MCP_A, R_MCP_A, L_MCP_A
		};

		for(auto it: arDOFAbduct) {
			SetJointAngle(
				it,	pRNG->GenerateInt31()%(2*iRandomAngleAbdMinMax+1) -
				iRandomAngleAbdMinMax
				);
		}

		// for now, randomize orientation quat as well
		VistaVector3D vAxis(
			pRNG->GenerateFloat(-1.0, 1.0),
			pRNG->GenerateFloat(-1.0, 1.0),
			pRNG->GenerateFloat(-1.0, 1.0));
		vAxis.Normalize();

		SetOrientation(
			VistaQuaternion(
				VistaAxisAndAngle(
					vAxis, pRNG->GenerateFloat1())));
	}
}
