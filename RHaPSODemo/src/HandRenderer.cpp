#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaMathBasics.h>
#include <VistaBase/VistaVector3D.h>

#include <VistaMath/VistaBoundingBox.h>

#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

#include <ShaderRegistry.hpp>

#include <HandModel.hpp>
#include <HandModelRep.hpp>

#include "HandRenderer.hpp"

namespace {
	const std::string sLogPrefix = "[HandRenderer] ";
}

namespace rhapsodies {
	HandRenderer::HandRenderer(ShaderRegistry *pReg) :
		m_pShaderReg(pReg),
		m_szSphereData(0),
		m_szCylinderData(0) {

		m_vSphereTransforms.reserve(22*64*2);
		m_vCylinderTransforms.reserve(15*64*2);

		std::vector<VistaIndexedVertex> vIndices;
		std::vector<VistaVector3D> vCoords;
		std::vector<VistaVector3D> vTexCoords;
		std::vector<VistaVector3D> vNormals;
		std::vector<float> vCoordsFloat;
		std::vector<float> vTexCoordsFloat;
		std::vector<float> vNormalsFloat;
		std::vector<VistaColor> vColors;

		VistaGeometryFactory::CreateEllipsoidData(
			&vIndices, &vCoords, &vTexCoords, &vNormals, &vColors);

		// generate vertex list from indices
		std::vector<VistaIndexedVertex>::iterator it;
		for( it = vIndices.begin() ; it != vIndices.end() ; ++it ) {
			for( int dim = 0 ; dim < 3 ; dim++ ) {
				m_vVertexData.push_back(
					vCoords[it->GetCoordinateIndex()][dim]);
			}
		}
		m_szSphereData = vIndices.size();

		vIndices.clear();
		vColors.clear();
		VistaGeometryFactory::CreateConeData(
			&vIndices, &vCoordsFloat, &vTexCoordsFloat,
			&vNormalsFloat, &vColors);

		// generate vertex list from indices
		for( it = vIndices.begin() ; it != vIndices.end() ; ++it ) {
			for( size_t dim = 0 ; dim < 3 ; dim++ ) {
				m_vVertexData.push_back(
					vCoordsFloat[3*it->GetCoordinateIndex()+dim]);
			}
		}
		m_szCylinderData = vIndices.size();

		PrepareBufferObjects();

		m_idProgram = m_pShaderReg->GetProgram("indexedtransform");

		m_idTransformBlock =
			glGetUniformBlockIndex(m_idProgram, "TransformBlock");

		m_locInstancesPerViewportUniform = glGetUniformLocation(
			m_idProgram, "instances_per_viewport");
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
					 sizeof(float)*m_vVertexData.size(),
					 &m_vVertexData[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);


		// initialize sphere and cylinder UBOs for transformation matrices
		glGenBuffers(1, &m_idUBOSphereTransforms);
		glGenBuffers(1, &m_idUBOCylinderTransforms);

		glBindBuffer(GL_UNIFORM_BUFFER, m_idUBOSphereTransforms);
		glBufferData(GL_UNIFORM_BUFFER, 64*2*22*sizeof(VistaTransformMatrix),
					 NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, m_idUBOCylinderTransforms);
		glBufferData(GL_UNIFORM_BUFFER, 64*2*15*sizeof(VistaTransformMatrix),
					 NULL, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	void HandRenderer::DrawSphere(VistaTransformMatrix matModel) {
		// glUniformMatrix4fv(m_locUniform, 1, false, matModel.GetData());
		// glDrawArrays(GL_TRIANGLES, 0, m_vSphereVertexData.size()/3);
		m_vSphereTransforms.push_back(matModel);
//		m_aSphereTransforms.push_back(matModel);
	}

	void HandRenderer::DrawCylinder(VistaTransformMatrix matModel) {
		// glUniformMatrix4fv(m_locUniform, 1, false, matModel.GetData());
		// glDrawArrays(GL_TRIANGLES, 0, m_vCylinderVertexData.size()/3);
//		m_aCylinderTransforms.emplace_back(matModel);
		m_vCylinderTransforms.push_back(matModel);
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
								HandModelRep *pModelRep) {

		VistaTransformMatrix matModel;
		VistaTransformMatrix matTransform;
		VistaTransformMatrix matOrigin;

		// these go to extra vis parameter class for model pso yo
		float fPalmWidth        = 0.08;
		float fPalmBottomRadius = 0.02;
		float fPalmDiameter     = fPalmWidth/2.0f;
		float fFingerDiameter   = fPalmWidth/4.0f;

		// for now we average the metacarpal lengths for palm height
		float fPalmHeight =
			(pModelRep->GetExtent(HandModelRep::I_MC) +
			 pModelRep->GetExtent(HandModelRep::M_MC) +
			 pModelRep->GetExtent(HandModelRep::R_MC) +
			 pModelRep->GetExtent(HandModelRep::L_MC))/4.0f/1000.0f;

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
		
		// // bottom palm cap
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

		// @todo: if drawing turns out to be a significant bottleneck,
		// we need to adopt the approach of pre-calculating all the
		// primitive transforms, upload them as UBO's and doing
		// instances drawing on the GPU side
		
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
				pModelRep->GetExtent(3+4*finger+1),
				pModel->GetJointAngle(4*(1+finger)+2),
				pModelRep->GetExtent(3+4*finger+2),
				pModel->GetJointAngle(4*(1+finger)+3),
				pModelRep->GetExtent(3+4*finger+3),
				false);
		}

		// draw the thumb
		matTransform.SetToTranslationMatrix(
			VistaVector3D((-fPalmWidth/2.0f + fPalmWidth/8.0f)*fLRFactor,
						  fPalmBottomRadius,
						  0));
			
		DrawFinger(
			matOrigin * matTransform,
			fFingerDiameter*1.2, fLRFactor,
			pModel->GetJointAngle(HandModel::T_CMC_F),
			pModel->GetJointAngle(HandModel::T_CMC_A),
			pModelRep->GetExtent(HandModelRep::T_MC),
			pModel->GetJointAngle(HandModel::T_MCP),
			pModelRep->GetExtent(HandModelRep::T_PP),
			pModel->GetJointAngle(HandModel::T_IP),
			pModelRep->GetExtent(HandModelRep::T_DP),
			true);	

	}

