#ifndef _RHAPSODIES_HANDRENDERDRAW
#define _RHAPSODIES_HANDRENDERDRAW

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

class ShaderRegistry;

namespace rhapsodies {
	class HandModel;
	class HandRenderer;
	
	class HandRenderDraw : public IVistaOpenGLDraw {
	public:
		HandRenderDraw(HandModel *pModelLeft,
					   HandModel *pModelRight,
					   HandModelRep *pModelRep,
					   HandRenderer *pRenderer);
		
		virtual bool Do();
		virtual bool GetBoundingBox(VistaBoundingBox &bb);

		// HandModel* GetModel();
		// void SetModel(HandModel* pModel);
		
	private:
		HandModel *m_pModelLeft;
		HandModel *m_pModelRight;

		HandModelRep *m_pModelRep;

		HandRenderer *m_pRenderer;
	};
}

#endif // _RHAPSODIES_HANDRENDERDRAW
