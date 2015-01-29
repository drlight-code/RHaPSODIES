#ifndef _RHAPSODIES_HANDRENDERER
#define _RHAPSODIES_HANDRENDERER

#include <vector>

#include <GL/gl.h>

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

class ShaderRegistry;

namespace rhapsodies {
	class HandModel;
	
	class HandRenderer : public IVistaOpenGLDraw {
	public:
		HandRenderer(HandModel *pModel,
					  ShaderRegistry *pReg);
		
		virtual bool Do();
		virtual bool GetBoundingBox(VistaBoundingBox &bb);

		HandModel* GetModel();
		void SetModel(HandModel* pModel);
		
	private:
		enum VBOIdIndex {
			VBO_SPHERE,
			VBO_CYLINDER,
			VBO_LAST
		};
		
		void PrepareVertexBufferObjects();

		HandModel *m_pModel;
		ShaderRegistry *m_pShaderReg;

		std::vector<float> m_vSphereVertexData;
		std::vector<float> m_vCylinderVertexData;
		
		GLuint m_idVA;
		GLuint m_idVBO[VBO_LAST];
	};
}

#endif // _RHAPSODIES_HANDRENDERER
