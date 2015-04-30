#ifndef _RHAPSODIES_HANDRENDERDRAW
#define _RHAPSODIES_HANDRENDERDRAW

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

namespace rhapsodies {
	class ShaderRegistry;
	class HandModel;
	class HandRenderer;
	
	class HandRenderDraw : public IVistaOpenGLDraw {
	public:
		HandRenderDraw(HandModel *pModelLeft,
					   HandModel *pModelRight,
					   HandGeometry *pModelRep);
		
		~HandRenderDraw();
		
		virtual bool Do();
		virtual bool GetBoundingBox(VistaBoundingBox &bb);

	private:
		HandModel *m_pModelLeft;
		HandModel *m_pModelRight;

		HandGeometry *m_pGeometry;
		HandRenderer *m_pRenderer;

		GLint  m_locColorUniform;
		GLuint m_idColorFragProgram;
	};
}

#endif // _RHAPSODIES_HANDRENDERDRAW
