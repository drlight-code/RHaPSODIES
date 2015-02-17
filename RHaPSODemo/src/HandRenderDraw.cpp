#include <VistaMath/VistaBoundingBox.h>

#include <HandRenderer.hpp>

#include "HandRenderDraw.hpp"

namespace rhapsodies {
	HandRenderDraw::HandRenderDraw(HandModel *pModelLeft,
								   HandModel *pModelRight,
								   HandRenderer *pRenderer) :
		m_pModelLeft(pModelLeft),
		m_pModelRight(pModelRight),
		m_pRenderer(pRenderer) {

	}

	bool HandRenderDraw::Do() {
		m_pRenderer->DrawHand(m_pModelLeft);
		m_pRenderer->DrawHand(m_pModelRight);
		return true;
	}

	bool HandRenderDraw::GetBoundingBox(VistaBoundingBox &bb) {
		bb = VistaBoundingBox(
			VistaVector3D(-10, -10, -10),
			VistaVector3D( 10,  10,  10));
		return true;
	}
}