	void HandRenderer::PerformDraw() {
		glUseProgram(m_idProgram);
		glBindVertexArray(m_idVertexArrayObject);

		// generate viewport array
		std::vector<float> vViewportData;
		vViewportData.reserve(64*4);
		vViewportData.clear();
		
		for(int row = 0 ; row < 8 ; ++row) {
			for(int col = 0 ; col < 8 ; ++col) {
				vViewportData.push_back(col*320);
				vViewportData.push_back(row*240);
				vViewportData.push_back(320);
				vViewportData.push_back(240);
			}
		}
//		glViewportArrayv(0, 64, &vViewportData[0]);
//		glViewport(0, 0, 320, 240);
		glViewportIndexedf(0, 0, 0, 320, 240);
		glViewportIndexedf(1, 320, 240, 320, 240);
		glViewportIndexedf(2, 640, 480, 320, 240);

		int maxvp = 0;
		glGetIntegerv(GL_MAX_VIEWPORTS, &maxvp);
		std::cout << maxvp << std::endl;

		// bind and fill sphere transform UBO
		size_t sizeUBO = sizeof(VistaTransformMatrix)*m_vSphereTransforms.size();

		glBindBuffer(GL_UNIFORM_BUFFER, m_idUBOSphereTransforms);
		glBufferSubData( GL_UNIFORM_BUFFER, 0,
						 sizeUBO,
						 &m_vSphereTransforms[0]);

		glUniformBlockBinding(m_idProgram, m_idTransformBlock, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_idUBOSphereTransforms);

		// set uniform for viewport indexing
		glUniform1i(m_locInstancesPerViewportUniform, 22*2);
		
		// draw all shperes
		glDrawArraysInstanced(GL_TRIANGLES, 0, m_szSphereData,
							  m_vSphereTransforms.size());
		
		// bind and fill cylinder transform UBO
		sizeUBO = sizeof(VistaTransformMatrix)*m_vCylinderTransforms.size();

		glBindBuffer(GL_UNIFORM_BUFFER, m_idUBOCylinderTransforms);
		glBufferSubData( GL_UNIFORM_BUFFER, 0,
						 sizeUBO,
						 &m_vCylinderTransforms[0]);

		glUniformBlockBinding(m_idProgram, m_idTransformBlock, 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_idUBOCylinderTransforms);

		// set uniform for viewport indexing
		glUniform1i(m_locInstancesPerViewportUniform, 15*2);

		// draw all cylinders
		glDrawArraysInstanced(GL_TRIANGLES, m_szSphereData, m_szCylinderData,
							  m_vCylinderTransforms.size());

		
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		// clear transform matrix vectors
		m_vSphereTransforms.clear();
		m_vCylinderTransforms.clear();
	}
}
