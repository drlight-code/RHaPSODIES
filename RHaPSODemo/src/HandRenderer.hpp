#ifndef _RHAPSODIES_HANDRENDERER
#define _RHAPSODIES_HANDRENDERER

#include <vector>

#include <GL/gl.h>

#include <VistaBase/VistaTransformMatrix.h>


namespace rhapsodies {
	class ShaderRegistry;
	class HandModel;
	class HandModelRep;
	
	class HandRenderer {
	public:
		HandRenderer(ShaderRegistry *pReg);
		void DrawHand(HandModel *pModel,
					  HandModelRep *pModelRep);

		void PerformDraw(unsigned int iViewPortCount,
						 float *pViewPortData);
		
	private:
		enum BufferObjectId {
			SPHERE,
			CYLINDER,
			BUFFER_OBJECT_LAST
		};
		
		void PrepareBufferObjects();
		inline void DrawSphere(VistaTransformMatrix matModel);
		inline void DrawCylinder(VistaTransformMatrix matModel);
		inline void DrawFinger(VistaTransformMatrix matOrigin,
							   float fFingerDiameter, float fLRFactor,
							   float fAng1F, float fAng1A, float fLen1,
							   float fAng2, float fLen2,
							   float fAng3, float fLen3,
							   bool bThumb);

		ShaderRegistry *m_pShaderReg;

		size_t m_szSphereData;
		size_t m_szCylinderData;

		std::vector<float> m_vVertexData;
		std::vector<VistaTransformMatrix> m_vSphereTransforms;
		std::vector<VistaTransformMatrix> m_vCylinderTransforms;

		GLuint m_idVertexArrayObject;
		GLuint m_idVertexBufferObject;

		GLuint m_idUBOSphereTransforms;
		GLuint m_idUBOCylinderTransforms;

		GLint m_idProgram;
		GLint m_idTransformBlock;
		
		GLint m_locInstancesPerViewportUniform;
	};
}

#endif // _RHAPSODIES_HANDRENDERER
