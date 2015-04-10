#include <string>

#include <VistaBase/VistaTimer.h>

#include "CameraFrameRecorder.hpp"

namespace rhapsodies {
	CameraFrameRecorder::CameraFrameRecorder() {

	}

	
	void CameraFrameRecorder::StartRecording() {
		std::string sFile = "resources/recordings/";
		sFile += std::to_string(
			VistaTimer::GetStandardTimer().GetSystemTime());
		sFile += ".dump";

		m_oStream.open(sFile);

		m_tStart = VistaTimer::GetStandardTimer().GetMicroTime();
	}
	
	void CameraFrameRecorder::StopRecording() {
		m_oStream.close();
	}

	void CameraFrameRecorder::RecordFrames(
		  const unsigned char  *colorFrame,
		  const unsigned short *depthFrame,
		  const float          *uvMapFrame) {
		VistaType::microtime tDiff =
			m_tStart - VistaTimer::GetStandardTimer().GetMicroTime();

		m_oStream << tDiff;
		m_oStream.write((const char*)(colorFrame), 3*320*240);
		m_oStream.write((const char*)(depthFrame), 2*320*240);
		m_oStream.write((const char*)(uvMapFrame), 4*320*240);
		m_oStream.flush();
	}
}
