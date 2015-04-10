#ifndef _RHAPSODIES_IDEBUGVIEW
#define _RHAPSODIES_IDEBUGVIEW

#include <string>

namespace rhapsodies {
	class IDebugView {
    public:
		enum Slot {
			LOOP_FPS,
			LOOP_TIME,
			CAMERAFRAMES_TIME,
			PSO_TIME,
			REDUCTION_TIME,			
			DIFFERENCE,
			UNION,
			INTERSECTION,
			PENALTY,
			MIDPIXEL_DEPTH_FIRST,
			MIDPIXEL_DEPTH_SECOND,
			FRAME_RECORDING,
			FRAME_PLAYBACK,
			SLOT_LAST
		};

		virtual ~IDebugView() {};
		virtual void Write(Slot slot, std::string sText) = 0;

    private:
	};
}

#endif // _RHAPSODIES_IDEBUGVIEW
