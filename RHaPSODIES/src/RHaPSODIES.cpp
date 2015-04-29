#include <string>

#include <VistaAspects/VistaDeSerializer.h>

#include "RHaPSODIES.hpp"

IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const unsigned char* val )
{
	ser.ReadUInt64(reinterpret_cast<VistaType::uint64&>(val));
	return ser;
}

IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const unsigned short* val )
{
	ser.ReadUInt64(reinterpret_cast<VistaType::uint64&>(val));
	return ser;
}

IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const float* val )
{
	ser.ReadUInt64(reinterpret_cast<VistaType::uint64&>(val));
	return ser;
}

namespace rhapsodies {
	const std::string RHaPSODIES::sRDIniFile          = "configfiles/rhapsodemo.ini";
	const std::string RHaPSODIES::sAppSectionName     = "APPLICATION";
	const std::string RHaPSODIES::sCameraSectionName  = "CAMERA";
	const std::string RHaPSODIES::sTrackerSectionName = "HANDTRACKER";

	ShaderRegistry RHaPSODIES::S_oShaderRegistry = ShaderRegistry();
	
	ShaderRegistry &RHaPSODIES::GetShaderRegistry() {
		return S_oShaderRegistry;
	}
}
