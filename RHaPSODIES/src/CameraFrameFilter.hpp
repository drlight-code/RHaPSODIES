#ifndef _RHAPSODIES_CAMERAFRAMEFILTER
#define _RHAPSODIES_CAMERAFRAMEFILTER

#include <list>

namespace rhapsodies {
	class SkinClassifier;
	
	class CameraFrameFilter {
    public:
		CameraFrameFilter(int iDilationSize,
						  int iErosionSize,
						  int iDepthLimit);
		~CameraFrameFilter();
		
		bool InitSkinClassifiers();

		SkinClassifier *GetSkinClassifier();
		void NextSkinClassifier();
		void PrevSkinClassifier();

		void ProcessFrames(
			unsigned char  *colorFrame,
			unsigned short *depthFrame,
			float          *uvMapFrame);

		unsigned char* GetUVMapRGB();

    private:
		void DepthToRGB(const unsigned short *depth,
						unsigned char *rgb);

		void UVMapToRGB(
			const unsigned char *color,
			const unsigned short *depth,
			const float *uvmap,
			unsigned char *rgb);

		typedef std::list<SkinClassifier*> ListSkinCl;
		ListSkinCl m_lClassifiers;
		ListSkinCl::iterator m_itCurrentClassifier;

		unsigned char m_pSkinMap[320*240];
		unsigned char m_pUVMapRGBBuffer[320*240*3];

		int m_iDilationSize;
		int m_iErosionSize;
		int m_iDepthLimit;
	};
}

#endif // _RHAPSODIES_CAMERAFRAMEFILTER
