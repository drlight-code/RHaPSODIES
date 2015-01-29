#include <GL/glew.h>

#include <VistaBase/VistaVector3D.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaMath/VistaBoundingBox.h>

#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

#include <ShaderRegistry.hpp>

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

		glUseProgram(m_pShaderReg->GetProgram("vpos_only"));

		// for now we will upload the same model transform matrix in
		// advance to calling glDrawArrays. If this turns out to be
		// slow, we might upload all the model matrices at once at the
		// beginning and use DrawArraysInstanced to look up the
		// specific matrix in the vertex shader by the instance id.

		// bottom palm sphere
		glBindVertexArray(m_idVertexArrayObjects[SPHERE]);
		glDrawArrays(GL_TRIANGLES, 0, m_vSphereVertexData.size()/3);

		glBindVertexArray(m_idVertexArrayObjects[CYLINDER]);
		glDrawArrays(GL_TRIANGLES, 0, m_vCylinderVertexData.size()/3);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return true;
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
