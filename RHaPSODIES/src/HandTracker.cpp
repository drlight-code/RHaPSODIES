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
#include <limits>
#include <exception>
#include <algorithm>

#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <VistaTools/VistaBasicProfiler.h>
#include <VistaTools/VistaIniFileParser.h>
#include <VistaTools/VistaRandomNumberGenerator.h>

#include <VistaKernel/DisplayManager/VistaSimpleTextOverlay.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "RHaPSODIES.hpp"
#include "ShaderRegistry.hpp"

#include "SkinClassifiers/SkinClassifierLogOpponentYIQ.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter0.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter1.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter2.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter3.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter4.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter5.hpp"
#include "SkinClassifiers/SkinClassifierDhawale.hpp"

#include "HandModel.hpp"
#include "HandGeometry.hpp"
#include "HandRenderer.hpp"
#include "DebugView.hpp"

#include "PSO/Particle.hpp"
#include "PSO/ParticleSwarm.hpp"

#include "CameraFrameRecorder.hpp"
#include "CameraFramePlayer.hpp"

#include "HandTracker.hpp"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace {
	// float MapRangeExp(float value) {
	// 	// map range 0-1 exponentially
	// 	float base = 0.01;
	// 	float ret = (1 - pow(base, value))/(1-base);
		
	// 	// clamp
	// 	ret = ret < 0.0 ? 0.0 : ret;
	// 	ret = ret > 1.0 ? 1.0 : ret;

	// 	return ret;
	// }
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
		vstr::err() << "FrameBuffer status complete!" << std::endl;
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
		vstr::out() << "MAX_COMPUTE_SHARED_MEMORY_SIZE:     " << values[0] << std::endl;

		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, values);
		vstr::out() << "MAX_COMPUTE_WORK_GROUP_INVOCATIONS: " << values[0] << std::endl;

		for(size_t index = 0 ; index < 3 ; ++index)
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, index, values+index);
		vstr::out() << "GL_MAX_COMPUTE_WORK_GROUP_COUNT:    "
					<< "[" << values[0] << ", " << values[1] << ", " << values[2]
					<< "]" << std::endl;

		for(size_t index = 0 ; index < 3 ; ++index)
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, index, values+index);
		vstr::out() << "GL_MAX_COMPUTE_WORK_GROUP_SIZE:     "
					<< "[" << values[0] << ", " << values[1] << ", " << values[2]
					<< "]" << std::endl;

		glGetIntegerv(GL_MAX_VIEWPORTS, values);
		vstr::out() << "GL_MAX_VIEWPORTS:                   " << values[0] << std::endl;
	}

	template<typename T> std::string ProfilerString(
		std::string sPrefix, T value) {
		std::ostringstream ostr;

		ostr << std::setw(30) << sPrefix
			 << std::boolalpha << std::fixed << value;
		return ostr.str();
	}

	inline float WorldToScreenProjective(
		float zWorld, float zNear=0.1f, float zFar=1.1f) {
		return ((zNear + zFar - 2.0f*zNear*zFar/zWorld /
				 (zFar - zNear) + 1.0f) / 2.0f);
	}

	inline float WorldToScreenLinear(
		float zWorld, float zNear=0.1f, float zFar=1.1f) {
		return (zWorld - zNear) / (zFar - zNear);
	}
}

namespace rhapsodies {
	const std::string sDepthLimitName   = "DEPTH_LIMIT";
	const std::string sErosionSizeName  = "EROSION_SIZE";
	const std::string sDilationSizeName = "DILATION_SIZE";

	const std::string sPSOGenerationsName = "PSO_GENERATIONS";

	const std::string sRecordingName = "RECORDING";
	const std::string sLoopName      = "LOOP";

	const std::string sAutoTrackingName = "AUTO_TRACKING";

	const std::string sPenaltyMinName   = "PENALTY_MIN";
	const std::string sPenaltyMaxName   = "PENALTY_MAX";
	const std::string sPenaltyStartName = "PENALTY_START";

	const std::string sViewportBatchName = "VIEWPORT_BATCH";

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	HandTracker::HandTracker(ShaderRegistry *pReg) :
		m_bCameraUpdate(true),
		m_bShowImage(false),
		m_bShowSkinMap(false),
		m_pShaderReg(pReg),
		m_pHandGeometry(new HandGeometry),
		m_pHandRenderer(new HandRenderer(pReg)),
		m_vViewportData(4*64),
		m_pDebugView(NULL),
		m_bFrameRecording(false),
		m_pRecorder(new CameraFrameRecorder),
		m_bFramePlayback(false),
		m_pPlayer(new CameraFramePlayer),
		m_bTrackingEnabled(false),
		m_pParticleBest(NULL),
		m_pSwarm(NULL) {

		m_pColorBuffer     = new unsigned char[320*240*3];
		m_pDepthBuffer     = new unsigned short[320*240];
		m_pDepthBufferUInt = new unsigned int[320*240];
		m_pUVMapBuffer     = new float[320*240*2];

		m_idGenerateTransformsProgram = pReg->GetProgram("generate_transforms");

		m_idReductionXProgram = pReg->GetProgram("reduction_x");
		m_idReductionYProgram = pReg->GetProgram("reduction_y");

		m_idColorFragProgram = pReg->GetProgram("shaded_indexedtransform");
		m_locColorUniform = glGetUniformLocation(m_idColorFragProgram, "color_in");

		glUseProgram(m_idColorFragProgram);
		glUniform3f(m_locColorUniform, 1.0f, 0.0f, 0.0f);
	}

