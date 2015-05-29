#include <VistaDataFlowNet/VdfnUtil.h>

#include "RHaPSODIES.hpp"
#include "HandTracker.hpp"
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
	const std::string g_sPortRecordFramesName   = "record_frames";
	const std::string g_sPortPlaybackFramesName = "playback_frames";
	const std::string g_sPortToggleTrackingName = "toggle_tracking";
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
		RegisterInPortPrototype( g_sPortRecordFramesName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );
		RegisterInPortPrototype( g_sPortPlaybackFramesName,
								 new TVdfnPortTypeCompare<TVdfnPort<bool> > );
		RegisterInPortPrototype( g_sPortToggleTrackingName,
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

		m_sPortRecordFrames.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortRecordFramesName, this );

		m_sPortPlaybackFrames.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortPlaybackFramesName, this );

		m_sPortToggleTracking.m_pPort =
			VdfnUtil::GetInPortTyped<TVdfnPort<bool>*>(
				g_sPortToggleTrackingName, this );

		return true;
	}

	bool HandTrackingNode::DoEvalNode() {
		if(!GetIsValid())
			return false;

		if(m_sPortDepthFrame.HasNewData()) {
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

		if(m_sPortRecordFrames.HasNewData()) {
			m_pTracker->ToggleFrameRecording();
		}

		if(m_sPortPlaybackFrames.HasNewData()) {
			m_pTracker->ToggleFramePlayback();
		}
		
		if(m_sPortToggleTracking.HasNewData()) {
			if(m_pTracker->GetIsTracking())
				m_pTracker->StopTracking();
			else
				m_pTracker->StartTracking();
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
