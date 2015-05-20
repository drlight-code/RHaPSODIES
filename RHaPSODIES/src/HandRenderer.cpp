#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaMathBasics.h>
#include <VistaBase/VistaVector3D.h>

#include <VistaMath/VistaBoundingBox.h>

#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

#include "ShaderRegistry.hpp"

#include "HandModel.hpp"
#include "HandGeometry.hpp"

#include "HandRenderer.hpp"

namespace {
	const std::string sLogPrefix = "[HandRenderer] ";
}

namespace rhapsodies {
	HandRenderer::HandRenderer(GLint idProgram,
							   bool bDrawNormals,
							   int iSegments) :
		m_idProgram(idProgram),
		m_idTransformBlock(0),
		m_bDrawNormals(bDrawNormals),
		m_szSphereData(0),
		m_szCylinderData(0) {

		m_vSphereTransforms.reserve(22*16*2);
		m_vCylinderTransforms.reserve(15*16*2);

		std::vector<VistaIndexedVertex> vIndices;
		std::vector<VistaVector3D> vCoords;
		std::vector<VistaVector3D> vTexCoords;
		std::vector<VistaVector3D> vNormals;
		std::vector<float> vCoordsFloat;
		std::vector<float> vTexCoordsFloat;
		std::vector<float> vNormalsFloat;
		std::vector<VistaColor> vColors;

		VistaGeometryFactory::CreateEllipsoidData(
			&vIndices, &vCoords, &vTexCoords, &vNormals, &vColors,
			0.5f, 0.5f, 0.5f,
			iSegments, iSegments);

		// generate vertex list from indices
		std::vector<VistaIndexedVertex>::iterator it;
		for( it = vIndices.begin() ; it != vIndices.end() ; ++it ) {
			for( int dim = 0 ; dim < 3 ; dim++ ) {
				m_vVertexData.push_back(
					vCoords[it->GetCoordinateIndex()][dim]);
				m_vNormalData.push_back(
					vNormals[it->GetNormalIndex()][dim]);
			}
		}
		m_szSphereData = vIndices.size();

		vIndices.clear();
		vColors.clear();
		VistaGeometryFactory::CreateConeData(
			&vIndices, &vCoordsFloat, &vTexCoordsFloat,
			&vNormalsFloat, &vColors,
			0.5f, 0.5f, 1.0f,
			iSegments, 1, 1
			);

		// generate vertex list from indices
		for( it = vIndices.begin() ; it != vIndices.end() ; ++it ) {
			for( size_t dim = 0 ; dim < 3 ; dim++ ) {
				m_vVertexData.push_back(
					vCoordsFloat[3*it->GetCoordinateIndex()+dim]);
				m_vNormalData.push_back(
					vNormalsFloat[3*it->GetNormalIndex()+dim]);
			}
		}
		m_szCylinderData = vIndices.size();

		PrepareBufferObjects();

		m_idTransformBlock =
			glGetProgramResourceIndex(m_idProgram,
									  GL_SHADER_STORAGE_BLOCK, "TransformBlock");

		m_locInstancesPerViewportUniform = glGetUniformLocation(
			m_idProgram, "instances_per_viewport");
		m_locTransformOffset = glGetUniformLocation(
			m_idProgram, "transform_offset");

	}

	void HandRenderer::PrepareBufferObjects() {

		// generate vertex array object names
		glGenVertexArrays(1, &m_idVertexArrayObject);
		// generate vertex buffer object names
		glGenBuffers(1, &m_idVertexBufferObject);

		// set vertex attrib pointer for sphere and fill with static
		// data
		glBindVertexArray(m_idVertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER, m_idVertexBufferObject);

		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(float)*m_vVertexData.size() +
					 sizeof(float)*m_vNormalData.size(),
					 NULL, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER,
						0,
						sizeof(float)*m_vVertexData.size(),
						&m_vVertexData[0]);
		glBufferSubData(GL_ARRAY_BUFFER,
						sizeof(float)*m_vVertexData.size(),
						sizeof(float)*m_vNormalData.size(),
						&m_vNormalData[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
							  (const GLvoid*)(sizeof(float)*m_vVertexData.size()));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		
		glBindVertexArray(0);

		// initialize SSBO for transformation matrices
		glGenBuffers(1, &m_idSSBOTransforms);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOTransforms);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 64*2*(22+16)*16*sizeof(float),
					 NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	void HandRenderer::DrawSphere(VistaTransformMatrix matModel) {
		m_vSphereTransforms.emplace_back(matModel);
	}

