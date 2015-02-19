#ifndef _RHAPSODIES_HANDRENDERER
#define _RHAPSODIES_HANDRENDERER

#include <vector>

#include <GL/gl.h>

#include <VistaBase/VistaTransformMatrix.h>

class ShaderRegistry;

namespace rhapsodies {
	class HandModel;
	class HandModelRep;
	
	class HandRenderer {
	public:
		HandRenderer(ShaderRegistry *pReg);
		void DrawHand(HandModel *pModel,
					  HandModelRep *pModelRep);
		
	private:
		enum BufferObjectId {
			SPHERE,
			CYLINDER,
			BUFFER_OBJECT_LAST
		};
		
		void PrepareVertexBufferObjects();
		inline void DrawSphere(VistaTransformMatrix matModel,
							   GLint locUniform);
		inline void DrawCylinder(VistaTransformMatrix matModel,
								 GLint locUniform);
		inline void DrawFinger(VistaTransformMatrix matOrigin,
							   float fFingerDiameter, float fLRFactor,
							   float fAng1F, float fAng1A, float fLen1,
							   float fAng2, float fLen2,
							   float fAng3, float fLen3,
							   bool bThumb);

		ShaderRegistry *m_pShaderReg;

		std::vector<float> m_vSphereVertexData;
		std::vector<float> m_vCylinderVertexData;
		
		GLuint m_idVertexArrayObjects[BUFFER_OBJECT_LAST];
		GLuint m_idBufferObjects[BUFFER_OBJECT_LAST];
	};
}

#endif // _RHAPSODIES_HANDRENDERER
