#include "RHaPSODemo.hpp"
#include <VistaDataFlowNet/VdfnUtil.h>

#include <HandTracker.hpp>

#include "HandTrackingNode.hpp"

namespace {
	const std::string g_sPortColorFrameName = "color_frame";
	const std::string g_sPortDepthFrameName = "depth_frame";
	const std::string g_sPortUVMapFrameName = "uvmap_frame";

	const std::string g_sPortNextClassifierName = "next_classifier";
	const std::string g_sPortPrevClassifierName = "prev_classifier";
	const std::string g_sPortFrameUpdateName    = "set_frame_update";
	const std::string g_sPortShowImageName      = "show_image";
	const std::string g_sPortToggleSkinMapName  = "toggle_skinmap";
	const std::string g_sPortRandomizeModelName = "randomize_model";
}

namespace rhapsodies {
	HandTrackingNode::HandTrackingNode(HandTracker *pTracker) :
		m_pTracker(pTracker) {
		RegisterInPortPrototype( g_sPortNextClassifierName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );
		RegisterInPortPrototype( g_sPortPrevClassifierName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );
		RegisterInPortPrototype( g_sPortFrameUpdateName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );
		RegisterInPortPrototype( g_sPortShowImageName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );
		RegisterInPortPrototype( g_sPortToggleSkinMapName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );
		RegisterInPortPrototype( g_sPortRandomizeModelName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );

		RegisterInPortPrototype(
			g_sPortColorFrameName,
			new TVdfnPortTypeCompare<TVdfnPort<const unsigned char*> > );

		RegisterInPortPrototype(
			g_sPortDepthFrameName,
			new TVdfnPortTypeCompare<TVdfnPort<const unsigned short*> > );

		RegisterInPortPrototype(
			g_sPortUVMapFrameName,
			new TVdfnPortTypeCompare<TVdfnPort<const float*> > );
	}

	bool HandTrackingNode::GetIsValid() const {
		return (m_sPortColorFrame.m_pPort &&
				m_sPortDepthFrame.m_pPort &&
				m_sPortUVMapFrame.m_pPort);
	}
	
	bool HandTrackingNode::PrepareEvaluationRun() {
		m_sPortColorFrame.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<const unsigned char*>*>(
				g_sPortColorFrameName, this );

		m_sPortDepthFrame.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<const unsigned short*>*>(
				g_sPortDepthFrameName, this );

		m_sPortUVMapFrame.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<const float*>*>(
				g_sPortUVMapFrameName, this );

		m_sPortNextClassifier.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortNextClassifierName, this );
		
		m_sPortPrevClassifier.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortPrevClassifierName, this );

		m_sPortFrameUpdate.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortFrameUpdateName, this );

		m_sPortShowImage.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortShowImageName, this );

		m_sPortToggleSkinMap.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortToggleSkinMapName, this );

		m_sPortRandomizeModel.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortRandomizeModelName, this );
		
		return true;
	}

	bool HandTrackingNode::DoEvalNode() {
		if(!GetIsValid())
			return false;

		if(m_sPortDepthFrame.HasNewData() &&
			m_sPortFrameUpdate.m_pPort->GetValue() == true) {
			m_pTracker->FrameUpdate(
				m_sPortColorFrame.m_pPort->GetValue(),
				m_sPortDepthFrame.m_pPort->GetValue(),
				m_sPortUVMapFrame.m_pPort->GetValue());
		}

		if(m_sPortNextClassifier.HasNewData()) {
			m_pTracker->NextSkinClassifier();
		}

		if(m_sPortPrevClassifier.HasNewData()) {
			m_pTracker->PrevSkinClassifier();
		}

		if(m_sPortShowImage.HasNewData()) {
			m_pTracker->ShowOpenCVImg();
		}

		if(m_sPortToggleSkinMap.HasNewData()) {
			m_pTracker->ToggleSkinMap();
		}

		if(m_sPortRandomizeModel.HasNewData()) {
			m_pTracker->RandomizeModels();
		}
		
		return true;
	}


	HandTrackingNodeCreate::HandTrackingNodeCreate(HandTracker *pTracker) :
		m_pTracker(pTracker) {

	}
	
	IVdfnNode *HandTrackingNodeCreate::CreateNode(
		const VistaPropertyList &oParams) const {
		HandTrackingNode *pNode = new HandTrackingNode(m_pTracker);

		// const VistaPropertyList &subs = oParams.GetSubListConstRef("param");
		// std::string strObj = subs.GetValueOrDefault<std::string>("object", "");

		return pNode;
	}
}
