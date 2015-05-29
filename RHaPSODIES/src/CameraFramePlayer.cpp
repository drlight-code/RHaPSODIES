#include <fstream>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimer.h>

#include "CameraFramePlayer.hpp"

namespace rhapsodies {

	CameraFramePlayer::CameraFramePlayer() :
		m_bLoop(false),
		m_bStopped(true) {

	}

	void CameraFramePlayer::SetInputFile(std::string sFile) {
		m_sInputFile = sFile;
	}

	void CameraFramePlayer::SetLoop(bool bLoop) {
		m_bLoop = bLoop;
	}
	
	void CameraFramePlayer::StartPlayback() {
		m_bStopped = false;
		m_iStream.open(
			m_sInputFile, std::ios_base::in | std::ios_base::binary);

		if(!m_iStream.good()) {
			vstr::out() << "[CameraFramePlayer] Failed to open input stream"
						<< std::endl;
		}			

		m_tStart = VistaTimer::GetStandardTimer().GetSystemTime();

		VistaType::systemtime tDelta;
		m_iStream.read((char*)(&tDelta), 8);

		m_tNextFrame = m_tStart + tDelta;
	}

	void CameraFramePlayer::StopPlayback() {
		m_iStream.close();
		m_bStopped = true;
	}

	bool CameraFramePlayer::GetIsStopped() {
		return m_bStopped;
	}

	bool CameraFramePlayer::PlaybackFrames(
		  unsigned char  *pColorBuffer,
		  unsigned short *pDepthBuffer,
		  float          *pUVMapBuffer) {

		if(VistaTimer::GetStandardTimer().GetSystemTime() >= m_tNextFrame) {
			m_iStream.read((char*)(pColorBuffer), 320*240*3);
			m_iStream.read((char*)(pDepthBuffer), 320*240*2);
			m_iStream.read((char*)(pUVMapBuffer), 320*240*4*2);

			VistaType::systemtime tDelta;
			m_iStream.read((char*)(&tDelta), 8);
			m_tNextFrame = m_tStart + tDelta;

			if(m_iStream.eof()) {
				StopPlayback();

				if(m_bLoop)
					StartPlayback();
			}

			return true;
		}

		return false;
	}
}
