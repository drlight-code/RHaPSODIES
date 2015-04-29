#include <string>

#include <GL/glew.h>

#include <VistaAspects/VistaDeSerializer.h>
#include <VistaTools/VistaEnvironment.h>

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
	const std::string RHaPSODIES::sRDIniFile          = "configfiles/rhapsodies.ini";
	const std::string RHaPSODIES::sCameraSectionName  = "CAMERA";
	const std::string RHaPSODIES::sTrackerSectionName = "HANDTRACKER";

	ShaderRegistry *RHaPSODIES::S_pShaderRegistry = NULL;

	bool RHaPSODIES::Initialize() {
		S_pShaderRegistry = new ShaderRegistry();

		glewInit();
		
   		return RegisterShaders();
	}
	
	ShaderRegistry &RHaPSODIES::GetShaderRegistry() {
		return *S_pShaderRegistry;
	}

	bool RHaPSODIES::RegisterShaders() {
		std::string sShaderPath =
			VistaEnvironment::GetEnv("RHAPSODIES_SHADER_PATH");
		
		S_pShaderRegistry->RegisterShader(
			"vert_vpos", GL_VERTEX_SHADER,
			sShaderPath + "/vpos.vert");
		S_pShaderRegistry->RegisterShader(
			"vert_vpos_indexedtransform", GL_VERTEX_SHADER,
			sShaderPath + "/vpos_indexedtransform.vert");
		S_pShaderRegistry->RegisterShader(
			"vert_vpos_vnorm_indexedtransform", GL_VERTEX_SHADER,
			sShaderPath + "/vpos_vnorm_indexedtransform.vert");
		S_pShaderRegistry->RegisterShader(
			"vert_vpos_uv", GL_VERTEX_SHADER,   
			sShaderPath + "/vpos_uv.vert");

		S_pShaderRegistry->RegisterShader(
			"frag_textured", GL_FRAGMENT_SHADER,
			sShaderPath + "/textured.frag");
		S_pShaderRegistry->RegisterShader(
			"frag_textured_uint", GL_FRAGMENT_SHADER,
			sShaderPath + "/textured_uint.frag");
		S_pShaderRegistry->RegisterShader(
			"frag_textured_uint8", GL_FRAGMENT_SHADER,
			sShaderPath + "/textured_uint8.frag");
		S_pShaderRegistry->RegisterShader(
			"frag_textured_uint_diff", GL_FRAGMENT_SHADER,
			sShaderPath + "/textured_uint_diff.frag");
		S_pShaderRegistry->RegisterShader(
			"frag_depthtexture", GL_FRAGMENT_SHADER,
			sShaderPath + "/depthtexture.frag");
		S_pShaderRegistry->RegisterShader(
			"frag_solid_green", GL_FRAGMENT_SHADER,
			sShaderPath + "/solid_green.frag");
		S_pShaderRegistry->RegisterShader(
			"frag_solid_blue", GL_FRAGMENT_SHADER,
			sShaderPath + "/solid_blue.frag");
		S_pShaderRegistry->RegisterShader(
			"frag_solid_colored", GL_FRAGMENT_SHADER,
			sShaderPath + "/solid_colored.frag");
		S_pShaderRegistry->RegisterShader(
			"frag_shaded_colored", GL_FRAGMENT_SHADER,
			sShaderPath + "/shaded_colored.frag");

		S_pShaderRegistry->RegisterShader(
			"indexed_viewport", GL_GEOMETRY_SHADER,
			sShaderPath + "/indexed_viewport.geom");

		S_pShaderRegistry->RegisterShader(
			"reduction_x", GL_COMPUTE_SHADER,
			sShaderPath + "/reduction_x.comp");
		S_pShaderRegistry->RegisterShader(
			"reduction_y", GL_COMPUTE_SHADER,
			sShaderPath + "/reduction_y.comp");
		S_pShaderRegistry->RegisterShader(
			"difference_score", GL_COMPUTE_SHADER,
			sShaderPath + "/difference_score.comp");
		S_pShaderRegistry->RegisterShader(
			"generate_transforms", GL_COMPUTE_SHADER,
			sShaderPath + "/generate_transforms.comp");

		std::vector<std::string> vec_shaders;
		vec_shaders.push_back("vert_vpos_uv");		
		vec_shaders.push_back("frag_textured");		
		S_pShaderRegistry->RegisterProgram("textured", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_uv");		
		vec_shaders.push_back("frag_textured_uint");		
		S_pShaderRegistry->RegisterProgram("textured_uint", vec_shaders);
	
		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_uv");		
		vec_shaders.push_back("frag_textured_uint8");		
		S_pShaderRegistry->RegisterProgram("textured_uint8", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_uv");		
		vec_shaders.push_back("frag_textured_uint_diff");		
		S_pShaderRegistry->RegisterProgram("textured_uint_diff", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos");
		vec_shaders.push_back("frag_solid_green");		
		S_pShaderRegistry->RegisterProgram("vpos_green", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos");
		vec_shaders.push_back("frag_solid_blue");		
		S_pShaderRegistry->RegisterProgram("vpos_blue", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos");
		vec_shaders.push_back("frag_solid_colored");		
		S_pShaderRegistry->RegisterProgram("vpos_colored", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_indexedtransform");
		vec_shaders.push_back("frag_solid_colored");
		vec_shaders.push_back("indexed_viewport");		
		S_pShaderRegistry->RegisterProgram("indexedtransform", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_vnorm_indexedtransform");
		vec_shaders.push_back("frag_shaded_colored");
//		vec_shaders.push_back("indexed_viewport");		
		S_pShaderRegistry->RegisterProgram("shaded_indexedtransform", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("difference_score");
		S_pShaderRegistry->RegisterProgram("difference_score", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("reduction_x");
		S_pShaderRegistry->RegisterProgram("reduction_x", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("reduction_y");
		S_pShaderRegistry->RegisterProgram("reduction_y", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("generate_transforms");
		S_pShaderRegistry->RegisterProgram("generate_transforms", vec_shaders);

		return true;
	}

}
