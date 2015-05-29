/*============================================================================*/
/*                  Copyright (c) 2014 RWTH Aachen University                 */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id: $

#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include <limits>
#include <exception>
#include <algorithm>

#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <VistaTools/VistaIniFileParser.h>
#include <VistaTools/VistaRandomNumberGenerator.h>
#include <VistaTools/VistaBasicProfiler.h>

#include <VistaKernel/DisplayManager/VistaSimpleTextOverlay.h>

#include "RHaPSODIES.hpp"
#include "ShaderRegistry.hpp"

#include "HandModel.hpp"
#include "HandGeometry.hpp"
#include "HandRenderer.hpp"
#include "DebugView.hpp"

#include "PSO/Particle.hpp"
#include "PSO/ParticleSwarm.hpp"

#include "CameraFrameRecorder.hpp"
#include "CameraFramePlayer.hpp"

#include "SkinClassifiers/SkinClassifier.hpp"
#include "CameraFrameFilter.hpp"

#include "HandTracker.hpp"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
//#define PSO_TESTING

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace {
	bool CheckFrameBufferStatus(GLuint idFBO) {
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE) {
			vstr::err() << "FrameBuffer not complete: " << std::hex << status
						<< std::endl;
			
			switch(status) {
			case GL_FRAMEBUFFER_UNDEFINED:
				vstr::err() << "GL_FRAMEBUFFER_UNDEFINED"
							<< std::endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				vstr::err() << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"
							<< std::endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				vstr::err() << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"
							<< std::endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				vstr::err() << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"
							<< std::endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				vstr::err() << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER"
							<< std::endl;
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				vstr::err() << "GL_FRAMEBUFFER_UNSUPPORTED"
							<< std::endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				vstr::err() << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"
							<< std::endl;
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				vstr::err() << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"
							<< std::endl;
				break;
			}

			return false;
		}
		vstr::debug() << "FrameBuffer status complete!"
					  << std::endl << std::endl;
		return true;
	}

	bool ValidateComputeShader(GLuint idProgram) {
		glValidateProgram(idProgram);

		GLint status;
		glGetProgramiv(idProgram, GL_VALIDATE_STATUS, &status);
		if(status == GL_FALSE) {
			GLint infoLogLength;
			glGetProgramiv(idProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLchar *strInfoLog = new GLchar[infoLogLength + 1];
			glGetProgramInfoLog(idProgram, infoLogLength, NULL, strInfoLog);
			std::cerr << "Redcution shader not valid: " << strInfoLog << std::endl;
			delete[] strInfoLog;

			return false;
		}
		return true;
	}

	void PrintGpuLimits() {
		GLint values[3];

		glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, values);
		vstr::debug() << "MAX_COMPUTE_SHARED_MEMORY_SIZE:     " << values[0] << std::endl;

		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, values);
		vstr::debug() << "MAX_COMPUTE_WORK_GROUP_INVOCATIONS: " << values[0] << std::endl;

		for(size_t index = 0 ; index < 3 ; ++index)
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, index, values+index);
		vstr::debug() << "GL_MAX_COMPUTE_WORK_GROUP_COUNT:    "
					  << "[" << values[0] << ", " << values[1] << ", " << values[2]
					  << "]" << std::endl;

		for(size_t index = 0 ; index < 3 ; ++index)
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, index, values+index);
		vstr::debug() << "GL_MAX_COMPUTE_WORK_GROUP_SIZE:     "
					  << "[" << values[0] << ", " << values[1] << ", " << values[2]
					  << "]" << std::endl;

		glGetIntegerv(GL_MAX_VIEWPORTS, values);
		vstr::debug() << "GL_MAX_VIEWPORTS:                   " << values[0] << std::endl;

		glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, values);
		vstr::debug() << "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS: "
					  << values[0] << std::endl;

		glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, values);
		vstr::debug() << "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS: "
					  << values[0] << std::endl;
		
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, values);
		vstr::debug() << "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS: "
					  << values[0] << std::endl;
		
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, values);
		vstr::debug() << "GL_MAX_SHADER_STORAGE_BLOCK_SIZE: "
					  << values[0] << std::endl;

		glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, values);
		vstr::debug() << "GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT: "
					  << values[0] << std::endl;

		glGetIntegerv(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES, values);
		vstr::debug() << "GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES: "
					  << values[0] << std::endl;

		glGetIntegerv(GL_MAX_IMAGE_UNITS, values);
		vstr::debug() << "GL_MAX_IMAGE_UNITS: "
					  << values[0] << std::endl;

		vstr::debug() << std::endl;		
	}

	VistaPropertyList ReadConfigSubList(
		VistaPropertyList oConfig,
		std::string sSectionName) {
		if(!oConfig.HasProperty(sSectionName)) {
			throw std::runtime_error(
				std::string() + "Config section ["
				+ sSectionName
				+ "] not found!");			
		}
		return oConfig.GetSubListCopy(sSectionName);
	}
}

namespace rhapsodies {
	const std::string sDepthLimitName   = "DEPTH_LIMIT";
	const std::string sErosionSizeName  = "EROSION_SIZE";
	const std::string sDilationSizeName = "DILATION_SIZE";

	const std::string sPSOGenerationsName    = "PSO_GENERATIONS";
	const std::string sPhiCognitiveBeginName = "PHI_COGNITIVE_BEGIN";
	const std::string sPhiCognitiveEndName   = "PHI_COGNITIVE_END";
	const std::string sKeepKBestName         = "KEEP_KBEST";

	const std::string sRecordingName  = "RECORDING";
	const std::string sPlaybackName   = "PLAYBACK";
	const std::string sIterationsName = "ITERATIONS";
	const std::string sConditionName  = "CONDITION";
	const std::string sEvaluateName   = "EVALUATE";
	const std::string sLoopName       = "LOOP";

	const std::string sAutoTrackingName = "AUTO_TRACKING";

	const std::string sPenaltyMinName   = "PENALTY_MIN";
	const std::string sPenaltyMaxName   = "PENALTY_MAX";
	const std::string sPenaltyStartName = "PENALTY_START";

	const std::string sSmoothingFactorName = "SMOOTHING_FACTOR";
	
	const std::string sViewportBatchName = "VIEWPORT_BATCH";

	const int iSSBOHandModelsLocation         = 0;
	const int iSSBOHandGeometryLocation       = 1;
	const int iSSBOTransformsLocation         = 2;
	const int iSSBOHandModelsIBestLocation    = 4;
	const int iSSBOHandModelsGBestLocation    = 5;
	const int iSSBOHandModelsVelocityLocation = 6;
	const int iSSBORandomLocation             = 7;
	const int iSSBODebugLocation              = 8;