	HandTracker::~HandTracker() {
		delete m_pSwarm;
		delete m_pParticleBest;
		
		for(ListSkinCl::iterator it = m_lClassifiers.begin() ;
			it != m_lClassifiers.end() ; ++it) {
			delete *it;
		}

		delete [] m_pColorBuffer;
		delete [] m_pDepthBuffer;
		delete [] m_pDepthBufferUInt;
		delete [] m_pUVMapBuffer;
		
		delete m_pRecorder;
		
		delete m_pHandRenderer;
		delete m_pHandGeometry;
	}
	
	void HandTracker::SetViewPBODraw(ViewType type,
									 ImagePBOOpenGLDraw *pPBODraw) {
		m_mapPBO[type] = pPBODraw;
	}

	void HandTracker::SetDebugView(IDebugView *pDebugView) {
		m_pDebugView = pDebugView;
	}
	
	HandModel *HandTracker::GetHandModelLeft() {
		return &m_pParticleBest->GetHandModelLeft();
	}
	
	HandModel *HandTracker::GetHandModelRight() {
		return &m_pParticleBest->GetHandModelRight();
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

	GLuint HandTracker::GetResultTextureId() {
		return m_idResultTexture;
	}

	GLuint HandTracker::GetDifferenceTextureId() {
		return m_idDifferenceTexture;
	}

	GLuint HandTracker::GetUnionTextureId() {
		return m_idUnionTexture;
	}

	GLuint HandTracker::GetIntersectionTextureId() {
		return m_idIntersectionTexture;
	}

	void HandTracker::ReadConfig() {
		VistaIniFileParser oIniParser(true);
		oIniParser.ReadFile(RHaPSODIES::sRDIniFile);

		const VistaPropertyList oConfig = oIniParser.GetPropertyList();

		if(!oConfig.HasProperty(RHaPSODIES::sTrackerSectionName)) {
			throw std::runtime_error(
				std::string() + "Config section ["
				+ RHaPSODIES::sTrackerSectionName
				+ "] not found!");			
		}

		const VistaPropertyList oTrackerConfig =
			oConfig.GetSubListConstRef(RHaPSODIES::sTrackerSectionName);

		m_oConfig.iDepthLimit = oTrackerConfig.GetValueOrDefault(
			sDepthLimitName, 500);
		m_oConfig.iErosionSize = oTrackerConfig.GetValueOrDefault(
			sErosionSizeName, 3);
		m_oConfig.iDilationSize = oTrackerConfig.GetValueOrDefault(
			sDilationSizeName, 5);

		m_oConfig.iPSOGenerations = oTrackerConfig.GetValueOrDefault(
			sPSOGenerationsName, 45);

		m_oConfig.sRecordingFile = oTrackerConfig.GetValueOrDefault(
			sRecordingName, std::string(""));
		m_pPlayer->SetInputFile(m_oConfig.sRecordingFile);

		m_oConfig.bLoop = oTrackerConfig.GetValueOrDefault(
			sLoopName, false);
		m_pPlayer->SetLoop(m_oConfig.bLoop);

		m_oConfig.bAutoTracking = oTrackerConfig.GetValueOrDefault(
			sAutoTrackingName, false);

		const VistaPropertyList &oCameraConfig =
			oConfig.GetSubListConstRef(RHaPSODIES::sCameraSectionName);

		std::string sIntrinsicSection =
			oCameraConfig.GetValue<std::string>("INTRINSICS");
		m_oCameraIntrinsics = oConfig.GetSubListCopy(sIntrinsicSection);

		m_oConfig.fPenaltyMin = oTrackerConfig.GetValueOrDefault(
			sPenaltyMinName, 0.5f);
		m_oConfig.fPenaltyMax = oTrackerConfig.GetValueOrDefault(
			sPenaltyMaxName, 1.5f);
		m_oConfig.fPenaltyStart = oTrackerConfig.GetValueOrDefault(
			sPenaltyStartName, 0.6f);

		m_oConfig.iViewportBatch = oTrackerConfig.GetValueOrDefault(
			sViewportBatchName, 1);
	}

	void HandTracker::PrintConfig() {
		vstr::out() << "* HandTracker configuration" << std::endl;
		vstr::out() << "Depth Limit: " << m_oConfig.iDepthLimit
					<< std::endl;
		vstr::out() << "Erosion Size: " << m_oConfig.iErosionSize
					<< std::endl;
		vstr::out() << "Dilation Size: " << m_oConfig.iDilationSize
					<< std::endl;

		vstr::out() << "PSO Generations: " << m_oConfig.iPSOGenerations
					<< std::endl;

		vstr::out() << "Recording file: " << m_oConfig.sRecordingFile
					<< std::endl;
		vstr::out() << "Loop: " << std::boolalpha << m_oConfig.bLoop
					<< std::endl;
		vstr::out() << "Auto tracking: " << std::boolalpha
					<< m_oConfig.bAutoTracking << std::endl;

	}

	bool HandTracker::Initialize() {
		vstr::out() << "Initializing RHaPSODIES HandTracker" << std::endl;

		// parse config params into member variables
		ReadConfig();
		PrintConfig();
		
		InitSkinClassifiers();

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
		
		return true;
	}

	bool HandTracker::HasGLComputeCapabilities() {
		return GLEW_ARB_shader_image_load_store && GLEW_ARB_compute_shader;
	}

	bool HandTracker::InitRendering() {
		// prepare texture and PBO for camera depth map
		glGenTextures(1, &m_idCameraTexture);
		glBindTexture(GL_TEXTURE_2D, m_idCameraTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
					 320*8, 240*8, 0,
					 GL_DEPTH_COMPONENT, GL_INT, NULL);

		glGenBuffers(1, &m_idCameraTexturePBO);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_idCameraTexturePBO);
		glBufferData(GL_PIXEL_UNPACK_BUFFER,
					 320*240*4, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		// prepare FBO rendering
		glGenTextures(1, &m_idRenderedTexture);
		glBindTexture(GL_TEXTURE_2D, m_idRenderedTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
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
		glGenBuffers(1, &m_idSSBOHandModels);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModels);
		// 32 instead of 27 for padding
		glBufferData(GL_SHADER_STORAGE_BUFFER, 64*2*32*sizeof(float),
					 NULL, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &m_idSSBOHandGeometry);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandGeometry);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 19*sizeof(float),
					 &m_pHandGeometry->GetExtents()[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		return true;
	}

	bool HandTracker::InitReduction() {
		glActiveTexture(GL_TEXTURE0);

		// prepare result texture
		glGenTextures(1, &m_idResultTexture);

		unsigned int *data = new unsigned int[3*240*8*8];
		for(int i = 0; i < 3*240*8*8; ++i) {
			data[i] = 0x0;
		}		
		
		glBindTexture(GL_TEXTURE_2D, m_idResultTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 3*8, 8*240);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 3*8, 8*240, GL_RED_INTEGER,
						GL_UNSIGNED_INT, data);
		delete [] data;

		glGenTextures(1, &m_idFinalResultTexture);

		data = new unsigned int[3*8*8];
		for(int i = 0; i < 3*8*8; ++i) {
			data[i] = 0x0;
		}		
		
		glBindTexture(GL_TEXTURE_2D, m_idFinalResultTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 3*8, 8);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 3*8, 8, GL_RED_INTEGER,
						GL_UNSIGNED_INT, data);
		delete [] data;

