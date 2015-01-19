#include <VistaBase/VistaVector3D.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaMath/VistaBoundingBox.h>

#include <VistaKernel/GraphicsManager/VistaGeometryFactory.h>

#include "HandModelView.hpp"

namespace rhapsodies {
	HandModelView::HandModelView(HandModel *pModel,
								 VistaSceneGraph *pSceneGraph) :
		m_pModel(pModel),
		m_pSceneGraph(pSceneGraph),
		m_pGeom(NULL) {

		VistaGeometryFactory oGeomFac(pSceneGraph);
		m_pGeom = oGeomFac.CreateSphere();
		m_vecGeom.reserve(m_pGeom->GetNumberOfCoords());
		m_pGeom->GetCoordinates(m_vecGeom);		
	}
	
	bool HandModelView::Do() {
		vstr::debug() << "rendering hand model" << std::endl;

		
		
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