	const std::string sEvalOutputSuffix = ".out";

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	HandTracker::HandTracker() :
		m_bCameraUpdate(true),
		m_pShaderReg(NULL),
		m_pHandGeometry(NULL),
		m_pHandRenderer(NULL),
		m_vViewportData(4*64),
		m_pDebugView(NULL),
		m_bFrameRecording(false),
		m_bFramePlayback(false),
		m_pFrameRecorder(NULL),
		m_pFramePlayer(NULL),
		m_pFrameFilter(NULL),
		m_iEvalIteration(0),
		m_bTrackingEnabled(false),
		m_pSwarm(NULL),
		m_pHandModelLeft(NULL),
		m_pHandModelRight(NULL),
		m_pRNG(NULL),
		m_pProfiler(new VistaBasicProfiler) {

		m_pShaderReg = RHaPSODIES::GetShaderRegistry();

		m_pHandGeometry = new HandGeometry;
		std::vector<float> &vecExtents = m_pHandGeometry->GetExtents();
		for(auto &e: vecExtents) {
			e *= 0.9;
		}
		
		m_pHandRenderer =
			new HandRenderer(m_pShaderReg->GetProgram("indexedtransform"));

		m_pFrameRecorder = new CameraFrameRecorder;
		m_pFramePlayer   = new CameraFramePlayer;

		m_pColorBuffer     = new unsigned char[320*240*3];
		m_pDepthBuffer     = new unsigned short[320*240];
		m_pUVMapBuffer     = new float[320*240*2];

		m_pRNG = VistaRandomNumberGenerator::GetStandardRNG();

		m_idGenerateTransformsProgram =
			m_pShaderReg->GetProgram("generate_transforms");

		m_idPrepareReductionTexturesProgram =
			m_pShaderReg->GetProgram("prepare_reduction_textures");

		m_idReduction0DifferenceProgram = m_pShaderReg->GetProgram("reduction0_difference");
		m_idReduction1DifferenceProgram = m_pShaderReg->GetProgram("reduction1_difference");
		m_idReduction2DifferenceProgram = m_pShaderReg->GetProgram("reduction2_difference");
		m_idReduction0UnionProgram = m_pShaderReg->GetProgram("reduction0_union");
		m_idReduction1UnionProgram = m_pShaderReg->GetProgram("reduction1_union");
		m_idReduction2UnionProgram = m_pShaderReg->GetProgram("reduction2_union");
		m_idReduction0IntersectionProgram = m_pShaderReg->GetProgram("reduction0_intersection");
		m_idReduction1IntersectionProgram = m_pShaderReg->GetProgram("reduction1_intersection");
		m_idReduction2IntersectionProgram = m_pShaderReg->GetProgram("reduction2_intersection");

		m_idUpdateScoresProgram = m_pShaderReg->GetProgram("update_scores");
		m_idUpdateGBestProgram  = m_pShaderReg->GetProgram("update_gbest");
		m_idUpdateSwarmProgram  = m_pShaderReg->GetProgram("update_swarm");

		m_idColorFragProgram =
			m_pShaderReg->GetProgram("shaded_indexedtransform");
		m_locColorUniform =
			glGetUniformLocation(m_idColorFragProgram, "color_in");

		glUseProgram(m_idColorFragProgram);
		glUniform3f(m_locColorUniform, 1.0f, 0.0f, 0.0f);

		m_locRandomOffsetUniform =
			glGetUniformLocation(m_idUpdateSwarmProgram, "iRandomOffset");
		m_locPhiCognitiveUniform =
			glGetUniformLocation(m_idUpdateSwarmProgram, "fPhiCognitive");
		m_locPhiSocialUniform =
			glGetUniformLocation(m_idUpdateSwarmProgram, "fPhiSocial");
	}

	HandTracker::~HandTracker() {
		delete m_pSwarm;
		
		delete [] m_pColorBuffer;
		delete [] m_pDepthBuffer;
		delete [] m_pUVMapBuffer;
		
		delete m_pFrameFilter;
		delete m_pFramePlayer;
		delete m_pFrameRecorder;
		
		delete m_pHandRenderer;
		delete m_pHandGeometry;

		delete m_pHandModelLeft;
		delete m_pHandModelRight;
	}
	
	void HandTracker::SetDebugView(IDebugView *pDebugView) {
		m_pDebugView = pDebugView;
	}
	
	HandModel *HandTracker::GetHandModelLeft() {
		return m_pHandModelLeft;
	}
	
	HandModel *HandTracker::GetHandModelRight() {
		return m_pHandModelRight;
	}

	HandGeometry *HandTracker::GetHandGeometry() {
		return m_pHandGeometry;
	}

	void HandTracker::SetHandRenderer(HandRenderer *pRenderer) {
		m_pHandRenderer = pRenderer;
	}

	GLuint HandTracker::GetRenderedTextureId() {
		return m_idRenderedTexture;
	}

	GLuint HandTracker::GetCameraTextureId() {
		return m_idCameraTexture;
	}

	GLuint HandTracker::GetDifferenceTextureId() {
		return m_idDifferenceTexture;
	}