		ValidateComputeShader(m_idReductionXProgram);
		ValidateComputeShader(m_idReductionYProgram);

		// difference inspection texture
		glGenTextures(1, &m_idDifferenceTexture);

		data = new unsigned int[320*240*8*8];
		for(int i = 0; i < 320*240*8*8; ++i) {
			data[i] = 0x0;
		}		
		
		glBindTexture(GL_TEXTURE_2D, m_idDifferenceTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 320*8, 240*8);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320*8, 240*8, GL_RED_INTEGER,
						GL_UNSIGNED_INT, data);
		delete [] data;

		// union inspection texture
		glGenTextures(1, &m_idUnionTexture);

		unsigned char *data_char = new unsigned char[320*240*8*8];
		for(int i = 0; i < 320*240*8*8; ++i) {
			data_char[i] = 0x0;
		}
		
		glBindTexture(GL_TEXTURE_2D, m_idUnionTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8UI, 320*8, 240*8);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320*8, 240*8, GL_RED_INTEGER,
						GL_UNSIGNED_BYTE, data_char);

		// intersection inspection texture
		glGenTextures(1, &m_idIntersectionTexture);

		glBindTexture(GL_TEXTURE_2D, m_idIntersectionTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8UI, 320*8, 240*8);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320*8, 240*8, GL_RED_INTEGER,
						GL_UNSIGNED_BYTE, data_char);
		delete [] data_char;

		// result pbo
		glGenBuffers(1, &m_idResultPBO);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, m_idResultPBO);
		glBufferData(GL_PIXEL_PACK_BUFFER,
					 8*8*3*4, 0, GL_DYNAMIC_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		
		return true;
	}
	

	bool HandTracker::InitSkinClassifiers() {
		SkinClassifierLogOpponentYIQ *pSkinLOYIQ =
			new SkinClassifierLogOpponentYIQ;
		m_lClassifiers.push_back(pSkinLOYIQ);

		SkinClassifier *pSkinCl = new SkinClassifierRedMatter0;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter1;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter2;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter3;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter4;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter5;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierDhawale;
		m_lClassifiers.push_back(pSkinCl);
		
		m_itCurrentClassifier = --m_lClassifiers.end();

		WriteDebug(IDebugView::SKIN_CLASSIFIER,
				   ProfilerString("Skin classifier: ",
								  (*m_itCurrentClassifier)->GetName()));

		return true;
	}

	bool HandTracker::InitParticleSwarm() {
		m_pParticleBest = new Particle;
		SetToInitialPose(*m_pParticleBest);
		
		m_pSwarm = new ParticleSwarm(64);
		m_pSwarm->InitializeAround(*m_pParticleBest);
		
		return true;
	}

	void HandTracker::SetToInitialPose(Particle &oParticle) {
		oParticle = Particle();
		oParticle.GetHandModelLeft().SetPosition(VistaVector3D(-0.14, -0.1, 0.5));
		oParticle.GetHandModelLeft().SetJointAngle(HandModel::T_CMC_A, 60);

		// oParticle.GetHandModelLeft().SetJointAngle(HandModel::T_CMC_F, 0);
		// oParticle.GetHandModelLeft().SetJointAngle(HandModel::T_CMC_A, 60);
		// oParticle.GetHandModelLeft().SetJointAngle(HandModel::T_MCP, 60);
		// oParticle.GetHandModelLeft().SetJointAngle(HandModel::T_IP, 60);

		oParticle.GetHandModelRight().SetPosition(VistaVector3D(0.14, -0.1, 0.5));
		oParticle.GetHandModelRight().SetJointAngle(HandModel::T_CMC_A, 60);

		// oParticle.GetHandModelLeft().SetJointAngle(HandModel::I_MCP_A, -20);
		// oParticle.GetHandModelLeft().SetJointAngle(HandModel::M_MCP_A,  20);
		// oParticle.GetHandModelRight().SetJointAngle(HandModel::I_MCP_A, -20);
		// oParticle.GetHandModelRight().SetJointAngle(HandModel::M_MCP_A,  20);
	}
	
	bool HandTracker::FrameUpdate(const unsigned char  *colorFrame,
								  const unsigned short *depthFrame,
								  const float          *uvMapFrame) {

		const VistaTimer &oTimer = VistaTimeUtils::GetStandardTimer();
		VistaType::microtime tStart;
		VistaType::microtime tProcessFrames;
		VistaType::microtime tPSO;
		
		tStart = oTimer.GetMicroTime();
		ProcessCameraFrames(colorFrame, depthFrame, uvMapFrame);
		tProcessFrames = oTimer.GetMicroTime() - tStart;

		WriteDebug(IDebugView::CAMERAFRAMES_TIME,
				   ProfilerString("Camera processing time: ",
								  tProcessFrames));

		UploadCameraDepthMap();
		SetupProjection();

		glBindFramebuffer(GL_FRAMEBUFFER, m_idRenderedTextureFBO);	

		if(m_bTrackingEnabled) {
			tStart = oTimer.GetMicroTime();		   
			PerformPSOTracking();
			tPSO = oTimer.GetMicroTime() - tStart;
			WriteDebug(IDebugView::PSO_TIME,
					   ProfilerString("PSO loop time: ",
									  tPSO));
			WriteDebug(IDebugView::LOOP_FPS,
					   ProfilerString("Tracking loop  fps: ",
									  1.0f/(tProcessFrames+tPSO)));
			WriteDebug(IDebugView::LOOP_TIME,
					   ProfilerString("Tracking loop time: ",
									  tProcessFrames + tPSO));
		}
		else {
			PerformStartPoseMatch();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}

	void HandTracker::UploadCameraDepthMap() {
 		// upload camera image to tiled texture
 		glBindTexture(GL_TEXTURE_2D, m_idCameraTexture);
 		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_idCameraTexturePBO);

 		m_pCameraTexturePBO = glMapBuffer(GL_PIXEL_UNPACK_BUFFER,
 										  GL_WRITE_ONLY);		
		memcpy(m_pCameraTexturePBO, m_pDepthBufferUInt, 320*240*4);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, 
								320*col, 240*row, 320, 240,
								GL_DEPTH_COMPONENT,
								GL_INT, NULL);
			}
		}
 		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
 		glBindTexture(GL_TEXTURE_2D, 0);
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
		
		m_pHandRenderer->DrawHand(
			&m_pParticleBest->GetHandModelLeft(),
			m_pHandGeometry);
		m_pHandRenderer->DrawHand(
			&m_pParticleBest->GetHandModelRight(),
			m_pHandGeometry);

		vViewportData.push_back(0);
		vViewportData.push_back(0);
		vViewportData.push_back(320);
		vViewportData.push_back(240);

		m_pHandRenderer->PerformDraw(true, 0, 1, &vViewportData[0]);

		ReduceDepthMaps();

		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_idFinalResultTexture);

		unsigned int result_data[8*8*3];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, result_data);

		float difference_result   = result_data[0] / float(0x7fff);
		float union_result        = result_data[1];
		float intersection_result = result_data[2];

		float fPenalty = Penalty(m_pParticleBest->GetHandModelLeft(),
								 m_pParticleBest->GetHandModelRight(),
								 difference_result,
								 union_result,
								 intersection_result);

		float fRed = PenaltyNormalize(fPenalty);
		float fGreen = 1 - fRed;
		
		glUseProgram(m_idColorFragProgram);
		glUniform3f(m_locColorUniform, fRed, fGreen, 0.0f);

		if(m_oConfig.bAutoTracking && !IsTracking()) {
			if(fPenalty < m_oConfig.fPenaltyStart)
				StartTracking();
		}

		WriteDebug(IDebugView::PENALTY,
				   ProfilerString("Penalty: ", fPenalty));
	}

	void HandTracker::PerformPSOTracking() {
		const VistaTimer &oTimer = VistaTimeUtils::GetStandardTimer();
		VistaType::microtime tStart = 0.0;
		VistaType::microtime tTransform = 0.0;
		VistaType::microtime tRendering = 0.0;
		VistaType::microtime tReduction = 0.0;
		VistaType::microtime tSwarmUpdate = 0.0;

		// as in the original paper, we initialize the swarm uniformly
		// around the best match from the previous frame.  we might
		// consider letting the particle swarm just do its work and
		// keep the positions and velocities in between frames.
		m_pSwarm->InitializeAround(*m_pParticleBest);
		m_pParticleBest->ResetPenalty();

		// upload hand models into SSBO
		UploadHandModels();

		for(unsigned gen = 0 ; gen < m_oConfig.iPSOGenerations ; gen++) {
			// generate transform buffer in parallel 8*8*2
			tStart = oTimer.GetMicroTime();
			GenerateTransforms();
			tTransform += oTimer.GetMicroTime() - tStart;

			// FBO rendering of tiled zbuffers
			tStart = oTimer.GetMicroTime();
			glClear(GL_DEPTH_BUFFER_BIT);
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
			glFinish();
			tRendering += oTimer.GetMicroTime() - tStart;
			
			tStart = oTimer.GetMicroTime();
			ReduceDepthMaps();
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			tReduction += oTimer.GetMicroTime() - tStart;
			
			tStart = oTimer.GetMicroTime();
//			GpuPSOStep();
			// UpdateScores();
			// m_pSwarm->Evolve();
			tSwarmUpdate += oTimer.GetMicroTime() - tStart;

			// keep the best match over all generations, not just the last one
			Particle oParticleGenerationBest = m_pSwarm->GetBestMatch();
			if(oParticleGenerationBest.GetIBestPenalty() <
			   m_pParticleBest->GetIBestPenalty()) {
				*m_pParticleBest = oParticleGenerationBest;
			}
		}

		WriteDebug(IDebugView::TRANSFORM_TIME,
				   ProfilerString("Transform time: ",
								  tTransform));
		WriteDebug(IDebugView::RENDER_TIME,
				   ProfilerString("Render time: ",
								  tRendering));
		WriteDebug(IDebugView::REDUCTION_TIME,
				   ProfilerString("Reduction time: ",
								  tReduction));
		WriteDebug(IDebugView::SWARMUPDATE_TIME,
				   ProfilerString("Swarm update time: ",
								  tSwarmUpdate));

		WriteDebug(IDebugView::PENALTY,
				   ProfilerString("Penalty: ",
								  m_pParticleBest->GetIBestPenalty()));
				
	}

	void HandTracker::UploadHandModels() {
		ParticleSwarm::ParticleVec &vecParticles = m_pSwarm->GetParticles();
		float aState[64];

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModels);
		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				size_t index = row*8+col;
				
				Particle::ParticleToStateArray(vecParticles[index], aState);

				// 64 instead of 54 for alignment
				glBufferSubData(GL_SHADER_STORAGE_BUFFER,
								index*64*4,	64*4,
								aState);
			}
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
	}

	void HandTracker::GenerateTransforms() {
		glUseProgram(m_idGenerateTransformsProgram);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandModels);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
						 m_idSSBOHandModels);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_idSSBOHandGeometry);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
						 m_idSSBOHandGeometry);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER,
					 m_pHandRenderer->GetSSBOSphereTransformsId());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
						 m_pHandRenderer->GetSSBOSphereTransformsId());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER,
					 m_pHandRenderer->GetSSBOCylinderTransformsId());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3,
						 m_pHandRenderer->GetSSBOCylinderTransformsId());

   		glDispatchCompute(64, 2, 1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	
	void HandTracker::ReduceDepthMaps() {
		if(!GLEW_ARB_shader_image_load_store || !GLEW_ARB_compute_shader) {
			return;
		}
		
		// bind input textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_idCameraTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_idRenderedTexture);

		// bind result image texture
		glBindImageTexture(0, m_idResultTexture,
						   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);
		glBindImageTexture(1, m_idDifferenceTexture,
						   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);
		glBindImageTexture(2, m_idUnionTexture,
						   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
		glBindImageTexture(3, m_idIntersectionTexture,
						   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);

		// reduction in x direction
		glUseProgram(m_idReductionXProgram);
		glDispatchCompute(8, 240*8/3, 1);

		// make sure all image stores are visible
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// unbind input textures
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindImageTexture(4, m_idFinalResultTexture,
						   0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);

		// reduction in y direction
		glUseProgram(m_idReductionYProgram);
		glDispatchCompute(8, 8, 1);

		// make sure all image stores are visible
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// unbind result image textures
		glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);
		glBindImageTexture(1, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);
		glBindImageTexture(2, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
		glBindImageTexture(3, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
		glBindImageTexture(4, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32UI);
	}

	float HandTracker::Penalty(HandModel& oModelLeft,
							   HandModel& oModelRight,
							   float fDiff,
							   float fUnion,
							   float fIntersection) {
		float fLambdaK = 2;
		
		float fPenalty =
			PenaltyFromReduction(fDiff, fUnion, fIntersection) +
			fLambdaK * (PenaltyPrior(oModelLeft) + PenaltyPrior(oModelRight));
		
		return fPenalty;
	}

	float HandTracker::PenaltyFromReduction(float fDiff,
											float fUnion,
											float fIntersection) {
		float fLambda = 25;
		float fDepthTerm = fDiff / (fUnion + 1e-6);
		float fSkinTerm = (1 - 2*fIntersection / (fIntersection + fUnion + 1e-6));
		float fPenalty = fLambda * fDepthTerm + fSkinTerm;

		WriteDebug(IDebugView::DEPTH_TERM,
				   ProfilerString("Depth term: ", fLambda*fDepthTerm));
		WriteDebug(IDebugView::SKIN_TERM,
				   ProfilerString("Skin term: ", fSkinTerm));

		return fPenalty;
	}

	float HandTracker::PenaltyPrior(HandModel& oModel) {
		float fPenaltySum = 0;

		for(size_t dof = 5; dof < 17; dof += 4) {
			fPenaltySum +=
				-std::min(oModel.GetJointAngle(dof) - oModel.GetJointAngle(dof+4), 0.0f);
		}	

		fPenaltySum = Vista::DegToRad(fPenaltySum);

		return fPenaltySum;
	}

	float HandTracker::PenaltyNormalize(float fPenalty) {
		fPenalty -= m_oConfig.fPenaltyMin;
		fPenalty /= (m_oConfig.fPenaltyMax - m_oConfig.fPenaltyMin);

		return fPenalty;
	}
	
	void HandTracker::UpdateScores() {
		ParticleSwarm::ParticleVec &vecParticles = m_pSwarm->GetParticles();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_idFinalResultTexture);

		//const VistaTimer &oTimer = VistaTimeUtils::GetStandardTimer();
		//VistaType::microtime tS = oTimer.GetMicroTime();

		// glGetTexImage took ~4.3ms per iteration
		// 3.73 PSO fps at 40 generations
		// 3.88 using PBO for pixel transfer :/

		glBindBuffer(GL_PIXEL_PACK_BUFFER, m_idResultPBO);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);

		unsigned int *result_data = (unsigned int*)(glMapBuffer(GL_PIXEL_PACK_BUFFER,
																GL_READ_ONLY));

		//vstr::out() << "getteximage: " << oTimer.GetMicroTime()-tS << std::endl;
		//tS = oTimer.GetMicroTime();
		
		for(int row = 0; row < 8; ++row) {
			for(int col = 0; col < 8; ++col) {
				size_t result_index = 3*8*row + 3*col;
				
				float difference_result   = result_data[result_index + 0] / float(0x7fff);
				float union_result        = result_data[result_index + 1];
				float intersection_result = result_data[result_index + 2];

				float fPenalty = Penalty(vecParticles[8*row + col].GetHandModelLeft(),
										 vecParticles[8*row + col].GetHandModelRight(),
										 difference_result,
										 union_result,
										 intersection_result);

				vecParticles[8*row + col].UpdateIBest(fPenalty);
			}
		}
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		//vstr::out() << "loop:        " << oTimer.GetMicroTime()-tS << std::endl;

	}

	void HandTracker::ProcessCameraFrames(
		const unsigned char  *colorFrame,
		const unsigned short *depthFrame,
		const float          *uvMapFrame) {

		if(m_bFrameRecording)
			m_pRecorder->RecordFrames(colorFrame, depthFrame, uvMapFrame);

		if(m_bFramePlayback) {
			bool frameread = m_pPlayer->PlaybackFrames(m_pColorBuffer,
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

		ImagePBOOpenGLDraw *pPBODraw;
		// pPBODraw = m_mapPBO[COLOR];;
		// if(pPBODraw) {
		// 	pPBODraw->FillPBOFromBuffer(m_pColorBuffer, 320, 240);
		// }

		// pPBODraw = m_mapPBO[DEPTH];
		// if(pPBODraw) {
		DepthToRGB(m_pDepthBuffer, m_pDepthRGBBuffer);
		// 	pPBODraw->FillPBOFromBuffer(m_pDepthRGBBuffer, 320, 240);
		// }

		pPBODraw = m_mapPBO[UVMAP];
		if(pPBODraw) {
			// UVMapToRGB(m_pUVMapBuffer, m_pDepthBuffer, m_pColorBuffer,
			// 		   m_pUVMapRGBBuffer);
			UVMapToRGB(m_pUVMapBuffer, m_pDepthBuffer, m_pColorBuffer,
					   m_pUVMapRGBBuffer);
			//pPBODraw->FillPBOFromBuffer(m_pUVMapRGBBuffer, 320, 240);
		}

		if(m_bShowImage) {
			vstr::debug() << "showing opencv image" << std::endl;
		
			cv::Mat image = cv::Mat(240, 320, CV_8UC3, m_pColorBuffer);
			cv::namedWindow( "window", CV_WINDOW_AUTOSIZE ); 
			cv::imshow("window", image);

			cv::waitKey(1);

			m_bShowImage = false;
		}
		
		FilterSkinAreas();
		
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
	}
	
	void HandTracker::FilterSkinAreas() {

		VistaBasicProfiler *pProf = VistaBasicProfiler::GetSingleton();

		pProf->StartSection("Skin classification");
		for(size_t pixel = 0 ; pixel < 76800 ; pixel++) {
			// don't filter the color map since it is considerably expensive
			// if( (*m_itCurrentClassifier)->IsSkinPixel(colorImage+3*pixel) ) {
			// 	// yay! skin!
			// }
			// else {
			// 	colorImage[3*pixel+0] = 0;
			// 	colorImage[3*pixel+1] = 0;
			// 	colorImage[3*pixel+2] = 0;
			// }
			
			if( (*m_itCurrentClassifier)->IsSkinPixel(m_pUVMapRGBBuffer+3*pixel) ) {
				// yay! skin!
				m_pSkinMap[pixel] = 255;
			}
			else {
			 	m_pSkinMap[pixel] = 0;
			}
		}
		pProf->StopSection();

		// dilate the skin map with opencv
		cv::Mat image = cv::Mat(240, 320, CV_8UC1, m_pSkinMap);
		cv::Mat image_processed;
		if(m_bShowSkinMap)
			cv::imshow("Skin Map", image);

		cv::Mat erode_element = getStructuringElement(
			cv::MORPH_ELLIPSE,
			cv::Size(m_oConfig.iErosionSize, m_oConfig.iErosionSize),
			cv::Point(m_oConfig.iErosionSize/2));
		cv::Mat dilate_element = getStructuringElement(
			cv::MORPH_ELLIPSE,
			cv::Size(m_oConfig.iDilationSize, m_oConfig.iDilationSize),
			cv::Point(m_oConfig.iDilationSize/2));

		pProf->StartSection("SkinMap dilate/erode");
		cv::erode(image, image_processed, erode_element);
		image = image_processed.clone();
		cv::dilate(image, image_processed, dilate_element);
		pProf->StopSection();

		if(m_bShowSkinMap) {
			cv::imshow("Skin Map Dilated", image_processed);
			cv::waitKey(1);
		}

		pProf->StartSection("Depth/UV filtering");
		unsigned int uiDepthValue = 0x7fffffffu;
		for(size_t pixel = 0 ; pixel < 76800 ; pixel++) {
			// if( image_processed.data[pixel] == 0 ||
			// 	m_pDepthBuffer[pixel] < 400 ||
			// 	m_pDepthBuffer[pixel] > 600) {
			if( image_processed.data[pixel] == 0) {
				m_pDepthRGBBuffer[3*pixel+0] = 0;
				m_pDepthRGBBuffer[3*pixel+1] = 0;
				m_pDepthRGBBuffer[3*pixel+2] = 0;
				
				m_pUVMapRGBBuffer[3*pixel+0] = 0;
				m_pUVMapRGBBuffer[3*pixel+1] = 0;
				m_pUVMapRGBBuffer[3*pixel+2] = 0;

				uiDepthValue = 0x7fffffffu;
			}
			else {
				// transform depth value range, in millimeters:
				// 100mm  -> 0
				// 1100mm -> ffffffff
				// @todo get rid of hard coding
				
				unsigned int zWorldMM = m_pDepthBuffer[pixel];
				float zScreen = 1.0f;

				// valid values [100,1100]
				if( zWorldMM >= 100 && zWorldMM <= 1100 ) {
//					zScreen = WorldToScreenProjective(float(zWorldMM)/1000.0f);
					zScreen = WorldToScreenLinear(float(zWorldMM)/1000.0f);
				}
				// we correct for a more or less static 10cm depth offset here
				// @todo get this right in accordance to cam specs!
				uiDepthValue = (zScreen+0.1) * 0x7fffffffu;
			}
			int targetRow = 240 - 1 - (pixel/320);
			int targetCol = pixel % 320;

			m_pDepthBufferUInt[320*targetRow + targetCol] = uiDepthValue;
		}
		pProf->StopSection();
	}

	SkinClassifier *HandTracker::GetSkinClassifier() {
		return *m_itCurrentClassifier;
	}
	
	void HandTracker::NextSkinClassifier() {
		m_itCurrentClassifier++;
		if(m_itCurrentClassifier == m_lClassifiers.end())
			m_itCurrentClassifier = m_lClassifiers.begin();

		WriteDebug(IDebugView::SKIN_CLASSIFIER,
				   ProfilerString("Skin classifier: ",
								  (*m_itCurrentClassifier)->GetName()));
	}

	void HandTracker::PrevSkinClassifier() {
		if(m_itCurrentClassifier == m_lClassifiers.begin())
			m_itCurrentClassifier = m_lClassifiers.end();
		m_itCurrentClassifier--;

		WriteDebug(IDebugView::SKIN_CLASSIFIER,
				   ProfilerString("Skin classifier: ",
								  (*m_itCurrentClassifier)->GetName()));
	}

	void HandTracker::ShowOpenCVImg() {
		m_bShowImage = true;
	}
	void HandTracker::ToggleSkinMap() {
		vstr::debug() << "Toggling skin map rendering" << std::endl;
		
		m_bShowSkinMap = !m_bShowSkinMap;

		if(m_bShowSkinMap) {
			cv::namedWindow( "Skin Map", CV_WINDOW_AUTOSIZE );
			cv::namedWindow( "Skin Map Dilated", CV_WINDOW_AUTOSIZE );
		}
		else {
			cv::destroyWindow("Skin Map");
			cv::destroyWindow("Skin Map Dilated");
			cv::waitKey(1);

			cv::Mat image = cv::Mat(240, 320, CV_8UC1, m_pSkinMap);
			cv::imshow("Skin Map", image);
			cv::imshow("Skin Map Dilated", image);
		}
	}
	
	void HandTracker::DepthToRGB(const unsigned short *depth,
								 unsigned char *rgb) {
		for(int i = 0 ; i < 76800 ; i++) {
			unsigned short val = depth[i];

			if(val > 0) {
				float linearvalue = val/2000.0;
//				float mappedvalue = MapRangeExp(linearvalue);
				float mappedvalue = linearvalue;

				rgb[3*i+0] = 255*(1-mappedvalue);
				rgb[3*i+1] = 255*(1-mappedvalue);
				rgb[3*i+2] = 0;
			}
			else {
				rgb[3*i+0] = 0;
				rgb[3*i+1] = 0;
				rgb[3*i+2] = 0;
			}
		}
	}

	void HandTracker::UVMapToRGB(const float *uvmap,
								 const unsigned short *depth,
								 const unsigned char *color,
								 unsigned char *rgb) {

		int color_index_x, color_index_y, color_index;
		float invalid = -std::numeric_limits<float>::max();

		for(int i = 0 ; i < 76800 ; i++) {
			color_index_x = 320*uvmap[2*i+0];
			color_index_y = 240*uvmap[2*i+1];
			color_index = 320*color_index_y + color_index_x;

			if(uvmap[2*i+0] != invalid &&
			   uvmap[2*i+1] != invalid &&
			   depth[i] < m_oConfig.iDepthLimit) {

				rgb[3*i+0] = color[3*color_index+0];
				rgb[3*i+1] = color[3*color_index+1];
				rgb[3*i+2] = color[3*color_index+2];
			}
			else {
				rgb[3*i+0] = 200;
				rgb[3*i+1] = 0;
				rgb[3*i+2] = 200;
			}
 		}
	}

	void HandTracker::StartTracking() {
		m_bTrackingEnabled = true;

		WriteDebug(IDebugView::TRACKING,
				   ProfilerString("Tracking: ",
								  m_bTrackingEnabled));
	}
	
	void HandTracker::StopTracking() {
		m_bTrackingEnabled = false;

		SetToInitialPose(*m_pParticleBest);

		WriteDebug(IDebugView::TRACKING,
				   ProfilerString("Tracking: ",
								  m_bTrackingEnabled));
	}

	bool HandTracker::IsTracking() {
		return m_bTrackingEnabled;
	}

	void HandTracker::ToggleFrameRecording() {
		m_bFrameRecording = !m_bFrameRecording;

		if(m_bFrameRecording) {
			m_pRecorder->StartRecording();
		}
		else {
			m_pRecorder->StopRecording();
		}
		WriteDebug(IDebugView::FRAME_RECORDING,
				   ProfilerString("Frame Recording: ",
								  m_bFrameRecording));
	}

	void HandTracker::ToggleFramePlayback() {
		m_bFramePlayback = !m_bFramePlayback;

		if(m_bFramePlayback) {
			m_pPlayer->StartPlayback();
		}
		else {
			m_pPlayer->StopPlayback();
		}

		WriteDebug(IDebugView::FRAME_PLAYBACK,
				   ProfilerString("Frame Playback: ",
								  m_bFramePlayback));

	}

	void HandTracker::WriteDebug(IDebugView::Slot eSlot,
								 std::string sMessage) {
		if(m_pDebugView)
			m_pDebugView->Write(eSlot, sMessage);
	}
}
