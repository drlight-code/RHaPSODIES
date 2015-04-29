#ifndef _RHAPSODIES_DEBUGVIEWCONSOLE
#define _RHAPSODIES_DEBUGVIEWCONSOLE

#include "DebugView.hpp"

namespace rhapsodies {
	class DebugViewConsole : public IDebugView {
    public:
		void Write(Slot slot, std::string sText);

    private:
	};
}

#endif // _RHAPSODIES_DEBUGVIEWCONSOLE