	void HandTracker::ReadConfig() {
		VistaIniFileParser oIniParser(true);
		oIniParser.ReadFile(RHaPSODIES::sRDIniFile);

		const VistaPropertyList oConfig = oIniParser.GetPropertyList();

		const VistaPropertyList oTrackerConfig =
			ReadConfigSubList(oConfig, RHaPSODIES::sTrackerSectionName);
		m_oConfig.bAutoTracking = oTrackerConfig.GetValueOrDefault(
			sAutoTrackingName, false);
		m_oConfig.fPenaltyMin = oTrackerConfig.GetValueOrDefault(
			sPenaltyMinName, 0.5f);
		m_oConfig.fPenaltyMax = oTrackerConfig.GetValueOrDefault(
			sPenaltyMaxName, 1.5f);
		m_oConfig.fPenaltyStart = oTrackerConfig.GetValueOrDefault(
			sPenaltyStartName, 0.6f);
		m_oConfig.fSmoothingFactor = oTrackerConfig.GetValueOrDefault(
			sSmoothingFactorName, 1.0f);


		const VistaPropertyList oCameraConfig =
			ReadConfigSubList(oConfig, RHaPSODIES::sCameraSectionName);
		std::string sIntrinsicSection =
			oCameraConfig.GetValue<std::string>("INTRINSICS");
		m_oCameraIntrinsics = oConfig.GetSubListCopy(sIntrinsicSection);

		const VistaPropertyList oImageProcessingConfig =
			ReadConfigSubList(oConfig, RHaPSODIES::sImageProcessingSectionName);
		m_oConfig.iDepthLimit = oImageProcessingConfig.GetValueOrDefault(
			sDepthLimitName, 500);
		m_oConfig.iErosionSize = oImageProcessingConfig.GetValueOrDefault(
			sErosionSizeName, 3);
		m_oConfig.iDilationSize = oImageProcessingConfig.GetValueOrDefault(
			sDilationSizeName, 5);

		const VistaPropertyList oParticleSwarmConfig =
			ReadConfigSubList(oConfig, RHaPSODIES::sParticleSwarmSectionName);
		m_oConfig.iPSOGenerations = oParticleSwarmConfig.GetValueOrDefault(
			sPSOGenerationsName, 45);
		m_oConfig.fPhiCognitiveBegin = oParticleSwarmConfig.GetValueOrDefault(
			sPhiCognitiveBeginName, 2.8);
		m_oConfig.fPhiCognitiveEnd = oParticleSwarmConfig.GetValueOrDefault(
			sPhiCognitiveEndName, 2.8);
		m_oConfig.iKeepKBest = oParticleSwarmConfig.GetValueOrDefault(
			sKeepKBestName, 0);

		const VistaPropertyList oRenderingConfig =
			ReadConfigSubList(oConfig, RHaPSODIES::sRenderingSectionName);
		m_oConfig.iViewportBatch = oRenderingConfig.GetValueOrDefault(
			sViewportBatchName, 1);

		const VistaPropertyList oEvaluationConfig =
			ReadConfigSubList(oConfig, RHaPSODIES::sEvaluationSectionName);
		m_oConfig.sRecordingFile = oEvaluationConfig.GetValueOrDefault(
			sRecordingName, std::string(""));
		m_oConfig.vecPlaybackFiles = oEvaluationConfig.GetValueOrDefault(
			sPlaybackName, std::vector<std::string>());
		m_oConfig.iIterations = oEvaluationConfig.GetValueOrDefault(
			sIterationsName, 1);
		m_oConfig.sCondition = oEvaluationConfig.GetValueOrDefault(
			sConditionName, std::string(""));
		m_oConfig.bEvaluate = oEvaluationConfig.GetValueOrDefault(
			sEvaluateName, false);
		m_oConfig.bLoop = oEvaluationConfig.GetValueOrDefault(
			sLoopName, false);
	}

	void HandTracker::PrintConfig(std::ostream &out) {
		out << "RHaPSODIES configuration:" << std::endl;

		out << "- Hand tracker:" << std::endl;
		out << "Penalty min:   " << m_oConfig.fPenaltyMin << std::endl;
		out << "Penalty max:   " << m_oConfig.fPenaltyMax << std::endl;
		out << "Penalty start: " << m_oConfig.fPenaltyStart
					<< std::endl;
		out << "Auto tracking: " << std::boolalpha
					<< m_oConfig.bAutoTracking << std::endl;
		out << "Smoothing factor: "
					<< m_oConfig.fSmoothingFactor << std::endl << std::endl;
		
		
		out << "- Image processing:" << std::endl;
		out << "Depth Limit:   " << m_oConfig.iDepthLimit
					<< std::endl;
		out << "Erosion Size:  " << m_oConfig.iErosionSize
					<< std::endl;
		out << "Dilation Size: " << m_oConfig.iDilationSize
					<< std::endl << std::endl;

		out << "- Rendering:" << std::endl;
		out << "Viewport batch:   " << m_oConfig.iViewportBatch
					<< std::endl << std::endl;
		
		out << "- Particle swarm:" << std::endl;
		out << "PSO Generations:    " << m_oConfig.iPSOGenerations
					<< std::endl;
		out << "PhiCognitive Begin: " << m_oConfig.fPhiCognitiveBegin
					<< std::endl;
		out << "PhiCognitive End:   " << m_oConfig.fPhiCognitiveEnd
					<< std::endl;
		out << "Keep k best:        " << m_oConfig.iKeepKBest
					<< std::endl << std::endl;

		out << "- Evaluation:" << std::endl;
		out << "Recording file: " << m_oConfig.sRecordingFile
					<< std::endl;

		std::string sOut = "Playback files: ";
		for(auto &s: m_oConfig.vecPlaybackFiles) {
			sOut += s + " ";
		}
		out << "Iterations:     " << m_oConfig.iIterations << std::endl;
		out << "Condition:      " << m_oConfig.sCondition << std::endl;
		out << sOut << std::endl;
		out << "Evaluate:       " << std::boolalpha << m_oConfig.bEvaluate
			<< std::endl;
		out << "Loop:           " << std::boolalpha << m_oConfig.bLoop
			<< std::endl << std::endl;
	}

	bool HandTracker::Initialize() {
		vstr::out() << "Initializing RHaPSODIES HandTracker"
					<< std::endl << std::endl;

		ReadConfig();
		PrintConfig(vstr::out());

		InitFrameFilter();
		InitRendering();

		if(HasGLComputeCapabilities()) {
			PrintGpuLimits();
			InitGpuPSO();
			InitReduction();
		}
		else {
			vstr::debug()
				<< "ARB_shader_image_load_store or ARB_compute_shader not supported!"
				<< std::endl << "Reduction stage will not be performed." << std::endl;
		}

		InitParticleSwarm();
		InitOutputModel();

		if(m_oConfig.bEvaluate)
			InitEvaluation();
		
		return true;
	}

	bool HandTracker::HasGLComputeCapabilities() {
		return GLEW_ARB_shader_image_load_store && GLEW_ARB_compute_shader;
	}

	bool HandTracker::InitFrameFilter() {
		m_pFrameFilter = new CameraFrameFilter(m_oConfig.iDilationSize,
											   m_oConfig.iErosionSize,
											   m_oConfig.iDepthLimit);
		
		bool success = m_pFrameFilter->InitSkinClassifiers();
		
		WriteDebug(IDebugView::SKIN_CLASSIFIER,
				   IDebugView::FormatString(
					   "Skin classifier: ",
					   m_pFrameFilter->GetSkinClassifier()->GetName()));

		return success;		
	}

	bool HandTracker::InitRendering() {
		// prepare texture and PBO for camera depth map
		glGenTextures(1, &m_idCameraTexture);
		glBindTexture(GL_TEXTURE_2D, m_idCameraTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
					 320*8, 240*8, 0,
					 GL_DEPTH_COMPONENT, GL_SHORT, NULL);

		glGenBuffers(1, &m_idCameraTexturePBO);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_idCameraTexturePBO);
		glBufferData(GL_PIXEL_UNPACK_BUFFER,
					 320*240*2, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		// prepare FBO rendering
		glGenTextures(1, &m_idRenderedTexture);
		glBindTexture(GL_TEXTURE_2D, m_idRenderedTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
					 320*8, 240*8, 0,
					 GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

		glGenFramebuffers(1, &m_idRenderedTextureFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_idRenderedTextureFBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							   GL_TEXTURE_2D, m_idRenderedTexture, 0);

		CheckFrameBufferStatus(m_idRenderedTextureFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// prepare constant viewport data array
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				size_t index = row*8 + col;
				m_vViewportData[4*index+0] = col*320;
				m_vViewportData[4*index+1] = row*240;
				m_vViewportData[4*index+2] = 320;
				m_vViewportData[4*index+3] = 240;
			}
		}

		return true;
	}

