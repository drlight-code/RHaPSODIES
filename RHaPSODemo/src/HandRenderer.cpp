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

		// generate and bind vertex array
		glGenVertexArrays(1, &m_idVA);
		glBindVertexArray(m_idVA);

		// generate buffer object names
		glGenBuffers(BUFFER_OBJECT_LAST, m_idBufferObjects);

		// set vertex attrib pointer for sphere vbo and fill with
		// static data
		glBindBuffer(GL_ARRAY_BUFFER, m_idBufferObjects[VBO_SPHERE]);
		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(float)*m_vSphereVertexData.size(),
					 &m_vSphereVertexData[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// set vertex attrib pointer for cylinder vbo and fill with
		// static data
		glBindBuffer(GL_ARRAY_BUFFER, m_idBufferObjects[VBO_CYLINDER]);
		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(float)*m_vCylinderVertexData.size(),
					 &m_vCylinderVertexData[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		// set vertex attrib pointer for modeltransform, but don't
		// fill with data yet
		glBindBuffer(GL_ARRAY_BUFFER,
					 m_idBufferObjects[BUFFER_OBJECT_MODELTRANSFORM]);
		glVertexAttribPointer(1, 16, GL_FLOAT, GL_FALSE, 0, 0);
		
		// unbind everything for now
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	bool HandRenderer::Do() {
		// measure timings!

		glBindVertexArray(m_idVA);

		glUseProgram(m_pShaderReg->GetProgram("vpos_only"));

		// for now we will upload the same model transform matrix in
		// advance to calling glDrawArrays. If this turns out to be
		// slow, we might upload all the model matrices at once at the
		// beginning and use DrawArraysInstanced to look up the
		// specific matrix in the vertex shader by the instance id.

		
		
		// bottom palm sphere
		glBindBuffer(GL_ARRAY_BUFFER, m_idBufferObjects[VBO_SPHERE]);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, m_vSphereVertexData.size());

		// glBindBuffer(GL_ARRAY_BUFFER, m_idBufferObjects[VBO_CYLINDER]);
		// glEnableVertexAttribArray(0);
		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		// glDrawArrays(GL_TRIANGLES, 0, m_vCylinderVertexData.size());
		
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
