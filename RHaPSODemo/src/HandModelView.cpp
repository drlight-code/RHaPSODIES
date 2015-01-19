#include <VistaBase/VistaVector3D.h>
#include <VistaBase/VistaStreamUtils.h>

#include <VistaMath/VistaBoundingBox.h>

#include "HandModelView.hpp"

namespace rhapsodies {
	HandModelView::HandModelView() :
		m_pModel(NULL) {

	}

	HandModelView::HandModelView(HandModel *pModel) :
		m_pModel(pModel) {

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
