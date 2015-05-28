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
	HandModelTestingNode::HandModelTestingNode(
		std::vector<HandModel*> vecModels) :
		m_vecModels(vecModels) {

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
			// @todo DRY store HandModel constraints in central
			// location!
			float fAbd =
				m_sPortThumbAdduction.m_pPort->GetValue() / 127.0f *
				80.0f + 10.0f;

			for(HandModel *pModel : m_vecModels) {
				pModel->SetJointAngle(HandModel::T_CMC_A, fAbd);
			}
		}
		if(m_sPortThumbFlexion.HasNewData()) {
			float fFlx =
				m_sPortThumbFlexion.m_pPort->GetValue() / 127.0f *
				100.0f - 60;

			for(HandModel *pModel : m_vecModels) {
				pModel->SetJointAngle(HandModel::T_CMC_F, fFlx);
			}
		}

		
		return true;
	}


	HandModelTestingNodeCreate::HandModelTestingNodeCreate(
		std::vector<HandModel*> vecModels) :
		m_vecModels(vecModels) {

	}
	
	IVdfnNode *HandModelTestingNodeCreate::CreateNode(
		const VistaPropertyList &oParams) const {
		HandModelTestingNode *pNode = new HandModelTestingNode(m_vecModels);

		return pNode;
	}
}
