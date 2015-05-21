#ifndef _RHAPSODIES_RHAPSODIES
#define _RHAPSODIES_RHAPSODIES

#include "ShaderRegistry.hpp"

class IVistaDeSerializer;
IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const unsigned char* val );
IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const unsigned short* val );
IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const float* val );

namespace rhapsodies {
	class RHaPSODIES {
    public:
		static const std::string sRDIniFile;
		static const std::string sCameraSectionName;
		static const std::string sTrackerSectionName;
		static const std::string sImageProcessingSectionName;
		static const std::string sRenderingSectionName;
		static const std::string sParticleSwarmSectionName;
		static const std::string sEvaluationSectionName;

		static bool Initialize();
		static ShaderRegistry *GetShaderRegistry();
		
	private:
		static bool RegisterShaders();

		static ShaderRegistry *S_pShaderRegistry;
	};
}

#endif // _RHAPSODIES_RHAPSODIES
