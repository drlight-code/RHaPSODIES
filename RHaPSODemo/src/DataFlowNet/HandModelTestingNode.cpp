#include <VistaBase/VistaStreamUtils.h>
#include <VistaDataFlowNet/VdfnUtil.h>

#include <HandModel.hpp>

#include <DataFlowNet/HandModelTestingNode.hpp>

namespace {
	const std::string g_sPortThumbAdductionName = "thumb_adduction";
	const std::string g_sPortThumbFlexionName   = "thumb_flexion";
	const std::string g_sPortThumbRollName      = "thumb_roll";
}

namespace rhapsodies {
	HandModelTestingNode::HandModelTestingNode(HandModel *pModel) :
		m_pModel(pModel) {
		RegisterInPortPrototype( g_sPortThumbAdductionName,
								 new TVdfnPortTypeCompare<TVdfnPort<int> > );
		RegisterInPortPrototype( g_sPortThumbFlexionName,
								 new TVdfnPortTypeCompare<TVdfnPort<int> > );
		RegisterInPortPrototype( g_sPortThumbRollName,
								 new TVdfnPortTypeCompare<TVdfnPort<int> > );
	}

	bool HandModelTestingNode::GetIsValid() const {
		return true;
	}
	
	bool HandModelTestingNode::PrepareEvaluationRun() {
		m_sPortThumbAdduction.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<int>*>(
				g_sPortThumbAdductionName, this );
		m_sPortThumbFlexion.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<int>*>(
				g_sPortThumbFlexionName, this );
		m_sPortThumbRoll.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<int>*>(
				g_sPortThumbRollName, this );

		return true;
	}

	bool HandModelTestingNode::DoEvalNode() {

		if(m_sPortThumbAdduction.HasNewData()) {
			float fAbd =
				m_sPortThumbAdduction.m_pPort->GetValue() / 127.0f *
				80.0f + 10.0f;

			vstr::out() << "Abduction angle: " << fAbd << std::endl;

			m_pModel->SetJointAngle(HandModel::T_CMC_A, fAbd);
		}
		if(m_sPortThumbFlexion.HasNewData()) {
			float fFlx =
				m_sPortThumbFlexion.m_pPort->GetValue() / 127.0f *
				100.0f - 60;

			vstr::out() << "Flexion angle: " << fFlx << std::endl;

			m_pModel->SetJointAngle(HandModel::T_CMC_F, fFlx);
		}

		
		return true;
	}


	HandModelTestingNodeCreate::HandModelTestingNodeCreate(HandModel *pModel) :
		m_pModel(pModel) {

	}
	
	IVdfnNode *HandModelTestingNodeCreate::CreateNode(
		const VistaPropertyList &oParams) const {
		HandModelTestingNode *pNode = new HandModelTestingNode(m_pModel);

		return pNode;
	}
}
