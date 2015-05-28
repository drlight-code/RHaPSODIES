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
		HandModelTestingNode(std::vector<HandModel*> vecModels);
		
		virtual bool GetIsValid() const;
		virtual bool PrepareEvaluationRun();

	protected:
		virtual bool DoEvalNode();
		
	private:
		sPortHlp<int> m_sPortThumbAdduction;
		sPortHlp<int> m_sPortThumbFlexion;
		sPortHlp<int> m_sPortThumbRoll;

		std::vector<HandModel*> m_vecModels;
	};

	class HandModelTestingNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator
	{
	public:
		HandModelTestingNodeCreate(std::vector<HandModel*> m_vecModels);
		virtual IVdfnNode *CreateNode( const VistaPropertyList &oParams ) const;

	private:
		std::vector<HandModel*> m_vecModels;
	};
}

#endif // _RHAPSODIES_HANDMODELTESTINGNODE
