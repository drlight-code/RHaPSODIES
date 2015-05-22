#ifndef _RHAPSODIES_IDEBUGVIEW
#define _RHAPSODIES_IDEBUGVIEW

#include <string>
#include <sstream>
#include <iomanip>

namespace rhapsodies {
	class IDebugView {
    public:
		enum Slot {
			CAMERAFRAMES_TIME,
			TRANSFORM_TIME,			
			RENDER_TIME,			
			REDUCTION_TIME,			
			REDUCTION_BANDWIDTH,			
			SWARMUPDATE_TIME,			
			PSO_TIME,
			LOOP_TIME,
			LOOP_FPS,
			APPLICATION_LOOP_TIME,
			APPLICATION_LOOP_FPS,
			PENALTY,
			DEPTH_TERM,
			SKIN_TERM,
			SKIN_CLASSIFIER,
			FRAME_RECORDING,
			FRAME_PLAYBACK,
			TRACKING,
			SLOT_LAST
		};

		virtual ~IDebugView() {};
		virtual void Write(Slot slot, std::string sText) = 0;
		
		template<typename T> static
		std::string	FormatString(std::string sPrefix, T value);

    private:
	};

	template<typename T>
	std::string	IDebugView::FormatString(std::string sPrefix, T value) {
		std::ostringstream ostr;

		ostr << std::setw(30) << sPrefix
			 << std::boolalpha << std::fixed << value;
		return ostr.str();
	}
}

#endif // _RHAPSODIES_IDEBUGVIEW
