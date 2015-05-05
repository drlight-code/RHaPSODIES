#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaSimpleTextOverlay.h>
#include <VistaKernel/DisplayManager/VistaTextEntity.h>

#include "DebugViewTextOverlay.hpp"

namespace rhapsodies {
	DebugViewTextOverlay::DebugViewTextOverlay(
		VistaDisplayManager *pDispMgr,
		std::vector<std::string> vViewportNames) {

		for(auto &sViewportName : vViewportNames) {
			VistaViewport *pViewport = 
				pDispMgr->GetViewportByName(sViewportName);

			if(pViewport) {
				m_vOverlays.push_back(
					new VistaSimpleTextOverlay(pViewport));
			}
		}

		IVistaTextEntity *pText;
		for(int slot = 0; slot < SLOT_LAST; ++slot) {
			pText = pDispMgr->CreateTextEntity();
			m_mapSlotEntity[slot] = pText;

			pText->SetFont("MONOSPACE", 15);
			pText->SetColor(VistaColor::YELLOW);
			pText->SetYPos(slot+1);
			pText->SetEnabled(true);

			for(auto &pOverlay : m_vOverlays) {
				pOverlay->AddText(pText, true);
			}
		}

	}

	DebugViewTextOverlay::~DebugViewTextOverlay() {
		for(auto &pOverlay : m_vOverlays) {
			delete pOverlay;
		}
	}

	void DebugViewTextOverlay::Write(Slot slot, std::string sText) {
		m_mapSlotEntity[slot]->SetText(sText);
	}
}
