#ifndef _RHAPSODIES_HANDMODELVIEW
#define _RHAPSODIES_HANDMODELVIEW

#include <vector>

#include <GL/gl.h>

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

class ShaderRegistry;

namespace rhapsodies {
	class HandModel;
	
	class HandModelView : public IVistaOpenGLDraw {
	public:
		HandModelView(HandModel *pModel,
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

#endif // _RHAPSODIES_HANDMODELVIEW
