#ifndef _RHAPSODIES_TEXTOVERLAYDEBUGVIEW
#define _RHAPSODIES_TEXTOVERLAYDEBUGVIEW

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
			VistaSimpleTextOverlay *pOverlay);
		
		void Write(Slot slot, std::string sText);
		
    private:
		VistaSimpleTextOverlay *m_pOverlay;
		std::map<int, IVistaTextEntity*> m_mapSlotEntity;
	};
}

#endif // _RHAPSODIES_TEXTOVERLAYDEBUGVIEW
