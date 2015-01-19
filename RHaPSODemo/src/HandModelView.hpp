#ifndef _RHAPSODIES_HANDMODELVIEW
#define _RHAPSODIES_HANDMODELVIEW

#include <vector>

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

class VistaGeometry;

namespace rhapsodies {
	class HandModel;
	
	class HandModelView : public IVistaOpenGLDraw {
	public:
		HandModelView(HandModel *pModel,
					  VistaSceneGraph *pSceneGraph);
		
		virtual bool Do();
		virtual bool GetBoundingBox(VistaBoundingBox &bb);

		HandModel* GetModel();
		void SetModel(HandModel* pModel);
		
	private:
		HandModel *m_pModel;

		VistaSceneGraph *m_pSceneGraph;
		VistaGeometry *m_pGeom;
		std::vector<float> m_vecGeom;

	};
}

#endif // _RHAPSODIES_HANDMODELVIEW
