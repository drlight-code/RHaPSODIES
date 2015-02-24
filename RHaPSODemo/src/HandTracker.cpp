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

#include <GL/glew.h>

#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimeUtils.h>
#include <VistaBase/VistaTimer.h>

#include <VistaTools/VistaBasicProfiler.h>
#include <VistaTools/VistaIniFileParser.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <RHaPSODemo.hpp>
#include <GLDraw/ImagePBOOpenGLDraw.hpp>

#include <SkinClassifiers/SkinClassifierLogOpponentYIQ.hpp>
#include <SkinClassifiers/SkinClassifierRedMatter0.hpp>
#include <SkinClassifiers/SkinClassifierRedMatter1.hpp>
#include <SkinClassifiers/SkinClassifierRedMatter2.hpp>
#include <SkinClassifiers/SkinClassifierRedMatter3.hpp>
#include <SkinClassifiers/SkinClassifierRedMatter4.hpp>
#include <SkinClassifiers/SkinClassifierRedMatter5.hpp>

#include <HandModel.hpp>
#include <HandModelRep.hpp>

#include <HandRenderer.hpp>

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

	
}

namespace rhapsodies {
	const std::string sDepthLimitName   = "DEPTH_LIMIT";
	const std::string sErosionSizeName  = "EROSION_SIZE";
	const std::string sDilationSizeName = "DILATION_SIZE";

	const std::string sPSOGenerationsName = "PSO_GENERATIONS";

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	HandTracker::HandTracker() :
		m_bCameraUpdate(true),
		m_bShowImage(false),
		m_bShowSkinMap(false),
		m_pHandModelLeft(NULL),
		m_pHandModelRight(NULL),
		m_pHandRenderer(NULL) {
	}

	HandTracker::~HandTracker() {
		for(ListSkinCl::iterator it = m_lClassifiers.begin() ;
			it != m_lClassifiers.end() ; ++it) {
			delete *it;
		}

		delete m_pHandModelLeft;
		delete m_pHandModelRight;

		delete m_pHandModelRep;
	}
	
	void HandTracker::SetViewPBODraw(ViewType type,
									 ImagePBOOpenGLDraw *pPBODraw) {
		m_mapPBO[type] = pPBODraw;
	}

	HandModel *HandTracker::GetHandModelLeft() {
		return m_pHandModelLeft;
	}
	
	HandModel *HandTracker::GetHandModelRight() {
		return m_pHandModelRight;
	}

	HandModelRep *HandTracker::GetHandModelRep() {
		return m_pHandModelRep;
	}

	void HandTracker::SetHandRenderer(HandRenderer *pRenderer) {
		m_pHandRenderer = pRenderer;
	}

	GLuint HandTracker::GetDepthTextureId() {
		return m_idDepthTexture;
	}

	GLuint HandTracker::GetCameraTextureId() {
		return m_idCameraTexture;
	}

	bool HandTracker::Initialize() {
		vstr::out() << "Initializing RHaPSODIES HandTracker" << std::endl;

		// parse config params into member variables
		ReadConfig();
		PrintConfig();
		
		// create the different skin classifiers
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

		m_itCurrentClassifier = m_lClassifiers.begin();
		m_itCurrentClassifier++;
		m_itCurrentClassifier++;

		// create hand models
		m_pHandModelLeft  = new HandModel;
		m_pHandModelLeft->SetType(HandModel::LEFT_HAND);
		m_pHandModelLeft->SetPosition(VistaVector3D(-0.1, 0, -0.3));

		m_pHandModelRight = new HandModel;
		m_pHandModelRight->SetType(HandModel::RIGHT_HAND);
		m_pHandModelRight->SetPosition(VistaVector3D(0.1, 0, -0.3));

		m_pHandModelRep = new HandModelRep;

		RandomizeModels();

		// prepare texture and PBO for camera depth map
		glGenTextures(1, &m_idCameraTexture);
		glBindTexture(GL_TEXTURE_2D, m_idCameraTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
					 320*8, 240*8, 0,
					 GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);

		glGenBuffers(1, &m_idCameraTexturePBO);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_idCameraTexturePBO);
		glBufferData(GL_PIXEL_UNPACK_BUFFER,
					 320*240*2, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		// prepare FBO rendering
		glGenTextures(1, &m_idDepthTexture);
		glBindTexture(GL_TEXTURE_2D, m_idDepthTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,
					 320*8, 240*8, 0,
					 GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);

		glGenFramebuffers(1, &m_idDepthTextureFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_idDepthTextureFBO);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							   GL_TEXTURE_2D, m_idDepthTexture, 0);

		CheckFrameBufferStatus(m_idDepthTextureFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		return true;
	}

	void HandTracker::ReadConfig() {
		VistaIniFileParser oIniParser(true);
		oIniParser.ReadFile(RHaPSODemo::sRDIniFile);

		const VistaPropertyList oConfig = oIniParser.GetPropertyList();

		if(!oConfig.HasProperty(RHaPSODemo::sTrackerSectionName)) {
			throw std::runtime_error(
				std::string() + "Config section ["
				+ RHaPSODemo::sTrackerSectionName
				+ "] not found!");			   
		}

		const VistaPropertyList oTrackerConfig =
			oConfig.GetSubListConstRef(RHaPSODemo::sTrackerSectionName);

		m_oConfig.iDepthLimit = oTrackerConfig.GetValueOrDefault(
			sDepthLimitName, 500);
		m_oConfig.iErosionSize = oTrackerConfig.GetValueOrDefault(
			sErosionSizeName, 3);
		m_oConfig.iDilationSize = oTrackerConfig.GetValueOrDefault(
			sDilationSizeName, 5);

		m_oConfig.iPSOGenerations = oTrackerConfig.GetValueOrDefault(
			sPSOGenerationsName, 45);
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

	}

