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

#include <GL/glew.h>

#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <VistaTools/VistaProfiler.h>

#include <VistaDeviceDriversBase/VistaDriverMap.h>
#include <VistaDeviceDriversBase/VistaDeviceSensor.h>

#include <VistaDeviceDrivers/VistaDepthSenseDriver/VistaDepthSenseDriver.h>

#include <VistaKernel/VistaSystem.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>
#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VflTextRendering/VtrFontManager.h>
#include <VistaOGLExt/VistaShaderRegistry.h>

#include <Vfl2DDiagrams/V2dGlobalConfig.h>
#include <Vfl2DDiagrams/Diagrams/V2dDiagramDefault.h>
#include <Vfl2DDiagrams/V2dDiagramTextureVista.h>

#include <ImageDraw.hpp>
#include <ImagePBOOpenGLDraw.hpp>
#include <ColorFrameHandler.hpp>
#include <DepthFrameHandler.hpp>
#include <ShaderRegistry.hpp>
#include <HandTracker.hpp>
#include <HistogramUpdater.hpp>

#include "RHaPSODemo.hpp"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace rhapsodies {
	const std::string RHaPSODemo::sRDIniFile = "configfiles/rhapsodemo.ini";
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

namespace rhapsodies {
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
	RHaPSODemo::RHaPSODemo() :
		m_pSystem(new VistaSystem),
		m_pShaderReg(new ShaderRegistry),
		m_pTracker(new HandTracker),
		m_pDrawMutex(new VistaMutex),
		m_camWidth(320), m_camHeight(240),
		m_iDepthMeasures(0), m_iColorMeasures(0)
	{
	}

	RHaPSODemo::~RHaPSODemo() {
		delete m_pColorFrameHandler;
		delete m_pDepthFrameHandler;

		delete m_pColorDraw;
		delete m_pDepthDraw;
		if(m_pDiagramDraw)
			delete m_pDiagramDraw;

		delete m_pTracker;
		delete m_pShaderReg;
		delete m_pSystem;
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	bool RHaPSODemo::Initialize(int argc, char** argv) {
		bool success = true;

		success &= ParseConfig();

		success &= m_pSystem->Init(argc, argv);
		glewInit();

		// static_cast for now. for dynamic_cast we need to link
		// against the VistaDepthSenseDriver for the vtable (and thus
		// typeinfo struct) to be available at link time.
		m_pDriver = static_cast<VistaDepthSenseDriver*>(
			m_pSystem->GetDriverMap()->GetDeviceDriver("DEPTHSENSE"));

		if(m_pDriver) {
			std::cout << "DEPTHSENSE driver found" << std::endl;
			std::cout << "Sensors: " << std::endl;
			for(int i = 0 ; i < m_pDriver->GetNumberOfSensors() ; i++) {
				std::cout << m_pDriver->GetSensorByIndex(i)
					->GetSensorName() << std::endl;
			}
			std::cout << std::endl;
		}
		else {
			std::cerr << "DEPTHSENSE driver NOT found!" << std::endl;
		}

		m_pDepthSensor = m_pDriver->GetSensorByName("DEPTH");
		m_pColorSensor = m_pDriver->GetSensorByName("RGB");

		VtrFontManager::GetInstance()
			->SetFontDirectory("resources/fonts/");
		VistaShaderRegistry::GetInstance()
			.AddSearchDirectory("resources/shaders/");
		V2dGlobalConfig::GetInstance()
			->SetDefaultFont("FreeSans.ttf");

   		success &= RegisterShaders();
		success &= InitTracker();
		success &= CreateScene();

		return success;
	}

	bool RHaPSODemo::ParseConfig() {
		VistaProfiler oProf;

		// read the ini file names from dispatch ini
		m_pSystem->SetIniFile(
			oProf.GetTheProfileString("APPLICATION", "MAININI",
									  "vista.ini",
									  sRDIniFile) );
		m_pSystem->SetDisplayIniFile(
			oProf.GetTheProfileString("APPLICATION", "DISPLAYINI",
									  "vista.ini",
									  sRDIniFile) );
		m_pSystem->SetClusterIniFile(
			oProf.GetTheProfileString("APPLICATION", "CLUSTERINI",
									  "vista.ini",
									  sRDIniFile) );
		m_pSystem->SetInteractionIniFile(
			oProf.GetTheProfileString("APPLICATION", "INTERACTIONINI",
									  "vista.ini",
									  sRDIniFile) );

		// read camera parameters
		m_camWidth  = oProf.GetTheProfileInt("CAMERAS", "RESOLUTION_X",
											 320, RHaPSODemo::sRDIniFile);
		m_camHeight = oProf.GetTheProfileInt("CAMERAS", "RESOLUTION_Y",
											 240, RHaPSODemo::sRDIniFile);

		return true;
	}

	bool RHaPSODemo::InitTracker() {
		bool success = true;
		
		VistaDepthSenseDriver* pDriver = static_cast<VistaDepthSenseDriver*>(
			m_pSystem->GetInteractionManager()->GetDeviceDriver("DEPTHSENSE"));
		m_pTracker->SetDriver(pDriver);
		success &= m_pTracker->Initialize();

		// register frame update handler
		m_pSystem->GetEventManager()->AddEventHandler(
			this, VistaSystemEvent::GetTypeId(), 
			VistaSystemEvent::VSE_POSTAPPLICATIONLOOP);

		return success;
	}

	bool RHaPSODemo::RegisterShaders() {
		m_pShaderReg->RegisterShader("vert_textured", GL_VERTEX_SHADER,   
									 "resources/shaders/textured.vs");
		m_pShaderReg->RegisterShader("frag_textured", GL_FRAGMENT_SHADER,
									 "resources/shaders/textured.fs");

		std::vector<std::string> vec_shaders;
		vec_shaders.push_back("vert_textured");		
		vec_shaders.push_back("frag_textured");		
		m_pShaderReg->RegisterProgram("textured", vec_shaders);

		return true;
	}

	bool RHaPSODemo::CreateScene() {
		VistaSceneGraph *pSG = m_pSystem->GetGraphicsManager()->GetSceneGraph();

		// create global scene transform
		m_pSceneTransform = pSG->NewTransformNode(pSG->GetRoot());
		m_pSceneTransform->Translate(0, 0, -2.2);

		// ImageDraw for color image
		ImagePBOOpenGLDraw *pPBODraw = 
			new ImagePBOOpenGLDraw(m_camWidth, m_camHeight,
								   m_pShaderReg, m_pDrawMutex);
		m_pColorDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pColorFrameHandler = new 
			ColorFrameHandler(pPBODraw);
		m_pColorDraw->GetTransformNode()->SetTranslation(VistaVector3D(-2,0,0));

		// ImageDraw for depth image
		pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight,
										  m_pShaderReg, m_pDrawMutex);
		m_pDepthDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pDepthFrameHandler = new DepthFrameHandler(pPBODraw);
		m_pDepthDraw->GetTransformNode()->SetTranslation(VistaVector3D(0,0,0));

		// ImageDraw for histogram
		// m_pDiagramDraw = new ImageDraw(m_pSceneTransform,
		// 							   m_pDepthFrameHandler->GetDiagramTexture(),
		// 							   pSG);
		// m_pDiagramDraw->GetTransformNode()->SetTranslation(VistaVector3D(2,0,0));


		m_pSystem->GetEventManager()->RegisterObserver(
		  	m_pDepthFrameHandler->GetHistogramUpdater(),
		 	VistaSystemEvent::GetTypeId());

		m_pColorFrameHandler->Enable(true);
		m_pDepthFrameHandler->Enable(true);

		return true;
	}

