#ifndef _RHAPSODIES_HANDMODELTESTINGNODE
#define _RHAPSODIES_HANDMODELTESTINGNODE

#include <VistaDataFlowNet/VdfnNode.h>
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaDataFlowNet/VdfnNodeFactory.h>

#include <DataFlowNet/NodeHelpers.hpp>

namespace rhapsodies {
	class HandModel;
	
	class HandModelTestingNode : public IVdfnNode {
	public:
		HandModelTestingNode(HandModel *pModel);
		
		virtual bool GetIsValid() const;
		virtual bool PrepareEvaluationRun();

	protected:
		virtual bool DoEvalNode();
		
	private:
		sPortHlp<int> m_sPortThumbAdduction;
		sPortHlp<int> m_sPortThumbFlexion;
		sPortHlp<int> m_sPortThumbRoll;

		HandModel *m_pModel;
	};

	class HandModelTestingNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
	{
	public:
		HandModelTestingNodeCreate(HandModel *pModel);
		virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;

	private:
		HandModel *m_pModel;
	};
}

#endif // _RHAPSODIES_HANDMODELTESTINGNODE
