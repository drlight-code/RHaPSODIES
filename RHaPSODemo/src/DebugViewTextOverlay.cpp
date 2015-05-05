#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaSimpleTextOverlay.h>
#include <VistaKernel/DisplayManager/VistaTextEntity.h>

#include "DebugViewTextOverlay.hpp"

namespace rhapsodies {
	DebugViewTextOverlay::DebugViewTextOverlay(
		VistaDisplayManager *pDispMgr,
		VistaSimpleTextOverlay *pOverlay) :
		m_pOverlay(pOverlay) {

		IVistaTextEntity *pText;
		for(int slot = 0; slot < SLOT_LAST; ++slot) {
			pText = pDispMgr->CreateTextEntity();
			m_mapSlotEntity[slot] = pText;

			pText->SetFont("MONOSPACE", 15);
			pText->SetColor(VistaColor::YELLOW);
			pText->SetYPos(slot+1);
			pText->SetEnabled(true);

			m_pOverlay->AddText(pText, true);
		}

	}

	void DebugViewTextOverlay::Write(Slot slot, std::string sText) {
		m_mapSlotEntity[slot]->SetText(sText);
	}
}