	bool HandTracker::InitGpuPSO() {
		// hand models SSBO
		glGenBuffers(1, &m_idSSBOHandModels);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModels);
		// 32 instead of 27 for padding
		glBufferData(GL_SHADER_STORAGE_BUFFER, 64*2*32*sizeof(float),
					 NULL, GL_DYNAMIC_DRAW);

		// hand models ibest SSBO
		glGenBuffers(1, &m_idSSBOHandModelsIBest);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsIBest);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 64*2*32*sizeof(float),
					 NULL, GL_DYNAMIC_DRAW);

		// hand models velocity SSBO
		glGenBuffers(1, &m_idSSBOHandModelsVelocity);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsVelocity);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 64*2*32*sizeof(float),
					 NULL, GL_DYNAMIC_DRAW);

		// hand models gbest SSBO
		glGenBuffers(1, &m_idSSBOHandModelsGBest);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsGBest);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 64*sizeof(float),
					 NULL, GL_DYNAMIC_DRAW);

		// hand geometry SSBO
		glGenBuffers(1, &m_idSSBOHandGeometry);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandGeometry);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 19*sizeof(float),
					 &m_pHandGeometry->GetExtents()[0], GL_DYNAMIC_DRAW);

		// random number SSBO
		glGenBuffers(1, &m_idSSBORandom);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBORandom);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 64*64*8*sizeof(float),
					 NULL, GL_DYNAMIC_DRAW);
		float *aRandom = (float*)(glMapBuffer(GL_SHADER_STORAGE_BUFFER,
											  GL_WRITE_ONLY));
		for(int i = 0; i < 64*64*8; ++i) {
			aRandom[i] = m_pRNG->GenerateFloat2();
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// debug SSBO
		glGenBuffers(1, &m_idSSBODebug);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBODebug);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 256*sizeof(float),
					 NULL, GL_DYNAMIC_DRAW);
		
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return true;
	}

	bool HandTracker::InitReduction() {
		glActiveTexture(GL_TEXTURE0);

		// prepare reduction textures
		unsigned short *data = new unsigned short[320*256*8*8];
		for(size_t i = 0; i < 320*256*8*8; ++i) {
			data[i] = 0x0;
		}
		unsigned int *data_uint = new unsigned int[320*256*8*8];
		for(size_t i = 0; i < 320*256*8*8; ++i) {
			data_uint[i] = 0x0;
		}

		// first step: 320x256
		glGenTextures(3, m_idReductionTextures320x256);
		for(size_t i = 0; i < 3; ++i) {
			glBindTexture(GL_TEXTURE_2D, m_idReductionTextures320x256[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16UI, 320*8, 256*8);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320*8, 256*8, GL_RED_INTEGER,
							GL_UNSIGNED_SHORT, data);
		}

		// second step: 40x32
		glGenTextures(3, m_idReductionTextures40x32);
		for(size_t i = 0; i < 3; ++i) {
			glBindTexture(GL_TEXTURE_2D, m_idReductionTextures40x32[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16UI, 40*8, 32*8);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 40*8, 32*8, GL_RED_INTEGER,
							GL_UNSIGNED_SHORT, data);
		}

		// third step: 5x4
		glGenTextures(3, m_idReductionTextures5x4);
		glBindTexture(GL_TEXTURE_2D, m_idReductionTextures5x4[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 5*8, 4*8);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 5*8, 4*8, GL_RED_INTEGER,
						GL_UNSIGNED_SHORT, data_uint);
		for(size_t i = 1; i < 3; ++i) {
			glBindTexture(GL_TEXTURE_2D, m_idReductionTextures5x4[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16UI, 5*8, 4*8);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 5*8, 4*8, GL_RED_INTEGER,
							GL_UNSIGNED_SHORT, data);
		}

		// fourth step: 1x1
		glGenTextures(3, m_idReductionTextures1x1);
		for(size_t i = 0; i < 3; ++i) {
			glBindTexture(GL_TEXTURE_2D, m_idReductionTextures1x1[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 8, 8);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 8, 8, GL_RED_INTEGER,
							GL_UNSIGNED_INT, data_uint);
		}


		// difference inspection texture
		glGenTextures(1, &m_idDifferenceTexture);

		glBindTexture(GL_TEXTURE_2D, m_idDifferenceTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R16UI, 320*8, 240*8);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320*8, 240*8, GL_RED_INTEGER,
						GL_UNSIGNED_SHORT, data);
		
		delete [] data;

		ValidateComputeShader(m_idReduction0DifferenceProgram);
		ValidateComputeShader(m_idReduction1DifferenceProgram);
		ValidateComputeShader(m_idReduction2DifferenceProgram);
		ValidateComputeShader(m_idReduction0UnionProgram);
		ValidateComputeShader(m_idReduction1UnionProgram);
		ValidateComputeShader(m_idReduction2UnionProgram);
		ValidateComputeShader(m_idReduction0IntersectionProgram);
		ValidateComputeShader(m_idReduction1IntersectionProgram);
		ValidateComputeShader(m_idReduction2IntersectionProgram);

		return true;
	}
	
	bool HandTracker::InitParticleSwarm() {
		m_pSwarm = new ParticleSwarm(64);
		SetToInitialPose(m_pSwarm->GetParticleBest());
		m_pSwarm->InitializeAroundBest(0);
		
		return true;
	}

	bool HandTracker::InitOutputModel() {
		m_pHandModelLeft = new HandModel();
		m_pHandModelRight = new HandModel();

		*m_pHandModelLeft  = *m_pSwarm->GetParticleBest().GetHandModelLeft();
		*m_pHandModelRight = *m_pSwarm->GetParticleBest().GetHandModelRight();

		return true;
	}

	bool HandTracker::InitEvaluation() {
		m_pFramePlayer->SetLoop(m_oConfig.bLoop);
		m_itCurPlayback = m_oConfig.vecPlaybackFiles.begin();

		PrepareEvaluationFiles();

		m_pFramePlayer->SetInputFile(*m_itCurPlayback);
		m_pFramePlayer->StartPlayback();

		return true;
	}

	void HandTracker::PrepareEvaluationFiles() {
		std::string sPlayback = *m_itCurPlayback;
		std::string sPlaybackBase =
 			sPlayback.substr(sPlayback.find_last_of('/')+1, std::string::npos);
		sPlaybackBase = sPlaybackBase.substr(0, sPlaybackBase.find_last_of('.'));

		std::string sEvalOutput = "resources/results/";
		sEvalOutput += sPlaybackBase + "-";
		sEvalOutput += m_oConfig.sCondition + "-";
		sEvalOutput += std::to_string(m_oConfig.iIterations);

		vstr::out() << "Recording score data to " << sEvalOutput
					<< sEvalOutputSuffix << std::endl;
		m_osEvalOutput.open(sEvalOutput+sEvalOutputSuffix,
							std::ios_base::out | std::ios_base::binary);

		std::ofstream osEvalConfig;
		osEvalConfig.open(sEvalOutput+".txt");
		PrintConfig(osEvalConfig);
		osEvalConfig.close();
	}

	void HandTracker::SetToInitialPose(Particle &oParticle) {
		oParticle = Particle();
		oParticle.GetHandModelLeft()->SetPosition(VistaVector3D(-0.14, -0.1, 0.5));
		oParticle.GetHandModelLeft()->SetJointAngle(HandModel::T_CMC_A, 10);
		oParticle.GetHandModelLeft()->SetJointAngle(HandModel::T_CMC_F, -40);
		oParticle.GetHandModelLeft()->SetJointAngle(HandModel::T_MCP, 20);
		oParticle.GetHandModelLeft()->SetJointAngle(HandModel::T_IP, 20);

		oParticle.GetHandModelRight()->SetPosition(VistaVector3D(0.14, -0.1, 0.5));
		oParticle.GetHandModelRight()->SetJointAngle(HandModel::T_CMC_A, 10);
		oParticle.GetHandModelRight()->SetJointAngle(HandModel::T_CMC_F, -40);
		oParticle.GetHandModelRight()->SetJointAngle(HandModel::T_MCP, 20);
		oParticle.GetHandModelRight()->SetJointAngle(HandModel::T_IP, 20);
	}
	
	bool HandTracker::FrameUpdate(const unsigned char  *colorFrame,
								  const unsigned short *depthFrame,
								  const float          *uvMapFrame) {

		m_pProfiler->NewFrame();

		const VistaTimer &oTimer = VistaTimeUtils::GetStandardTimer();
		VistaType::microtime tStart;
		VistaType::microtime tProcessFrames;
		VistaType::microtime tPSO;

		FrameRecordingAndPlayback(colorFrame,
								  depthFrame,
								  uvMapFrame);

		// ImagePBOOpenGLDraw *pPBODraw;
		// pPBODraw = m_mapPBO[COLOR];;
		// if(pPBODraw) {
		// 	pPBODraw->FillPBOFromBuffer(m_pColorBuffer, 320, 240);
		// }

		// pPBODraw = m_mapPBO[DEPTH];
		// if(pPBODraw) {
		//DepthToRGB(m_pDepthBuffer, m_pDepthRGBBuffer);
		// 	pPBODraw->FillPBOFromBuffer(m_pDepthRGBBuffer, 320, 240);
		// }

		tStart = oTimer.GetMicroTime();
		m_pFrameFilter->ProcessFrames(m_pColorBuffer,
									  m_pDepthBuffer,
									  m_pUVMapBuffer);
		tProcessFrames = oTimer.GetMicroTime() - tStart;

		// pPBODraw = m_mapPBO[UVMAP];
		// if(pPBODraw) {
		// 	pPBODraw->FillPBOFromBuffer(m_pUVMapRGBBuffer, 320, 240);
		// }
		
		// pPBODraw = m_mapPBO[COLOR_SEGMENTED];
		// if(pPBODraw) {
		// 	pPBODraw->FillPBOFromBuffer(m_pColorBuffer, 320, 240);
		// }

		// pPBODraw = m_mapPBO[DEPTH_SEGMENTED];
		// if(pPBODraw) {
		// 	pPBODraw->FillPBOFromBuffer(m_pDepthRGBBuffer, 320, 240);
		// }

		// pPBODraw = m_mapPBO[UVMAP_SEGMENTED];
		// if(pPBODraw) {
		// 	pPBODraw->FillPBOFromBuffer(m_pUVMapRGBBuffer, 320, 240);
		// }
		
		WriteDebug(IDebugView::CAMERAFRAMES_TIME,
				   IDebugView::FormatString("Camera processing time: ",
											tProcessFrames));

		ResourcesBind();

#ifndef PSO_TESTING		
		UploadCameraDepthMap();
#endif
		SetupProjection();

		if(m_bTrackingEnabled) {
			tStart = oTimer.GetMicroTime();		   
			PerformPSOTracking();
			tPSO = oTimer.GetMicroTime() - tStart;
			WriteDebug(IDebugView::PSO_TIME,
					   IDebugView::FormatString("PSO loop time: ",
												tPSO));
			WriteDebug(IDebugView::LOOP_FPS,
					   IDebugView::FormatString("Tracking loop  fps: ",
												1.0f/(tProcessFrames+tPSO)));
			WriteDebug(IDebugView::LOOP_TIME,
					   IDebugView::FormatString("Tracking loop time: ",
												tProcessFrames + tPSO));
		}
		else {
			PerformStartPoseMatch();
		}

		ResourcesUnbind();

		//m_pProfiler->PrintProfile(vstr::out());
		// vstr::out()
		// 	<< "Reduction time: "
		// 	<< m_pProfiler->GetRoot()->GetChild("Reduction")->GetLastFrameTime()
		// 	<< std::endl;			

		return true;
	}

	void HandTracker::FrameRecordingAndPlayback(
		const unsigned char  *colorFrame,
		const unsigned short *depthFrame,
		const float          *uvMapFrame) {

		if(m_bFrameRecording)
			m_pFrameRecorder->RecordFrames(colorFrame, depthFrame, uvMapFrame);

		if(m_bFramePlayback) {
			bool frameread = m_pFramePlayer->PlaybackFrames(m_pColorBuffer,
															m_pDepthBuffer,
															m_pUVMapBuffer);
			
			if(!frameread)
				return;
		}
		else {
			memcpy(m_pColorBuffer, colorFrame, 320*240*3);
			memcpy(m_pDepthBuffer, depthFrame, 320*240*2);
			memcpy(m_pUVMapBuffer, uvMapFrame, 320*240*4*2);
		}
	}

	void HandTracker::ResourcesBind() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_idRenderedTextureFBO);	

		// bind result image textures
		for(size_t i = 0; i < 3; ++i) {
			glBindImageTexture(0 + i,
							   m_idReductionTextures320x256[i],
							   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16UI);
		}
		for(size_t i = 0; i < 3; ++i) {
			glBindImageTexture(3 + i,
							   m_idReductionTextures40x32[i],
							   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16UI);
		}
		glBindImageTexture(6,
						   m_idReductionTextures5x4[0],
						   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);
		for(size_t i = 1; i < 3; ++i) {
			glBindImageTexture(6 + i,
							   m_idReductionTextures5x4[i],
							   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16UI);
		}
		for(size_t i = 0; i < 3; ++i) {
			glBindImageTexture(9 + i,
							   m_idReductionTextures1x1[i],
							   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);
		}
		glBindImageTexture(12, m_idDifferenceTexture,
						   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16UI);

		// bind input textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_idCameraTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_idRenderedTexture);

