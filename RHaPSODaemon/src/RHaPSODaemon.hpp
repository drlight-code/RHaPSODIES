#ifndef _RHAPSODIES_RHAPSODAEMON
#define _RHAPSODIES_RHAPSODAEMON

namespace rhapsodies {
	class HandTracker;
	class IDebugView;
	
	class RHaPSODaemon {
    public:
		RHaPSODaemon();
		~RHaPSODaemon();

		bool Initialize();
		bool Run();

    private:
		bool InitGlut();
		bool InitDeviceDriver();
		bool InitHandTracker();

		HandTracker *m_pTracker;
		IDebugView  *m_pDebugView;

		unsigned char  *m_pFakeColorBuffer;
		unsigned short *m_pFakeDepthBuffer;
		float          *m_pFakeUVMapBuffer;
	};
}

#endif // _RHAPSODIES_RHAPSODAEMON
