#ifndef _RHAPSODIES_RHAPSODAEMON
#define _RHAPSODIES_RHAPSODAEMON

namespace rhapsodies {
	class HandTracker;

	class RHaPSODaemon {
    public:
		RHaPSODaemon();

		bool Initialize();
		bool Run();

    private:
		bool InitGlut();
		bool InitDeviceDriver();
		bool InitHandTracker();

		HandTracker *m_pTracker;
	};
}

#endif // _RHAPSODIES_RHAPSODAEMON