	void HandTracker::ProcessCameraFrames(
		const unsigned char  *colorFrame,
		const unsigned short *depthFrame,
		const float          *uvMapFrame) {

		memcpy(m_pColorBuffer, colorFrame, 320*240*3);
		memcpy(m_pDepthBuffer, depthFrame, 320*240*2);

		ImagePBOOpenGLDraw *pPBODraw;
		// pPBODraw = m_mapPBO[COLOR];
		// if(pPBODraw) {
		// 	pPBODraw->FillPBOFromBuffer(m_pColorBuffer, 320, 240);
		// }

		// pPBODraw = m_mapPBO[DEPTH];
		// if(pPBODraw) {
		DepthToRGB(m_pDepthBuffer, m_pDepthRGBBuffer);
		// 	pPBODraw->FillPBOFromBuffer(m_pDepthRGBBuffer, 320, 240);
		// }

		// pPBODraw = m_mapPBO[UVMAP];
		// if(pPBODraw) {
		UVMapToRGB(uvMapFrame, depthFrame, colorFrame, m_pUVMapRGBBuffer);
		// 	pPBODraw->FillPBOFromBuffer(m_pUVMapRGBBuffer, 320, 240);
		// }

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

		pPBODraw = m_mapPBO[DEPTH_SEGMENTED];
		if(pPBODraw) {
			pPBODraw->FillPBOFromBuffer(m_pDepthRGBBuffer, 320, 240);
		}

		// pPBODraw = m_mapPBO[UVMAP_SEGMENTED];
		// if(pPBODraw) {
		// 	pPBODraw->FillPBOFromBuffer(m_pUVMapRGBBuffer, 320, 240);
		// }
	}

	void HandTracker::PerformPSOTracking() {
 		// upload camera image to tiled texture
 		glBindTexture(GL_TEXTURE_2D, m_idCameraTexture);
 		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_idCameraTexturePBO);

 		m_pCameraTexturePBO = glMapBuffer(GL_PIXEL_UNPACK_BUFFER,
 										  GL_WRITE_ONLY);		
		memcpy(m_pCameraTexturePBO, m_pDepthBuffer, 320*240*2);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		for(int row = 0 ; row < 8 ; row++) {
			for(int col = 0 ; col < 8 ; col++) {
				glTexSubImage2D(GL_TEXTURE_2D, 0, 
								320*col, 240*row, 320, 240,
								GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
			}
		}

 		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
 		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_idDepthTextureFBO);
		for(unsigned gen = 0 ; gen < m_oConfig.iPSOGenerations ; gen++) {
			// PSO for model hypotheses
		
			
			// FBO rendering of tiled zbuffers
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-0.3, 0.3, -0.3, 0.3, 0.0, 32.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);


			for(int row = 0 ; row < 8 ; row++) {
				for(int col = 0 ; col < 8 ; col++) {
					glViewport(col*320, row*240, 320, 240);

					RandomizeModels();
					m_pHandRenderer->DrawHand(m_pHandModelLeft,  m_pHandModelRep);
					m_pHandRenderer->DrawHand(m_pHandModelRight, m_pHandModelRep);
				}
			}
			// reduction with compute shader or opencl
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// update actual model fit		
	}
	
	bool HandTracker::FrameUpdate(const unsigned char  *colorFrame,
								  const unsigned short *depthFrame,
								  const float          *uvMapFrame) {

		const VistaTimer &oTimer = VistaTimeUtils::GetStandardTimer();

		VistaType::microtime tStart = oTimer.GetMicroTime();
		ProcessCameraFrames(colorFrame, depthFrame, uvMapFrame);
		VistaType::microtime tProcessFrames =
			oTimer.GetMicroTime() - tStart;
		tStart = oTimer.GetMicroTime();		   
		PerformPSOTracking();
		VistaType::microtime tPSO =
			oTimer.GetMicroTime() - tStart;
		vstr::out() << "Overall time: " << tProcessFrames + tPSO << std::endl;
		vstr::out() << "ProcessCameraFrames: " << tProcessFrames << std::endl;
		vstr::out() << "PerformPSOTracking: " << tPSO
					<< std::endl << std::endl;
		
		return true;
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
		for(size_t pixel = 0 ; pixel < 76800 ; pixel++) {
			if( image_processed.data[pixel] == 0 ) {
				m_pDepthRGBBuffer[3*pixel+0] = 0;
				m_pDepthRGBBuffer[3*pixel+1] = 0;
				m_pDepthRGBBuffer[3*pixel+2] = 0;
				m_pDepthBuffer[pixel] = 0xffff;

				m_pUVMapRGBBuffer[3*pixel+0] = 0;
				m_pUVMapRGBBuffer[3*pixel+1] = 0;
				m_pUVMapRGBBuffer[3*pixel+2] = 0;
			}
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

		vstr::debug() << "Selected skin classifier: "
					  << (*m_itCurrentClassifier)->GetName()
					  << std::endl;
	}

	void HandTracker::PrevSkinClassifier() {
		if(m_itCurrentClassifier == m_lClassifiers.begin())
			m_itCurrentClassifier = m_lClassifiers.end();
		m_itCurrentClassifier--;
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
	
	void HandTracker::RandomizeModels() {
		m_pHandModelRight->Randomize();
		m_pHandModelLeft->Randomize();

		// for comparison, choose same configuration for left hand
		// for(size_t index = 0 ; index < HandModel::JOINTDOF_LAST ; index++) {
		// 	m_pHandModelLeft->SetJointAngle(
		// 		index,
		// 		m_pHandModelRight->GetJointAngle(index));
		// }

		// m_pHandModelLeft->SetOrientation(
		// 	m_pHandModelRight->GetOrientation());
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
}
