#ifndef _RHAPSODIES_RHAPSODIES
#define _RHAPSODIES_RHAPSODIES

class IVistaDeSerializer;
IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const unsigned char* val );
IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const unsigned short* val );
IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const float* val );

namespace rhapsodies {
	class RHaPSODIES {
    public:
		static const std::string sRDIniFile;
		static const std::string sAppSectionName;
		static const std::string sCameraSectionName;
		static const std::string sTrackerSectionName;
	};
}

#endif // _RHAPSODIES_RHAPSODIES