	void HandRenderer::DrawCylinder(VistaTransformMatrix matModel) {
		m_vCylinderTransforms.emplace_back(matModel);
	}

	void HandRenderer::DrawFinger(VistaTransformMatrix matOrigin,
								  float fFingerDiameter, float fLRFactor,
								  float fAng1F, float fAng1A, float fLen1,
								  float fAng2, float fLen2,
								  float fAng3, float fLen3,
								  bool bThumb) {
		
		// @todo invert axis here for left/right hand?
		// rotate locally around X for flexion/extension
		VistaAxisAndAngle aaaX = VistaAxisAndAngle(VistaVector3D(1,0,0), 0.0);
		// rotate locally around Z for adduction/abduction
		VistaAxisAndAngle aaaZ = VistaAxisAndAngle(VistaVector3D(0,0,1), 0.0);
		
		VistaTransformMatrix matModel;  // final applied transform
		VistaTransformMatrix matTransform; // auxiliary for
										   // accumulative transforms

		VistaTransformMatrix matSphereScale; // reused sphere scale
		matSphereScale.SetToScaleMatrix(fFingerDiameter);
		
		// start at first joint
		matModel = matOrigin * matSphereScale;
		if(!bThumb) {
			DrawSphere(matModel);
		}

		// first joint flexion rotation
		aaaX.m_fAngle = Vista::DegToRad(-fAng1F);
		matTransform = VistaTransformMatrix(aaaX);
		matOrigin *= matTransform;

		// first joint abduction rotation
		aaaZ.m_fAngle = Vista::DegToRad(fAng1A*fLRFactor);
		matTransform = VistaTransformMatrix(aaaZ);
		matOrigin *= matTransform;
		
		// move to center of first segment
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen1/1000.0f/2.0f, 0));
		matOrigin *= matTransform;

		// set scale and draw first segment
		if(bThumb) {
			matTransform.SetToScaleMatrix(
				fFingerDiameter*1.5f,
				fLen1/1000.0f*1.5f,
				fFingerDiameter*1.5f);
			matModel = matOrigin * matTransform;

			DrawSphere(matModel);
		}
		else {
			matTransform.SetToScaleMatrix(
				fFingerDiameter, fLen1/1000.0f, fFingerDiameter);
			matModel = matOrigin * matTransform;

			DrawCylinder(matModel);
		}

		// move to second joint
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen1/1000.0f/2.0f, 0));
		matOrigin *= matTransform;

		// draw scaled sphere
		matModel = matOrigin * matSphereScale;
		DrawSphere(matModel);

		// second joint flexion rotation
		aaaX.m_fAngle = Vista::DegToRad(-fAng2);
		matTransform = VistaTransformMatrix(aaaX);
		matOrigin *= matTransform;

		// move to center of second segment
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen2/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// set scale and draw second segment cylinder
		matTransform.SetToScaleMatrix(
			fFingerDiameter, fLen2/1000.0f, fFingerDiameter);
		matModel = matOrigin * matTransform;
		DrawCylinder(matModel);

		// move to third joint
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen2/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// draw scaled sphere
		matModel = matOrigin * matSphereScale;
		DrawSphere(matModel);

		// third joint flexion rotation
		aaaX.m_fAngle = Vista::DegToRad(-fAng3);
		matTransform = VistaTransformMatrix(aaaX);
		matOrigin *= matTransform;
		
		// move to center of third segment
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen3/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// set scale and draw third segment cylinder
		matTransform.SetToScaleMatrix(
			fFingerDiameter, fLen3/1000.0f, fFingerDiameter);
		matModel = matOrigin * matTransform;
		DrawCylinder(matModel);

		// move to tip
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen3/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// draw scaled sphere
		matModel = matOrigin * matSphereScale;
		DrawSphere(matModel);
	}

	void HandRenderer::DrawHand(HandModel *pModel,
								HandGeometry *pGeometry) {

		VistaTransformMatrix matModel;
		VistaTransformMatrix matTransform;
		VistaTransformMatrix matOrigin;

		// these go to extra vis parameter class for model pso yo
		float fPalmWidth        = 0.09;
		float fPalmBottomRadius = 0.01;
		float fPalmDiameter     = fPalmWidth/2.0f;
		float fFingerDiameter   = fPalmWidth/4.0f;

		// for now we average the metacarpal lengths for palm height
		float fPalmHeight =
			(pGeometry->GetExtent(HandGeometry::I_MC) +
			 pGeometry->GetExtent(HandGeometry::M_MC) +
			 pGeometry->GetExtent(HandGeometry::R_MC) +
			 pGeometry->GetExtent(HandGeometry::L_MC))/4.0f/1000.0f;


		// @todo get hand geometry straight, palm is too small...
		fPalmHeight -= fPalmBottomRadius * 2.0f;

		float fLRFactor =
			(pModel->GetType() == HandModel::LEFT_HAND) ?
			-1.0f : 1.0f;
		
		// for now we will upload the same model transform matrix in
		// advance to calling glDrawArrays. If this turns out to be
		// slow, we might upload all the model matrices at once at the
		// beginning and use DrawArraysInstanced to look up the
		// specific matrix in the vertex shader by the instance id.

		matOrigin.Compose(
			pModel->GetPosition(),
			pModel->GetOrientation(),
			VistaVector3D(1,1,1));
		
		// bottom palm cap
		matTransform.Compose(
			VistaVector3D(0, fPalmBottomRadius, 0),
			VistaQuaternion(),
			VistaVector3D(fPalmWidth,
						  fPalmBottomRadius*2.0f,
						  fPalmDiameter));
		matModel = matOrigin * matTransform;		
		DrawSphere(matModel);

		// top palm cap
		matTransform.Compose(
			VistaVector3D(0, fPalmBottomRadius+fPalmHeight, 0),
			VistaQuaternion(),
			VistaVector3D(fPalmWidth,
						  fPalmBottomRadius*2.0f,
						  fPalmDiameter));
		matModel = matOrigin * matTransform;		
		DrawSphere(matModel);
		
		// palm cylinder
		matTransform.Compose(
			VistaVector3D(0, fPalmBottomRadius + fPalmHeight/2.0f, 0),
			VistaQuaternion(),
			VistaVector3D(fPalmWidth, fPalmHeight, fPalmDiameter));
		matModel = matOrigin * matTransform;		

		DrawCylinder(matModel);

		// draw the fingers
		for(int finger = 0 ; finger < 4 ; finger++) {
			matTransform.SetToTranslationMatrix(
				VistaVector3D((-fPalmWidth/2.0f + fPalmWidth/8.0f +
							   finger*fPalmWidth/4.0f) * fLRFactor,
							  fPalmBottomRadius + fPalmHeight,
							  0));
			
			DrawFinger(
				matOrigin * matTransform,
				fFingerDiameter, fLRFactor,
				pModel->GetJointAngle(4*(1+finger)),
				pModel->GetJointAngle(4*(1+finger)+1),
				pGeometry->GetExtent(3+4*finger+1),
				pModel->GetJointAngle(4*(1+finger)+2),
				pGeometry->GetExtent(3+4*finger+2),
				pModel->GetJointAngle(4*(1+finger)+3),
				pGeometry->GetExtent(3+4*finger+3),
				false);
		}

		// draw the thumb
		matTransform.SetToTranslationMatrix(
			VistaVector3D((-fPalmWidth/2.0f + fPalmWidth/2.0f)*fLRFactor,
						  fPalmBottomRadius,
						  0));
			
		DrawFinger(
			matOrigin * matTransform,
			fFingerDiameter*1.2, fLRFactor,
			pModel->GetJointAngle(HandModel::T_CMC_F),
			pModel->GetJointAngle(HandModel::T_CMC_A),
			pGeometry->GetExtent(HandGeometry::T_MC),
			pModel->GetJointAngle(HandModel::T_MCP),
			pGeometry->GetExtent(HandGeometry::T_PP),
			pModel->GetJointAngle(HandModel::T_IP),
			pGeometry->GetExtent(HandGeometry::T_DP),
			true);

		matModel = VistaTransformMatrix(
			0,0,0,0,
			0,0,0,0,
			0,0,0,0,
			0,0,0,0);
		DrawCylinder(matModel);
	}

	void HandRenderer::PreDraw() {
		glUseProgram(m_idProgram);
		glBindVertexArray(m_idVertexArrayObject);
		glBindBuffer(GL_ARRAY_BUFFER, m_idVertexBufferObject);
	}
	
	void HandRenderer::PerformDraw(
		bool bTransformTransfer,
		unsigned int iBaseViewport,
		unsigned int iViewPortCount,
		float *pViewPortData) {


		size_t iSpheresPerViewport   = 22*2;
		size_t iCylindersPerViewport = 16*2;

		size_t iSpheresPerViewportUniform   = iSpheresPerViewport;
		size_t iCylindersPerViewportUniform = iCylindersPerViewport;

		if(iViewPortCount > 0) {
			glViewportArrayv(0, iViewPortCount,
							 &pViewPortData[4*iBaseViewport]);
		}
		else {
			// so instance id division always yields 0 in shader
			iSpheresPerViewportUniform   = ~0;
			iCylindersPerViewportUniform = ~0;

			// we use 0 to mark not setting a viewport, but for
			// following buffer size calculations we set it to 1 here!
			iViewPortCount = 1; 
		}			

		if(bTransformTransfer) {
			// bind and fill sphere transform SSBO
			size_t sizeSphereTransforms =
				sizeof(VistaTransformMatrix) *
				iSpheresPerViewport *
				iViewPortCount;
			size_t sizeCylinderTransforms =
				sizeof(VistaTransformMatrix) *
				iCylindersPerViewport *
				iViewPortCount;

			size_t sizeSpheresArray = 22*2*16;
			float *data = new float[sizeSpheresArray];
			for(size_t i = 0; i < sizeSpheresArray; ++i) {
				data[i] = 0;
			}			

			glBindBuffer(GL_SHADER_STORAGE_BUFFER,
						 m_idSSBOTransforms);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER,
							0, sizeSphereTransforms,
							&m_vSphereTransforms[0]);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER,
							sizeof(VistaTransformMatrix) *
							iSpheresPerViewport * 64, sizeCylinderTransforms,
							&m_vCylinderTransforms[0]);
			// glBufferSubData(GL_SHADER_STORAGE_BUFFER,
			// 				0, sizeSphereTransforms,
			// 				data);
			// glBufferSubData(GL_SHADER_STORAGE_BUFFER,
			// 				sizeof(VistaTransformMatrix) *
			// 				iSpheresPerViewport * 64, sizeCylinderTransforms,
			// 				data);
			delete [] data;

			m_vSphereTransforms.clear();
			m_vCylinderTransforms.clear();
		}

		// set uniform for transform buffer offset
		glUniform1ui(m_locTransformOffset,
					 iSpheresPerViewport*iBaseViewport);
		
		// set uniform for viewport indexing
		glUniform1i(m_locInstancesPerViewportUniform,
					iSpheresPerViewportUniform);
		
		// draw all shperes
		glDrawArraysInstanced(GL_TRIANGLES,
							  0, m_szSphereData,
							  iSpheresPerViewport * iViewPortCount);


		// set uniform for transform buffer offset
		glUniform1ui(m_locTransformOffset,
					 iSpheresPerViewport * 64 +
					 iCylindersPerViewport*iBaseViewport);

		// set uniform for viewport indexing
		glUniform1i(m_locInstancesPerViewportUniform,
					iCylindersPerViewportUniform);


		// draw all cylinders
		glDrawArraysInstanced(GL_TRIANGLES,
							  m_szSphereData, m_szCylinderData,
							  iCylindersPerViewport * iViewPortCount);
	}

	void HandRenderer::PostDraw() {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	GLuint HandRenderer::GetSSBOTransformsId() {
		return m_idSSBOTransforms;
	}
}
