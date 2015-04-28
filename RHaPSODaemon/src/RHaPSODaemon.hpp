#ifndef _RHAPSODIES_RHAPSODAEMON
#define _RHAPSODIES_RHAPSODAEMON

namespace rhapsodies {
	class RHaPSODaemon {
    public:
		RHaPSODaemon();

		bool Initialize();
		bool Run();

    private:
		bool InitGlut();
		bool InitDeviceDriver();
		bool InitHandTracker();
	};
}

#endif // _RHAPSODIES_RHAPSODAEMON
