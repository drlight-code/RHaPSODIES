#ifndef _RHAPSODIES_CAMERAFRAMEPLAYER
#define _RHAPSODIES_CAMERAFRAMEPLAYER

namespace rhapsodies {
  class CameraFramePlayer {
  public:
	  CameraFramePlayer();

	  void SetInputFile(std::string sFile);
	  void SetLoop(bool bLoop);

	  void StartPlayback();
	  void StopPlayback();

	  bool GetIsStopped();

	  bool PlaybackFrames(
		  unsigned char  *pColorBuffer,
		  unsigned short *pDepthBuffer,
		  float          *pUVMapBuffer);
	  
  private:
	  bool m_bLoop;
	  bool m_bStopped;

	  std::string m_sInputFile;
	  std::ifstream m_iStream;

	  VistaType::systemtime m_tStart;
	  VistaType::systemtime m_tNextFrame;
  };
}

#endif // _RHAPSODIES_CAMERAFRAMEPLAYER
