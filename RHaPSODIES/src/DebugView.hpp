#ifndef _RHAPSODIES_IDEBUGVIEW
#define _RHAPSODIES_IDEBUGVIEW

#include <string>

namespace rhapsodies {
	class IDebugView {
    public:
		enum Slot {
			CAMERAFRAMES_TIME,
			TRANSFORM_TIME,			
			RENDER_TIME,			
			REDUCTION_TIME,			
			SWARMUPDATE_TIME,			
			PSO_TIME,
			LOOP_TIME,
			LOOP_FPS,
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

    private:
	};
}

#endif // _RHAPSODIES_IDEBUGVIEW