	bool RHaPSODemo::Run() {
		return m_pSystem->Run();
	}

	void RHaPSODemo::FrameLoop() {
		// poll sensors for new data
		int iNewCount = m_pDepthSensor->GetDataCount();
		if(iNewCount > m_iDepthMeasures) {
			m_iDepthMeasures = iNewCount;
			// new depth data
			const VistaMeasureHistory &oHistory =
				m_pDepthSensor->GetMeasures();
			const VistaSensorMeasure *pM = 
				oHistory.GetCurrentRead();
				
			const VistaDepthSenseDriver::DepthMeasure *pMeasure =
				pM->getRead<VistaDepthSenseDriver::DepthMeasure>();
			m_pDepthFrameHandler->ProcessFrame(pMeasure->frame);
		}
		iNewCount = m_pColorSensor->GetDataCount();
		if(iNewCount > m_iColorMeasures) {
			m_iColorMeasures = iNewCount;
			// new color data
			const VistaMeasureHistory &oHistory =
				m_pColorSensor->GetMeasures();
			const VistaSensorMeasure *pM = 
				oHistory.GetCurrentRead();
				
			const VistaDepthSenseDriver::ColorMeasure *pMeasure =
				pM->getRead<VistaDepthSenseDriver::ColorMeasure>();
			m_pColorFrameHandler->ProcessFrame(pMeasure->frame);
		}
	}

	void RHaPSODemo::HandleEvent(VistaEvent *pEvent) {
		if(pEvent->GetType() == VistaSystemEvent::GetTypeId()) {
			if(pEvent->GetId() == VistaSystemEvent::VSE_POSTAPPLICATIONLOOP) {
				FrameLoop();
			}
		}
	}
}
