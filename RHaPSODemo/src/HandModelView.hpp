#ifndef _RHAPSODIES_HANDMODELVIEW
#define _RHAPSODIES_HANDMODELVIEW

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

namespace rhapsodies {
	class HandModel;
	
	class HandModelView : public IVistaOpenGLDraw {
	public:
		HandModelView();
		HandModelView(HandModel *pModel);
		
		virtual bool Do();
		virtual bool GetBoundingBox(VistaBoundingBox &bb);

		HandModel* GetModel();
		void SetModel(HandModel* pModel);
		
	private:
		HandModel *m_pModel;
	};
}

#endif // _RHAPSODIES_HANDMODELVIEW