#ifndef PSO_TESTING		
		// bind pixel unpack PBO
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_idCameraTexturePBO);
#endif
		// bind transform SSBOs
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandModelsLocation,
						 m_idSSBOHandModels);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandGeometryLocation,
						 m_idSSBOHandGeometry);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOTransformsLocation,
						 m_pHandRenderer->GetSSBOTransformsId());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandModelsIBestLocation,
						 m_idSSBOHandModelsIBest);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandModelsGBestLocation,
						 m_idSSBOHandModelsGBest);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandModelsVelocityLocation,
						 m_idSSBOHandModelsVelocity);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBORandomLocation,
						 m_idSSBORandom);
	}

	void HandTracker::ResourcesUnbind() {
		// unbind transform SSBOs
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandModelsLocation, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandGeometryLocation, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOTransformsLocation, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandModelsIBestLocation, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandModelsGBestLocation, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBOHandModelsVelocityLocation, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,
						 iSSBORandomLocation, 0);

#ifndef PSO_TESTING		
		// unbind pixel unpack PBO
 		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif
		
		// unbind input textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		// unbind result image textures
		glBindImageTexture(12, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16UI);
		for(size_t i = 0; i < 12; ++i) {
			glBindImageTexture(i, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16UI);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void HandTracker::UploadCameraDepthMap() {
		// upload camera image to tiled texture
 		unsigned short *aCameraTexturePBO =
			(unsigned short*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER,
										 GL_WRITE_ONLY);		
		memcpy(aCameraTexturePBO, m_pDepthBuffer,
			   320*240*sizeof(unsigned short));
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		glActiveTexture(GL_TEXTURE0);
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, 
								320*col, 240*row, 320, 240,
								GL_DEPTH_COMPONENT,
								GL_SHORT, NULL);
			}
		}
	}

	void HandTracker::SetupProjection() {
		// set up camera projection from intrinsic parameters
		// we don't do non-linear radial distortion corretion for now.
		float cx = m_oCameraIntrinsics.GetValue<float>("CX");
		float cy = m_oCameraIntrinsics.GetValue<float>("CY");
		float fx = m_oCameraIntrinsics.GetValue<float>("FX");
		float fy = m_oCameraIntrinsics.GetValue<float>("FY");

		// we measure in m, focal length given in mm
		cx /= 1000.0f;
		cy /= 1000.0f;
		fx /= 1000.0f;
		fy /= 1000.0f;		

		// https://sightations.wordpress.com/2010/08/03/simulating-calibrated-cameras-in-opengl/
		float znear = 0.1f;
		float zfar  = 1.1f;
		float x = znear + zfar;
		float y = znear * zfar;

		VistaTransformMatrix mProj(
			fx, 0, -cx, 0,
			0, fy, -cy, 0,
			0,  0,   x, y,
			0,  0,  -1, 0 );
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		glOrtho(0.0, 0.32, 0.0, 0.24, znear, zfar);
		glMultMatrixf(mProj.GetData());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		VistaQuaternion qRotY =
			VistaQuaternion(
				VistaAxisAndAngle(
					VistaVector3D(0, 1, 0), Vista::Pi));

		VistaTransformMatrix mRotY(qRotY);
		glMultMatrixf(mRotY.GetData());

		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST);
	}
	
	void HandTracker::PerformStartPoseMatch() {
		std::vector<float> vViewportData;

		glClear(GL_DEPTH_BUFFER_BIT);

		UploadHandModels();
		
		m_pHandRenderer->DrawHand(
			m_pSwarm->GetParticleBest().GetHandModelLeft(),
			m_pHandGeometry);
		m_pHandRenderer->DrawHand(
			m_pSwarm->GetParticleBest().GetHandModelRight(),
			m_pHandGeometry);

		vViewportData.push_back(0);
		vViewportData.push_back(0);
		vViewportData.push_back(320);
		vViewportData.push_back(240);

		m_pHandRenderer->PreDraw();
		m_pHandRenderer->PerformDraw(true, 0, 1, &vViewportData[0]);
		m_pHandRenderer->PostDraw();

		ReduceDepthMaps();
		UpdateScores();

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsIBest);
		float *pModelsIBest = (float*)(glMapBuffer(GL_SHADER_STORAGE_BUFFER,
												   GL_READ_ONLY));
		float fPenalty = pModelsIBest[31];
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);		
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		float fRed = PenaltyNormalize(fPenalty);
		float fGreen = 1 - fRed;
		
		glUseProgram(m_idColorFragProgram);
		glUniform3f(m_locColorUniform, fRed, fGreen, 0.0f);

		if(m_oConfig.bAutoTracking && !GetIsTracking()) {
			if(fPenalty < m_oConfig.fPenaltyStart)
				StartTracking();
		}

		WriteDebug(IDebugView::PENALTY,
				   IDebugView::FormatString("Penalty: ", fPenalty));
	}

	void HandTracker::PerformPSOTracking() {
		const VistaTimer &oTimer = VistaTimeUtils::GetStandardTimer();
		VistaType::microtime tStart = 0.0;
		VistaType::microtime tTransform = 0.0;
		VistaType::microtime tRendering = 0.0;
		VistaType::microtime tReduction = 0.0;
		VistaType::microtime tSwarmUpdate = 0.0;

		UploadHandModels();
		
		float fPhiCognitive;
		float fPhiSocial;
		for(unsigned gen = 0 ; gen < m_oConfig.iPSOGenerations ; gen++) {
			tStart = oTimer.GetMicroTime();
			GenerateTransforms();
			tTransform += oTimer.GetMicroTime() - tStart;

			// FBO rendering of tiled zbuffers
			tStart = oTimer.GetMicroTime();
			glClear(GL_DEPTH_BUFFER_BIT);
			m_pHandRenderer->PreDraw();
			for(int row = 0 ; row < 8 ; row++) {
				for(int col = 0 ; col < 8 ; col++) {
					size_t index = row*8 + col;

					if( (index+1) % m_oConfig.iViewportBatch == 0 ) {
						m_pHandRenderer->PerformDraw(
							false,
							index/m_oConfig.iViewportBatch *
							m_oConfig.iViewportBatch,
							m_oConfig.iViewportBatch,
							&m_vViewportData[0]);
					}
				}
			}
			m_pHandRenderer->PostDraw();
			glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT);
			tRendering += oTimer.GetMicroTime() - tStart;
			
			tStart = oTimer.GetMicroTime();
			ReduceDepthMaps();
			tReduction += oTimer.GetMicroTime() - tStart;
			
			tStart = oTimer.GetMicroTime();
			UpdateScores();

			fPhiCognitive = m_oConfig.fPhiCognitiveBegin +
				float(gen)/float(m_oConfig.iPSOGenerations-1) *
				(m_oConfig.fPhiCognitiveEnd - m_oConfig.fPhiCognitiveBegin);
			fPhiSocial = 4.1f - fPhiCognitive;

			UpdateSwarm(fPhiCognitive, fPhiSocial);
			tSwarmUpdate += oTimer.GetMicroTime() - tStart;

			if(m_oConfig.bEvaluate)
				EvaluationStep();
		}

		DownloadHandModels();
		UpdateOutputModel();

		EvaluationPostFrame();

		m_pSwarm->InitializeAroundBest(m_oConfig.iKeepKBest);
		
		WriteDebug(IDebugView::TRANSFORM_TIME,
				   IDebugView::FormatString("Transform time: ",
											tTransform));
		WriteDebug(IDebugView::RENDER_TIME,
				   IDebugView::FormatString("Render time: ",
											tRendering));

		VistaType::microtime tReductionProfiler =
			m_pProfiler->GetRoot()->GetChild("Reduction")->GetLastFrameTime();
		WriteDebug(IDebugView::REDUCTION_TIME,
				   IDebugView::FormatString("Reduction time: ",
											tReductionProfiler));
					   
		WriteDebug(IDebugView::SWARMUPDATE_TIME,
				   IDebugView::FormatString("Swarm update time: ",
											tSwarmUpdate));

		WriteDebug(IDebugView::PENALTY,
				   IDebugView::FormatString(
					   "Penalty: ",
					   m_pSwarm->GetParticleBest().GetIBestPenalty()));
				
	}

	void HandTracker::UploadHandModels() {
		ParticleSwarm::ParticleVec &vecParticles = m_pSwarm->GetParticles();
		float *aBuffer;

		// upload HandModel
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModels);
		aBuffer = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				size_t index = row*8+col;
				Particle::ParticleToStateArray(&vecParticles[index],
											   aBuffer + 64*index);
			}
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// upload HandModelVelocity
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsVelocity);
		aBuffer = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				size_t index = row*8+col;
				memcpy(aBuffer + 64*index,
					   &vecParticles[index].GetVelocity()[0],
					   64*sizeof(float));
			}
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		
		// reset HandModelIBest penalty
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsIBest);
		aBuffer = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				size_t index = row*8+col;
				aBuffer[64*index + 31] = 1e20;
			}
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void HandTracker::DownloadHandModels() {
		ParticleSwarm::ParticleVec &vecParticles = m_pSwarm->GetParticles();
		float *aBuffer;

		// download HandModel
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModels);
		aBuffer = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				size_t index = row*8+col;
				Particle::StateArrayToParticle(&vecParticles[index],
											   aBuffer + 64*index);
			}
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// download HandModelVelocity
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsVelocity);
		aBuffer = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				size_t index = row*8+col;
				memcpy(&vecParticles[index].GetVelocity()[0],
					   aBuffer + 64*index, 64*sizeof(float));
			}
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		
		// download HandModelIBest
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsIBest);
		aBuffer = (float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				size_t index = row*8+col;
				HandModel::StateArrayToHandModel(
					vecParticles[index].GetIBestModelLeft(),
					aBuffer + 64*index);
				HandModel::StateArrayToHandModel(
					vecParticles[index].GetIBestModelRight(),
					aBuffer + 64*index + 32);

				vecParticles[index].SetIBestPenalty(aBuffer[64*index + 31]);
			}
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	
	void HandTracker::GenerateTransforms() {
		glUseProgram(m_idGenerateTransformsProgram);
   		glDispatchCompute(64, 2, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	
	void HandTracker::ReduceDepthMaps() {
		glUseProgram(m_idPrepareReductionTexturesProgram);
		glDispatchCompute(320, 128, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

#ifdef PSO_TESTING
		unsigned short *data;
		
		// TESTING: initialize textures with constant 1
		data = new unsigned short[320*8*256*8];
		for(size_t i = 0; i < 320*8*256*8; ++i) {
			data[i] = 1;
		}			
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_idReductionTextures320x256[0]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320*8, 256*8,
						GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);
#endif
		m_pProfiler->StartSection("Reduction");
		
		glUseProgram(m_idReduction1DifferenceProgram);
		glDispatchCompute(40, 8, 1);
		glUseProgram(m_idReduction1UnionProgram);
		glDispatchCompute(40, 8, 1);
		glUseProgram(m_idReduction1IntersectionProgram);
		glDispatchCompute(40, 8, 1);	
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
		glUseProgram(m_idReduction2DifferenceProgram);
		glDispatchCompute(8, 8, 1);
		glUseProgram(m_idReduction2UnionProgram);
		glDispatchCompute(8, 8, 1);
		glUseProgram(m_idReduction2IntersectionProgram);
		glDispatchCompute(8, 8, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
#ifdef PSO_TESTING
		glFinish();
#endif

		m_pProfiler->StopSection();				

#ifdef PSO_TESTING
		glBindTexture(GL_TEXTURE_2D, m_idReductionTextures320x256[0]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);
		vstr::err() << data[0] << std::endl;

		glBindTexture(GL_TEXTURE_2D, m_idReductionTextures40x32[0]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);
		vstr::err() << data[0] << std::endl;

		glBindTexture(GL_TEXTURE_2D, m_idReductionTextures5x4[0]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);
		vstr::err() << data[0] << std::endl;

		unsigned int *data_uint = new unsigned int[8*8];
		glBindTexture(GL_TEXTURE_2D, m_idReductionTextures1x1[0]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, data_uint);
		vstr::err() << data_uint[0] << std::endl;

		delete [] data;
		delete [] data_uint;
#endif
	}

	void HandTracker::UpdateSwarm(float fPhiCognitive, float fPhiSocial) {
		// evolve particle swarm
		glUseProgram(m_idUpdateSwarmProgram);

		// set uniform for random SSBO offset
		glUniform1ui(
			m_locRandomOffsetUniform,
			VistaRandomNumberGenerator::GetStandardRNG()->GenerateInt32());
		
		// set uniforms for cognitive/social behavior
		glUniform1f(m_locPhiCognitiveUniform, fPhiCognitive);
		glUniform1f(m_locPhiSocialUniform, fPhiSocial);

		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// // DEBUG: print velocities
		// glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsVelocity);
		// float *aVelocities = (float*)(glMapBuffer(GL_SHADER_STORAGE_BUFFER,
		// 										  GL_READ_ONLY));
		// for(int i = 0; i < 64*3; ++i) {
		// 	vstr::out() << "velocity " << i/64 << " " << i%64 << ": "
		// 				<< aVelocities[i] << std::endl;
		// }
		// glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// // DEBUG: print model state
		// glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModels);
		// float *aState = (float*)(glMapBuffer(GL_SHADER_STORAGE_BUFFER,
		// 										  GL_READ_ONLY));
		// for(int i = 0; i < 64*3; ++i) {
		// 	vstr::out() << "state " << i/64 << " " << i%64 << ": "
		// 				<< aState[i] << std::endl;
		// }
		// glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	float HandTracker::PenaltyNormalize(float fPenalty) {
		fPenalty -= m_oConfig.fPenaltyMin;
		fPenalty /= (m_oConfig.fPenaltyMax - m_oConfig.fPenaltyMin);

		return fPenalty;
	}
	
	void HandTracker::UpdateScores() {
		// update ibest scores via compute shader 8*8
		glUseProgram(m_idUpdateScoresProgram);
   		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// // DEBUG: print all particle scores
		// glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsIBest);
		// float *aStateModels = (float*)(glMapBuffer(GL_SHADER_STORAGE_BUFFER,
		// 										   GL_READ_ONLY));	
		// for(int i = 0; i < 64; ++i) {
		// 	vstr::out() << "ibest " << i << ": "
		// 				<< aStateModels[64*i+31] << std::endl;
		// }
		// glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// find gbest particle
		glUseProgram(m_idUpdateGBestProgram);
   		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// // DEBUG: print gbest particle score
		// glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsGBest);
		// float *aStateGBest = (float*)(glMapBuffer(GL_SHADER_STORAGE_BUFFER,
		// 										  GL_READ_ONLY));
		// float gbest = aStateGBest[31];
		// vstr::out() << "gbest: " << gbest << std::endl;
		// glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	void HandTracker::UpdateOutputModel() {
		// get best match from gbest buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModelsGBest);
		float *aStateGBest = (float*)(glMapBuffer(GL_SHADER_STORAGE_BUFFER,
												  GL_READ_ONLY));	

		Particle::StateArrayToParticle(
			&m_pSwarm->GetParticleBest(), aStateGBest);

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// do exponential smoothing on the output model
		SmoothInterpolateModel(
			m_oConfig.fSmoothingFactor,
			m_pSwarm->GetParticleBest().GetHandModelLeft(),
			m_pHandModelLeft);
		SmoothInterpolateModel(
			m_oConfig.fSmoothingFactor,
			m_pSwarm->GetParticleBest().GetHandModelRight(),
			m_pHandModelRight);
	}

	void HandTracker::SmoothInterpolateModel(
		float fSmoothingFactor,
		HandModel *pModelNew,
		HandModel *pModelAccumulated) {
		std::vector<float> &vecAnglesNew =
			pModelNew->GetJointAngles();
		std::vector<float> &vecAnglesAccumulated =
			pModelAccumulated->GetJointAngles();

		for(size_t i = 0; i < vecAnglesNew.size(); ++i) {
			vecAnglesAccumulated[i] =
				fSmoothingFactor*vecAnglesNew[i] +
				(1-fSmoothingFactor)*vecAnglesAccumulated[i];				
		}

		pModelAccumulated->SetPosition(
			fSmoothingFactor*pModelNew->GetPosition() +
			(1-fSmoothingFactor)*pModelAccumulated->GetPosition());

		pModelAccumulated->SetOrientation(
			pModelAccumulated->GetOrientation().Slerp(
				pModelNew->GetOrientation(), fSmoothingFactor));
	}

	void HandTracker::EvaluationStep() {
		// write out HandModel scores
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModels);
		float *aBuffer =
			(float*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				size_t index = row*8+col;
				m_osEvalOutput << aBuffer[64*index + 31];
			}
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void HandTracker::EvaluationPostFrame() {
		if(m_pFramePlayer->GetIsStopped()) {
			if(++m_iEvalIteration < m_oConfig.iIterations) {
				m_pFramePlayer->StartPlayback();
			}
			else {
				m_itCurPlayback++;
				if(m_itCurPlayback != m_oConfig.vecPlaybackFiles.end()) {
					PrepareEvaluationFiles();
					m_pFramePlayer->SetInputFile(*m_itCurPlayback);
					m_pFramePlayer->StartPlayback();
				}
				else {
					m_osEvalOutput.close();
					m_oConfig.bEvaluate = false;
				}
			}
		}
	}

	void HandTracker::NextSkinClassifier() {
		m_pFrameFilter->NextSkinClassifier();

		WriteDebug(IDebugView::SKIN_CLASSIFIER,
				   IDebugView::FormatString(
					   "Skin classifier: ",
					   m_pFrameFilter->GetSkinClassifier()->GetName()));
	}

	void HandTracker::PrevSkinClassifier() {
		m_pFrameFilter->PrevSkinClassifier();

		WriteDebug(IDebugView::SKIN_CLASSIFIER,
				   IDebugView::FormatString(
					   "Skin classifier: ",
					   m_pFrameFilter->GetSkinClassifier()->GetName()));
	}

	void HandTracker::StartTracking() {
		m_bTrackingEnabled = true;

		WriteDebug(IDebugView::TRACKING,
				   IDebugView::FormatString("Tracking: ",
											m_bTrackingEnabled));
	}
	
	void HandTracker::StopTracking() {
		m_bTrackingEnabled = false;

		SetToInitialPose(m_pSwarm->GetParticleBest());
		*m_pHandModelLeft  = *m_pSwarm->GetParticleBest().GetHandModelLeft();
		*m_pHandModelRight = *m_pSwarm->GetParticleBest().GetHandModelRight();

		WriteDebug(IDebugView::TRACKING,
				   IDebugView::FormatString("Tracking: ",
											m_bTrackingEnabled));
	}

	bool HandTracker::GetIsTracking() {
		return m_bTrackingEnabled;
	}

	void HandTracker::ToggleFrameRecording() {
		m_bFrameRecording = !m_bFrameRecording;

		if(m_bFrameRecording) {
			m_pFrameRecorder->StartRecording();
		}
		else {
			m_pFrameRecorder->StopRecording();
		}
		WriteDebug(IDebugView::FRAME_RECORDING,
				   IDebugView::FormatString("Frame Recording: ",
											m_bFrameRecording));
	}

	void HandTracker::ToggleFramePlayback() {
		m_bFramePlayback = !m_bFramePlayback;

		if(m_bFramePlayback) {
			m_pFramePlayer->StartPlayback();
		}
		else {
			m_pFramePlayer->StopPlayback();
		}

		WriteDebug(IDebugView::FRAME_PLAYBACK,
				   IDebugView::FormatString("Frame Playback: ",
											m_bFramePlayback));

	}

	void HandTracker::WriteDebug(IDebugView::Slot eSlot,
								 std::string sMessage) {
		if(m_pDebugView)
			m_pDebugView->Write(eSlot, sMessage);
	}
}
