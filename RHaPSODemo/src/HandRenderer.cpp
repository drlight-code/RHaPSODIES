#include <GL/glew.h>

#include <VistaBase/VistaVector3D.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaMath/VistaBoundingBox.h>

#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

#include <ShaderRegistry.hpp>

#include <HandModel.hpp>

#include "HandRenderer.hpp"

namespace {
	const std::string sLogPrefix = "[HandRenderer] ";
}

namespace rhapsodies {
	HandRenderer::HandRenderer(HandModel *pModel,
							   ShaderRegistry *pReg) :
		m_pModel(pModel),
		m_pShaderReg(pReg) {

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
				m_vSphereVertexData.push_back(
					vCoords[it->GetCoordinateIndex()][dim]);
			}
		}

		vIndices.clear();
		vColors.clear();
		VistaGeometryFactory::CreateConeData(
			&vIndices, &vCoordsFloat, &vTexCoordsFloat,
			&vNormalsFloat, &vColors);

		// generate vertex list from indices
		for( it = vIndices.begin() ; it != vIndices.end() ; ++it ) {
			for( size_t dim = 0 ; dim < 3 ; dim++ ) {
				m_vCylinderVertexData.push_back(
					vCoordsFloat[3*it->GetCoordinateIndex()+dim]);
			}
		}

		PrepareVertexBufferObjects();
	}

	void HandRenderer::PrepareVertexBufferObjects() {

		// generate vertex array object names
		glGenVertexArrays(BUFFER_OBJECT_LAST, m_idVertexArrayObjects);
		// generate vertex buffer object names
		glGenBuffers(BUFFER_OBJECT_LAST, m_idBufferObjects);

		// set vertex attrib pointer for sphere and fill with static
		// data
		glBindVertexArray(m_idVertexArrayObjects[SPHERE]);
		glBindBuffer(GL_ARRAY_BUFFER, m_idBufferObjects[SPHERE]);
		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(float)*m_vSphereVertexData.size(),
					 &m_vSphereVertexData[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		// set vertex attrib pointer for cylinder and fill with static
		// data
		glBindVertexArray(m_idVertexArrayObjects[CYLINDER]);
		glBindBuffer(GL_ARRAY_BUFFER, m_idBufferObjects[CYLINDER]);
		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(float)*m_vCylinderVertexData.size(),
					 &m_vCylinderVertexData[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		
		// unbind everything for now
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	bool HandRenderer::Do() {
		// measure timings!

		GLint idProgramS = m_pShaderReg->GetProgram("vpos_green");
		GLint idProgramC = m_pShaderReg->GetProgram("vpos_blue");
		GLint locUniformS = glGetUniformLocation(idProgramS,
												 "model_transform");
		GLint locUniformC = glGetUniformLocation(idProgramC,
												 "model_transform");

		VistaTransformMatrix matModel;
		VistaTransformMatrix matTransform;

		// these go to extra vis parameter class for model pso yo
		float fPalmBottomRadius = 0.02;
		float fPalmWidth    = 0.08;
		float fPalmDiameter = fPalmWidth/4.0f;
		float fFingerDiameter = fPalmWidth/4.0f;

		// for now we average the metacarpal lengths for palm height
		float fPalmHeight =
			(m_pModel->GetExtent(HandModel::I_MC) +
			 m_pModel->GetExtent(HandModel::M_MC) +
			 m_pModel->GetExtent(HandModel::R_MC) +
			 m_pModel->GetExtent(HandModel::L_MC))/4.0f/1000.0f;
		
		// for now we will upload the same model transform matrix in
		// advance to calling glDrawArrays. If this turns out to be
		// slow, we might upload all the model matrices at once at the
		// beginning and use DrawArraysInstanced to look up the
		// specific matrix in the vertex shader by the instance id.

		glUseProgram(idProgramS);
		glBindVertexArray(m_idVertexArrayObjects[SPHERE]);

		// bottom palm cap
		matModel.Compose(
			VistaVector3D(0, fPalmBottomRadius, 0),
			VistaQuaternion(),
			VistaVector3D(fPalmWidth,
						  fPalmBottomRadius*2.0f,
						  fPalmDiameter));
		DrawSphere(matModel, locUniformS);

		// top palm cap
		matModel.Compose(
			VistaVector3D(0, fPalmBottomRadius+fPalmHeight, 0),
			VistaQuaternion(),
			VistaVector3D(fPalmWidth,
						  fPalmBottomRadius*2.0f,
						  fPalmDiameter));
		DrawSphere(matModel, locUniformS);
		
		// palm cylinder
		glUseProgram(idProgramC);
		glBindVertexArray(m_idVertexArrayObjects[CYLINDER]);
		matModel.Compose(
			VistaVector3D(0, fPalmBottomRadius + fPalmHeight/2.0f, 0),
			VistaQuaternion(),
			VistaVector3D(fPalmWidth, fPalmHeight, fPalmDiameter));
		DrawCylinder(matModel, locUniformC);

		// draw the fingers
		for(int i = 0 ; i < 4 ; i++) {
			DrawFinger(
				VistaVector3D(-fPalmWidth/2.0f + fPalmWidth/8.0f + i*fPalmWidth/4.0f,
							  fPalmBottomRadius + fPalmHeight, 0),
				fFingerDiameter,
				m_pModel->GetJointAngle(4*(1+i)),
				m_pModel->GetJointAngle(4*(1+i)+1),
				m_pModel->GetExtent(3+4*i+1),
				m_pModel->GetJointAngle(4*(1+i)+2),
				m_pModel->GetExtent(3+4*i+2),
				m_pModel->GetJointAngle(4*(1+i)+3),
				m_pModel->GetExtent(3+4*i+3),
				false);
		}		

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return true;
	}

	void HandRenderer::DrawSphere(VistaTransformMatrix matModel,
								  GLint locUniform) {
		glUniformMatrix4fv(locUniform, 1, false, matModel.GetData());
		glDrawArrays(GL_TRIANGLES, 0, m_vSphereVertexData.size()/3);
	}

	void HandRenderer::DrawCylinder(VistaTransformMatrix matModel,
									GLint locUniform) {
		glUniformMatrix4fv(locUniform, 1, false, matModel.GetData());
		glDrawArrays(GL_TRIANGLES, 0, m_vCylinderVertexData.size()/3);
	}

	void HandRenderer::DrawFinger(VistaVector3D v3Pos, float fFingerDiameter,
								  float fAng1F, float fAng1A, float fLen1,
								  float fAng2, float fLen2,
								  float fAng3, float fLen3,
								  bool bThumb) {
		
		GLint idProgramS = m_pShaderReg->GetProgram("vpos_green");
		GLint idProgramC = m_pShaderReg->GetProgram("vpos_blue");
		GLint locUniformS = glGetUniformLocation(idProgramS,
												 "model_transform");
		GLint locUniformC = glGetUniformLocation(idProgramC,
												 "model_transform");

		VistaTransformMatrix matModel;  // final applied transform
		VistaTransformMatrix matOrigin; // unscaled origin for subparts
		VistaTransformMatrix matTransform; // auxiliary for
										   // accumulative transforms

		VistaTransformMatrix matSphereScale; // reused sphere scale
		matSphereScale.SetToScaleMatrix(fFingerDiameter);
		

		glUseProgram(idProgramS);
		glBindVertexArray(m_idVertexArrayObjects[SPHERE]);

		// start at first joint
		matOrigin.SetToTranslationMatrix(v3Pos);
		matModel = matOrigin * matSphereScale;
		DrawSphere(matModel, locUniformS);
			
		// move to center of first segment
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen1/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// set scale and draw first segment cylinder
		glUseProgram(idProgramC);
		glBindVertexArray(m_idVertexArrayObjects[CYLINDER]);
		matTransform.SetToScaleMatrix(
			fFingerDiameter, fLen1/1000.0f, fFingerDiameter);
		matModel = matOrigin * matTransform;
		DrawCylinder(matModel, locUniformC);

		// move to second joint
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen1/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// draw scaled sphere
		glUseProgram(idProgramS);
		glBindVertexArray(m_idVertexArrayObjects[SPHERE]);
		matModel = matOrigin * matSphereScale;
		DrawSphere(matModel, locUniformS);
		
		// move to center of second segment
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen2/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// set scale and draw second segment cylinder
		glUseProgram(idProgramC);
		glBindVertexArray(m_idVertexArrayObjects[CYLINDER]);
		matTransform.SetToScaleMatrix(
			fFingerDiameter, fLen2/1000.0f, fFingerDiameter);
		matModel = matOrigin * matTransform;
		DrawCylinder(matModel, locUniformC);

		// move to third joint
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen2/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// draw scaled sphere
		glUseProgram(idProgramS);
		glBindVertexArray(m_idVertexArrayObjects[SPHERE]);
		matModel = matOrigin * matSphereScale;
		DrawSphere(matModel, locUniformS);
		
		// move to center of third segment
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen3/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// set scale and draw third segment cylinder
		glUseProgram(idProgramC);
		glBindVertexArray(m_idVertexArrayObjects[CYLINDER]);
		matTransform.SetToScaleMatrix(
			fFingerDiameter, fLen3/1000.0f, fFingerDiameter);
		matModel = matOrigin * matTransform;
		DrawCylinder(matModel, locUniformC);

		// move to tip
		matTransform.SetToTranslationMatrix(
			VistaVector3D(0, fLen3/1000.0f/2.0f, 0)); // rotation missing here
		matOrigin *= matTransform;

		// draw scaled sphere
		glUseProgram(idProgramS);
		glBindVertexArray(m_idVertexArrayObjects[SPHERE]);
		matModel = matOrigin * matSphereScale;
		DrawSphere(matModel, locUniformS);
	}

	bool HandRenderer::GetBoundingBox(VistaBoundingBox &bb) {
		bb = VistaBoundingBox(
			VistaVector3D(-10, -10, -10),
			VistaVector3D( 10,  10,  10));
		return true;
	}	
	
	HandModel* HandRenderer::GetModel() {
		return m_pModel;
	}

	void HandRenderer::SetModel(HandModel* pModel) {
		m_pModel = pModel;
	}
}
