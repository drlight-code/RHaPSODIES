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
	const std::string RHaPSODIES::sRDIniFile =
		"configfiles/rhapsodies.ini";
	const std::string RHaPSODIES::sCameraSectionName =
		"CAMERA";
	const std::string RHaPSODIES::sTrackerSectionName =
		"HANDTRACKER";
	const std::string RHaPSODIES::sImageProcessingSectionName =
		"IMAGE_PROCESSING";
	const std::string RHaPSODIES::sRenderingSectionName =
		"RENDERING";
	const std::string RHaPSODIES::sParticleSwarmSectionName =
		"PARTICLE_SWARM";
	const std::string RHaPSODIES::sEvaluationSectionName =
		"EVALUATION";


	ShaderRegistry *RHaPSODIES::S_pShaderRegistry = NULL;

	bool RHaPSODIES::Initialize() {
		glewInit();
		
		S_pShaderRegistry = new ShaderRegistry();
   		return RegisterShaders();
	}
	
	ShaderRegistry *RHaPSODIES::GetShaderRegistry() {
		return S_pShaderRegistry;
	}

	bool RHaPSODIES::RegisterShaders() {
		std::string sShaderPath =
			VistaEnvironment::GetEnv("RHAPSODIES_SHADER_PATH");
		
		S_pShaderRegistry->RegisterShader(
			"vert_vpos_indexedtransform", GL_VERTEX_SHADER,
			{sShaderPath + "/vpos_indexedtransform.vert"});
		S_pShaderRegistry->RegisterShader(
			"vert_vpos_vnorm_indexedtransform", GL_VERTEX_SHADER,
			{sShaderPath + "/vpos_vnorm_indexedtransform.vert"});

		S_pShaderRegistry->RegisterShader(
			"frag_solid_colored", GL_FRAGMENT_SHADER,
			{sShaderPath + "/solid_colored.frag"});
		S_pShaderRegistry->RegisterShader(
			"frag_shaded_colored", GL_FRAGMENT_SHADER,
			{sShaderPath + "/shaded_colored.frag"});

		S_pShaderRegistry->RegisterShader(
			"indexed_viewport", GL_GEOMETRY_SHADER,
			{sShaderPath + "/indexed_viewport.geom"});

		S_pShaderRegistry->RegisterShader(
			"reduction0_difference", GL_COMPUTE_SHADER,
			{sShaderPath + "/reduction_header0_difference.part",
			 sShaderPath + "/reduction.comp"});
		S_pShaderRegistry->RegisterShader(
			"reduction1_difference", GL_COMPUTE_SHADER,
			{sShaderPath + "/reduction_header1_difference.part",
			 sShaderPath + "/reduction.comp"});
		S_pShaderRegistry->RegisterShader(
			"reduction2_difference", GL_COMPUTE_SHADER,
			{sShaderPath + "/reduction_header2_difference.part",
			 sShaderPath + "/reduction.comp"});

		S_pShaderRegistry->RegisterShader(
			"reduction0_union", GL_COMPUTE_SHADER,
			{sShaderPath + "/reduction_header0_union.part",
			 sShaderPath + "/reduction.comp"});
		S_pShaderRegistry->RegisterShader(
			"reduction1_union", GL_COMPUTE_SHADER,
			{sShaderPath + "/reduction_header1_union.part",
			 sShaderPath + "/reduction.comp"});
		S_pShaderRegistry->RegisterShader(
			"reduction2_union", GL_COMPUTE_SHADER,
			{sShaderPath + "/reduction_header2_union.part",
			 sShaderPath + "/reduction.comp"});

		S_pShaderRegistry->RegisterShader(
			"reduction0_intersection", GL_COMPUTE_SHADER,
			{sShaderPath + "/reduction_header0_intersection.part",
			 sShaderPath + "/reduction.comp"});
		S_pShaderRegistry->RegisterShader(
			"reduction1_intersection", GL_COMPUTE_SHADER,
			{sShaderPath + "/reduction_header1_intersection.part",
			 sShaderPath + "/reduction.comp"});
		S_pShaderRegistry->RegisterShader(
			"reduction2_intersection", GL_COMPUTE_SHADER,
			{sShaderPath + "/reduction_header2_intersection.part",
			 sShaderPath + "/reduction.comp"});

		S_pShaderRegistry->RegisterShader(
			"prepare_reduction_textures", GL_COMPUTE_SHADER,
			{sShaderPath + "/prepare_reduction_textures.comp"});
		S_pShaderRegistry->RegisterShader(
			"generate_transforms", GL_COMPUTE_SHADER,
			{sShaderPath + "/generate_transforms.comp"});
		S_pShaderRegistry->RegisterShader(
			"update_scores", GL_COMPUTE_SHADER,
			{sShaderPath + "/update_scores.comp"});
		S_pShaderRegistry->RegisterShader(
			"update_gbest", GL_COMPUTE_SHADER,
			{sShaderPath + "/update_gbest.comp"});
		S_pShaderRegistry->RegisterShader(
			"update_swarm", GL_COMPUTE_SHADER,
			{sShaderPath + "/update_swarm.comp"});

		std::vector<std::string> vec_shaders;

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_indexedtransform");
		vec_shaders.push_back("frag_solid_colored");
		vec_shaders.push_back("indexed_viewport");		
		S_pShaderRegistry->RegisterProgram("indexedtransform", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_vnorm_indexedtransform");
		vec_shaders.push_back("frag_shaded_colored");
		S_pShaderRegistry->RegisterProgram("shaded_indexedtransform", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("prepare_reduction_textures");
		S_pShaderRegistry->RegisterProgram("prepare_reduction_textures", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("reduction0_difference");
		S_pShaderRegistry->RegisterProgram("reduction0_difference", vec_shaders);
		vec_shaders.clear();
		vec_shaders.push_back("reduction1_difference");
		S_pShaderRegistry->RegisterProgram("reduction1_difference", vec_shaders);
		vec_shaders.clear();
		vec_shaders.push_back("reduction2_difference");
		S_pShaderRegistry->RegisterProgram("reduction2_difference", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("reduction0_union");
		S_pShaderRegistry->RegisterProgram("reduction0_union", vec_shaders);
		vec_shaders.clear();
		vec_shaders.push_back("reduction1_union");
		S_pShaderRegistry->RegisterProgram("reduction1_union", vec_shaders);
		vec_shaders.clear();
		vec_shaders.push_back("reduction2_union");
		S_pShaderRegistry->RegisterProgram("reduction2_union", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("reduction0_intersection");
		S_pShaderRegistry->RegisterProgram("reduction0_intersection", vec_shaders);
		vec_shaders.clear();
		vec_shaders.push_back("reduction1_intersection");
		S_pShaderRegistry->RegisterProgram("reduction1_intersection", vec_shaders);
		vec_shaders.clear();
		vec_shaders.push_back("reduction2_intersection");
		S_pShaderRegistry->RegisterProgram("reduction2_intersection", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("generate_transforms");
		S_pShaderRegistry->RegisterProgram("generate_transforms", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("update_scores");
		S_pShaderRegistry->RegisterProgram("update_scores", vec_shaders);
		vec_shaders.clear();
		vec_shaders.push_back("update_gbest");
		S_pShaderRegistry->RegisterProgram("update_gbest", vec_shaders);
		vec_shaders.clear();
		vec_shaders.push_back("update_swarm");
		S_pShaderRegistry->RegisterProgram("update_swarm", vec_shaders);

		return true;
	}
}
