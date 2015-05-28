#ifndef _RHAPSODIES_HANDTRACKINGNODE
#define _RHAPSODIES_HANDTRACKINGNODE

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaDataFlowNet/VdfnNodeFactory.h>

#include <DataFlowNet/NodeHelpers.hpp>

namespace rhapsodies {
	class HandTracker;
	
	class HandTrackingNode : public IVdfnNode {
	public:
		HandTrackingNode(HandTracker *pTracker);
		
		virtual bool GetIsValid() const;
		virtual bool PrepareEvaluationRun();

	protected:
		virtual bool DoEvalNode();
		
	private:
		sPortHlp<bool> m_sPortNextClassifier;
		sPortHlp<bool> m_sPortPrevClassifier;
		sPortHlp<bool> m_sPortFrameUpdate;
		sPortHlp<bool> m_sPortShowImage;
		sPortHlp<bool> m_sPortToggleSkinMap;
		sPortHlp<bool> m_sPortRecordFrames;
		sPortHlp<bool> m_sPortPlaybackFrames;
		sPortHlp<bool> m_sPortToggleTracking;

		sPortHlp<const unsigned char*>  m_sPortColorFrame;
		sPortHlp<const unsigned short*> m_sPortDepthFrame;
		sPortHlp<const float*> m_sPortUVMapFrame;

		HandTracker *m_pTracker;
	};

	class HandTrackingNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
	{
	public:
		HandTrackingNodeCreate(HandTracker *pTracker);
		virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;

	private:
		HandTracker *m_pTracker;
	};
}

#endif // _RHAPSODIES_HANDTRACKINGNODE
