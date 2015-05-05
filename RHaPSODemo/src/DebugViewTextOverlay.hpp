#ifndef _RHAPSODIES_DEBUGVIEWTEXTOVERLAY
#define _RHAPSODIES_DEBUGVIEWTEXTOVERLAY

#include <map>

#include <DebugView.hpp>

class VistaDisplayManager;
class VistaSimpleTextOverlay;
class IVistaTextEntity;

namespace rhapsodies {
	class DebugViewTextOverlay : public IDebugView {
    public:
		DebugViewTextOverlay(
			VistaDisplayManager *pDispMgr,
			std::vector<std::string> vViewportNames);
		virtual ~DebugViewTextOverlay();
		
		void Write(Slot slot, std::string sText);
		
    private:
		std::vector<VistaSimpleTextOverlay*> m_vOverlays;
		std::map<int, IVistaTextEntity*> m_mapSlotEntity;
	};
}

#endif // _RHAPSODIES_DEBUGVIEWTEXTOVERLAY
