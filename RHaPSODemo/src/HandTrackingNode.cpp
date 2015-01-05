#include "RHaPSODemo.hpp"
#include <VistaDataFlowNet/VdfnUtil.h>

#include <HandTracker.hpp>

#include "HandTrackingNode.hpp"

namespace {
	const std::string g_sPortNextClassifierName = "next_classifier";
	const std::string g_sPortPrevClassifierName = "prev_classifier";

	const std::string g_sPortColorFrameName = "color_frame";
	const std::string g_sPortDepthFrameName = "depth_frame";
}

namespace rhapsodies {
	HandTrackingNode::HandTrackingNode(HandTracker *pTracker) :
		m_pTracker(pTracker) {
		RegisterInPortPrototype( g_sPortNextClassifierName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );
		RegisterInPortPrototype( g_sPortPrevClassifierName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );

		RegisterInPortPrototype(
			g_sPortColorFrameName,
			new TVdfnPortTypeCompare<TVdfnPort<const unsigned char*> > );

		RegisterInPortPrototype(
			g_sPortDepthFrameName,
			new TVdfnPortTypeCompare<TVdfnPort<const unsigned short*> > );
	}

	bool HandTrackingNode::GetIsValid() const {
		return (m_sPortColorFrame.m_pPort &&
				m_sPortDepthFrame.m_pPort);
//		return IVdfnNode::GetIsValid();
	}
	
	bool HandTrackingNode::PrepareEvaluationRun() {
		m_sPortNextClassifier.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortNextClassifierName, this );
		
		m_sPortPrevClassifier.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortPrevClassifierName, this );

		m_sPortColorFrame.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<const unsigned char*>*>(
				g_sPortColorFrameName, this );

		m_sPortDepthFrame.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<const unsigned short*>*>(
				g_sPortDepthFrameName, this );

		return true;
	}

	bool HandTrackingNode::DoEvalNode() {
		if(m_sPortNextClassifier.HasNewData()) {
			m_pTracker->NextSkinClassifier();				
		}

		if(m_sPortPrevClassifier.HasNewData()) {
			m_pTracker->PrevSkinClassifier();
		}

		if(m_sPortDepthFrame.HasNewData()) {
			m_pTracker->FrameUpdate(
				m_sPortColorFrame.m_pPort->GetValue(),
				m_sPortDepthFrame.m_pPort->GetValue());
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
