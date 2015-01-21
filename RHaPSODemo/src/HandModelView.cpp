#include <GL/glew.h>

#include <VistaBase/VistaVector3D.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaMath/VistaBoundingBox.h>

#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

#include <ShaderRegistry.hpp>

#include "HandModelView.hpp"

namespace {
	const std::string sLogPrefix = "[HandModelView] ";
}

namespace rhapsodies {
	HandModelView::HandModelView(HandModel *pModel,
								 ShaderRegistry *pReg) :
		m_pModel(pModel),
		m_pShaderReg(pReg) {

		std::vector<VistaIndexedVertex> vIndices;
		std::vector<VistaVector3D> vCoords;
		std::vector<VistaVector3D> vTexCoords;
		std::vector<VistaVector3D> vNormals;
		std::vector<VistaColor> vColors;
		
		VistaGeometryFactory::CreateEllipsoidData(
			&vIndices, &vCoords, &vTexCoords, &vNormals, &vColors);

		// generate vertex list from indices
		for( std::vector<VistaIndexedVertex>::iterator it = vIndices.begin() ;
			 it != vIndices.end() ; ++it ) {
			for( int dim = 0 ; dim < 3 ; dim++ ) {
				m_vSphereVertexData.push_back(
					vCoords[it->GetCoordinateIndex()][dim]);
			}
		}

		PrepareVertexBufferObjects();
	}

	void HandModelView::PrepareVertexBufferObjects() {
		glGenVertexArrays(1, &m_idVA);
		glBindVertexArray(m_idVA);

		glGenBuffers(1, &m_idVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_idVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_vSphereVertexData.size(),
					 &m_vSphereVertexData[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	bool HandModelView::Do() {
		// measure timings!

		glBindVertexArray(m_idVA);

		glUseProgram(m_pShaderReg->GetProgram("vpos_only"));

		glBindBuffer(GL_ARRAY_BUFFER, m_idVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vSphereVertexData.size());
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return true;
	}

	bool HandModelView::GetBoundingBox(VistaBoundingBox &bb) {
		bb = VistaBoundingBox(
			VistaVector3D(-10, -10, -10),
			VistaVector3D( 10,  10,  10));
		return true;
	}	
	
	HandModel* HandModelView::GetModel() {
		return m_pModel;
	}

	void HandModelView::SetModel(HandModel* pModel) {
		m_pModel = pModel;
	}
}
