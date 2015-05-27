#include <array>

#include <VistaTools/VistaRandomNumberGenerator.h>

#include "HandModel.hpp"

namespace rhapsodies {
	const int iRandomAngleMin = 0;
	const int iRandomAngleMax = 40;
	const int iRandomAngleAbdMinMax = 10;
	
	HandModel::HandModel() :
		m_vecJointAngles(20, 0)
	{}

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

	std::vector<float>& HandModel::GetJointAngles() {
		return m_vecJointAngles;
	}

	void HandModel::SetJointAngle(size_t eDOF, float fAngleDegrees) {
		m_vecJointAngles[eDOF] = fAngleDegrees;
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

		VistaQuaternion qRot =
			// VistaQuaternion(
			// 	VistaAxisAndAngle(
			// 		VistaVector3D(0, 1, 0), Vista::Pi)) *
			VistaQuaternion(
				VistaAxisAndAngle(
					vAxis, pRNG->GenerateFloat1()));
		
		SetOrientation(qRot);
	}

	void HandModel::HandModelToStateArray(HandModel &model, float *aState) {
		for(size_t dof = 0; dof < 20; ++dof) {
			aState[ 0+dof] = model.GetJointAngle(dof);
		}

		VistaVector3D vPos = model.GetPosition();
		for(size_t dim = 0; dim < 4; ++dim) {
			aState[20+dim] = vPos[dim];
		}

		VistaQuaternion qOri = model.GetOrientation();
		for(size_t dim = 0; dim < 4; ++dim) {
			aState[24+dim] = qOri[dim];
		}
	}

	void HandModel::StateArrayToHandModel(HandModel &model, float *aState) {
		for(size_t dof = 0; dof < 20; ++dof) {
			model.SetJointAngle(dof, aState[dof]);
		}

		VistaVector3D vPos;
		for(size_t dim = 0; dim < 4; ++dim) {
			vPos[dim] = aState[20+dim];
		}
		model.SetPosition(vPos);

		VistaQuaternion qOri;
		for(size_t dim = 0; dim < 4; ++dim) {
			qOri[dim] = aState[24+dim];
		}
		model.SetOrientation(qOri);
	}
}
