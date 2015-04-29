#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>

#include <VistaMath/VistaBoundingBox.h>

#include <ShaderRegistry.hpp>
#include <HandRenderer.hpp>

#include "HandRenderDraw.hpp"

namespace rhapsodies {
	HandRenderDraw::HandRenderDraw(HandModel *pModelLeft,
								   HandModel *pModelRight,
								   HandGeometry *pGeometry,
								   ShaderRegistry *pReg) :
		m_pModelLeft(pModelLeft),
		m_pModelRight(pModelRight),
		m_pGeometry(pGeometry),
		m_pRenderer(new HandRenderer(pReg, true, 10)) {
	}

	HandRenderDraw::~HandRenderDraw() {
		delete m_pRenderer;
	}

	bool HandRenderDraw::Do() {
		m_pRenderer->PreDraw();

		m_pRenderer->DrawHand(m_pModelLeft,  m_pGeometry);
		m_pRenderer->DrawHand(m_pModelRight, m_pGeometry);
		
		m_pRenderer->PerformDraw(true, 0, 0, NULL);

		m_pRenderer->PostDraw();
		
		return true;
	}

	bool HandRenderDraw::GetBoundingBox(VistaBoundingBox &bb) {
		bb = VistaBoundingBox(
			VistaVector3D(-10, -10, -10),
			VistaVector3D( 10,  10,  10));
		return true;
	}
}
