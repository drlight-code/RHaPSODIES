#ifndef _RHAPSODIES_CAMERAFRAMERECORDER
#define _RHAPSODIES_CAMERAFRAMERECORDER

#include <fstream>

namespace rhapsodies {
  class CameraFrameRecorder {
    public:
	  CameraFrameRecorder();

	  void StartRecording();
	  void StopRecording();

	  void RecordFrames(
		  const unsigned char  *colorFrame,
		  const unsigned short *depthFrame,
		  const float          *uvMapFrame);
	  
    private:
	  std::ofstream m_oStream;
	  VistaType::microtime m_tStart;
  };
}

#endif // _RHAPSODIES_CAMERAFRAMERECORDER
