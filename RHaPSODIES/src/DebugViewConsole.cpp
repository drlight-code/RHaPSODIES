#include <VistaBase/VistaStreamUtils.h>

#include "DebugViewConsole.hpp"

namespace rhapsodies {
	void DebugViewConsole::Write(Slot slot, std::string sText) {
		vstr::out() << sText << std::endl;
	}
}
