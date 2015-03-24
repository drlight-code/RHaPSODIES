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

#include <VistaTools/VistaIniFileParser.h>

#include <VistaDataFlowNet/VdfnPortFactory.h>
#include <VistaDataFlowNet/VdfnHistoryPort.h>
#include <VistaDataFlowNet/VdfnActionObject.h>

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
#include <VistaKernel/GraphicsManager/VistaAxes.h>
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

//#include <VflTextRendering/VtrFontManager.h>
//#include <VistaOGLExt/VistaShaderRegistry.h>

// #include <Vfl2DDiagrams/V2dGlobalConfig.h>
// #include <Vfl2DDiagrams/Diagrams/V2dDiagramDefault.h>
// #include <Vfl2DDiagrams/V2dDiagramTextureVista.h>

#include <ShaderRegistry.hpp>

#include <HandModel.hpp>
#include <HandRenderer.hpp>
#include <HandTracker.hpp>
#include <HandTrackingNode.hpp>

#include <GLDraw/ImageDraw.hpp>
#include <GLDraw/ImagePBOOpenGLDraw.hpp>
#include <GLDraw/HandRenderDraw.hpp>

#include <DepthHistogramHandler.hpp>
#include <HistogramUpdater.hpp>

#include "RHaPSODemo.hpp"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace rhapsodies {
	const std::string RHaPSODemo::sRDIniFile          = "configfiles/rhapsodemo.ini";
	const std::string RHaPSODemo::sAppSectionName     = "APPLICATION";
	const std::string RHaPSODemo::sCameraSectionName  = "CAMERA";
	const std::string RHaPSODemo::sTrackerSectionName = "HANDTRACKER";
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace {
	template<typename T>
	void RegisterDefaultPortAccess( VdfnPortFactory* pFac )
	{
		pFac->AddPortAccess( typeid(T).name(),
					new VdfnPortFactory::CPortAccess(
							new VdfnTypedPortCreate<T>,
							new TVdfnTranscodePortSet<T>,
							new VdfnTypedPortStringGet<T> ) );
	}

	template<class T> void CondDelete(T* p) {
		if(p) {
			delete p;
			p = NULL;
		}
	}
}

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
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
	RHaPSODemo::RHaPSODemo() :
		m_camWidth(320), m_camHeight(240),
		m_pSystem(NULL),
		m_pShaderReg(NULL),
		m_pHandRenderDraw(NULL),
		m_pHandTracker(NULL),
		m_pSceneTransform(NULL),
		m_pDiagramTransform(NULL),
		m_pHandModelTransform(NULL),
		m_pHandModelGLNode(NULL),
		m_pAxesTransform(NULL),
		m_pAxes(NULL),
		m_pDiagramDraw(NULL), 
		m_pColorDraw(NULL),
		m_pColorSegDraw(NULL),
		m_pDepthDraw(NULL), 
		m_pDepthSegDraw(NULL), 
		m_pUVMapDraw(NULL),
		m_pUVMapSegDraw(NULL),
		m_pDepthRenderedDraw(NULL),
		m_pDepthCameraDraw(NULL),
		m_pDepthHistogramHandler(NULL) {

		m_pSystem = new VistaSystem;
		m_pShaderReg = new ShaderRegistry;
	}

	RHaPSODemo::~RHaPSODemo() {
		CondDelete(m_pDepthHistogramHandler);

		CondDelete(m_pColorDraw);
		CondDelete(m_pColorSegDraw);
		CondDelete(m_pDepthDraw);
		CondDelete(m_pDepthSegDraw);
		CondDelete(m_pUVMapDraw);
		CondDelete(m_pUVMapSegDraw);
		CondDelete(m_pDiagramDraw);
		CondDelete(m_pDepthRenderedDraw);
		CondDelete(m_pDepthCameraDraw);
		CondDelete(m_pDifferenceTextureDraw);
		CondDelete(m_pUnionTextureDraw);
		CondDelete(m_pIntersectionTextureDraw);

		CondDelete(m_pHandRenderDraw);
		CondDelete(m_pAxes);

		CondDelete(m_pHandTracker);
		CondDelete(m_pShaderReg);
		CondDelete(m_pSystem);
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	bool RHaPSODemo::Initialize(int argc, char** argv) {
		bool success = true;

		ReadConfig();

		success &= m_pSystem->Init(argc, argv);

		GLenum err = glewInit();
		if (err != GLEW_OK) {
			vstr::err() << "glewInit error: " << glewGetErrorString(err)
						<< std::endl;
			vstr::err() << "GLEW initialization failed! Aborting."
						<< std::endl;
			
			exit(1);
		}

		// VtrFontManager::GetInstance()
		// 	->SetFontDirectory("resources/fonts/");
		// VistaShaderRegistry::GetInstance()
		// 	.AddSearchDirectory("resources/shaders/");
		// V2dGlobalConfig::GetInstance()
		// 	->SetDefaultFont("FreeSans.ttf");

   		success &= RegisterShaders();
		success &= InitTracker();
		success &= CreateScene();

		// register port and functor access for pointer types. this
		// needs to be cleanly integrated into the baselibs, with a
		// TVdfnPortSerializerAdapter specialization for pointer types
		// which inhibits serialization of pointers (e.g. by throwing
		// an exception).
		VdfnPortFactory *pPortFac = VdfnPortFactory::GetSingleton();
		RegisterDefaultPortAccess<const unsigned char*>( pPortFac );
		RegisterDefaultPortAccess<const unsigned short*>( pPortFac );
		RegisterDefaultPortAccess<const float*>( pPortFac );
		
		// register tracking node with DFN
		VdfnNodeFactory *pNodeFac = VdfnNodeFactory::GetSingleton();
		pNodeFac->SetNodeCreator( "HandTracker",
								  new HandTrackingNodeCreate(m_pHandTracker) );
		
		return success;
	}
	
	void RHaPSODemo::ReadConfig() {
		VistaIniFileParser oIniParser(true);
		oIniParser.ReadFile(sRDIniFile);

		const VistaPropertyList &oConfig = oIniParser.GetPropertyList();

		if(!oConfig.HasProperty(sAppSectionName)) {
			throw std::runtime_error(
				std::string() + "Config section ["
				+ RHaPSODemo::sAppSectionName
				+ "] not found!");
		}
		if(!oConfig.HasProperty(sCameraSectionName)) {
			throw std::runtime_error(
				std::string() + "Config section ["
				+ RHaPSODemo::sCameraSectionName
				+ "] not found!");
		}
		
		const VistaPropertyList &oApplicationSection =
			oConfig.GetSubListConstRef( sAppSectionName );
		const VistaPropertyList &oCameraSection =
			oConfig.GetSubListConstRef( sAppSectionName );

		// read the ini file names from rhapsodemo ini
		m_pSystem->SetIniFile(
			oApplicationSection.GetValueOrDefault<std::string>("MAININI",
															   "vista.ini"));
		m_pSystem->SetDisplayIniFile(
			oApplicationSection.GetValueOrDefault<std::string>("DISPLAYINI",
															   "vista.ini"));
		m_pSystem->SetClusterIniFile(
			oApplicationSection.GetValueOrDefault<std::string>("CLUSTERINI",
															   "vista.ini"));
		m_pSystem->SetInteractionIniFile(
			oApplicationSection.GetValueOrDefault<std::string>("INTERACTIONINI",
															   "vista.ini"));

		// read camera parameters
		m_camWidth  = oCameraSection.GetValueOrDefault("RESOLUTION_X", 320);
		m_camHeight = oCameraSection.GetValueOrDefault("RESOLUTION_Y", 240);
	}

	bool RHaPSODemo::InitTracker() {
		bool success = true;

		m_pHandTracker = new HandTracker(m_pShaderReg);
		success &= m_pHandTracker->Initialize();

		// register frame update handler
		m_pSystem->GetEventManager()->AddEventHandler(
			this, VistaSystemEvent::GetTypeId(), 
			VistaSystemEvent::VSE_POSTAPPLICATIONLOOP);

		return success;
	}

	bool RHaPSODemo::RegisterShaders() {
		m_pShaderReg->RegisterShader(
			"vert_vpos", GL_VERTEX_SHADER,
			"resources/shaders/vpos.vert");
		m_pShaderReg->RegisterShader(
			"vert_vpos_indexedtransform", GL_VERTEX_SHADER,
			"resources/shaders/vpos_indexedtransform.vert");
		m_pShaderReg->RegisterShader(
			"vert_vpos_uv", GL_VERTEX_SHADER,   
			"resources/shaders/vpos_uv.vert");

		m_pShaderReg->RegisterShader(
			"frag_textured", GL_FRAGMENT_SHADER,
			"resources/shaders/textured.frag");
		m_pShaderReg->RegisterShader(
			"frag_textured_uint", GL_FRAGMENT_SHADER,
			"resources/shaders/textured_uint.frag");
		m_pShaderReg->RegisterShader(
			"frag_textured_uint8", GL_FRAGMENT_SHADER,
			"resources/shaders/textured_uint8.frag");
		m_pShaderReg->RegisterShader(
			"frag_depthtexture", GL_FRAGMENT_SHADER,
			"resources/shaders/depthtexture.frag");
		m_pShaderReg->RegisterShader(
			"frag_solid_green", GL_FRAGMENT_SHADER,
			"resources/shaders/solid_green.frag");
		m_pShaderReg->RegisterShader(
			"frag_solid_blue", GL_FRAGMENT_SHADER,
			"resources/shaders/solid_blue.frag");

		m_pShaderReg->RegisterShader(
			"indexed_viewport", GL_GEOMETRY_SHADER,
			"resources/shaders/indexed_viewport.geom");

		m_pShaderReg->RegisterShader(
			"reduction", GL_COMPUTE_SHADER,
			"resources/shaders/reduction.comp");

		m_pShaderReg->RegisterShader(
			"reduction_x", GL_COMPUTE_SHADER,
			"resources/shaders/reduction_x.comp");

		m_pShaderReg->RegisterShader(
			"difference_score", GL_COMPUTE_SHADER,
			"resources/shaders/difference_score.comp");

		std::vector<std::string> vec_shaders;
		vec_shaders.push_back("vert_vpos_uv");		
		vec_shaders.push_back("frag_textured");		
		m_pShaderReg->RegisterProgram("textured", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_uv");		
		vec_shaders.push_back("frag_textured_uint");		
		m_pShaderReg->RegisterProgram("textured_uint", vec_shaders);
	
		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_uv");		
		vec_shaders.push_back("frag_textured_uint8");		
		m_pShaderReg->RegisterProgram("textured_uint8", vec_shaders);
	
		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos");
		vec_shaders.push_back("frag_solid_green");		
		m_pShaderReg->RegisterProgram("vpos_green", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos");
		vec_shaders.push_back("frag_solid_blue");		
		m_pShaderReg->RegisterProgram("vpos_blue", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("vert_vpos_indexedtransform");
		vec_shaders.push_back("frag_solid_green");		
//		vec_shaders.push_back("indexed_viewport");		
		m_pShaderReg->RegisterProgram("indexedtransform", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("difference_score");
		m_pShaderReg->RegisterProgram("difference_score", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("reduction");
		m_pShaderReg->RegisterProgram("reduction", vec_shaders);

		vec_shaders.clear();
		vec_shaders.push_back("reduction_x");
		m_pShaderReg->RegisterProgram("reduction_x", vec_shaders);

		return true;
	}

	bool RHaPSODemo::CreateScene() {
		VistaGraphicsManager *pGraphicsMgr = m_pSystem->GetGraphicsManager();
		VistaSceneGraph *pSG = pGraphicsMgr->GetSceneGraph();

		pGraphicsMgr->SetBackgroundColor(VistaColor(32, 32, 32));

		// create global scene transform
		m_pSceneTransform = pSG->NewTransformNode(pSG->GetRoot());
		m_pSceneTransform->Translate(0, 0, -3.0);

		// hand model and view
		m_pHandRenderDraw = new HandRenderDraw(
			m_pHandTracker->GetHandModelLeft(),
			m_pHandTracker->GetHandModelRight(),
			m_pHandTracker->GetHandModelRep(),
			m_pShaderReg);
		
		m_pHandModelTransform = pSG->NewTransformNode(m_pSceneTransform);
		m_pHandModelTransform->SetTranslation(0,-0.10,2);
		m_pHandModelGLNode = pSG->NewOpenGLNode(m_pHandModelTransform,
												m_pHandRenderDraw);

		m_pAxesTransform = pSG->NewTransformNode(m_pHandModelTransform);
		m_pAxesTransform->SetScale(0.05f, 0.05f, 0.05f);
		m_pAxes = new VistaAxes(pSG, m_pAxesTransform);
								
		// ImageDraw: color image
		// ImagePBOOpenGLDraw *pPBODraw = 
		// 	new ImagePBOOpenGLDraw(m_camWidth, m_camHeight, m_pShaderReg);
		// m_pHandTracker->SetViewPBODraw(HandTracker::COLOR, pPBODraw); 

		// m_pColorDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		// m_pColorDraw->GetTransformNode()->SetTranslation(VistaVector3D(-2,1,0));

		// ImageDraw: depth image
		ImagePBOOpenGLDraw *pPBODraw;
		//= new ImagePBOOpenGLDraw(m_camWidth, m_camHeight, m_pShaderReg);
		// m_pHandTracker->SetViewPBODraw(HandTracker::DEPTH, pPBODraw); 

		// m_pDepthDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		// m_pDepthDraw->GetTransformNode()->SetTranslation(VistaVector3D(0,1,0));
//		m_pDepthHistogramHandler = new DepthHistogramHandler(pPBODraw);

		// ImageDraw: UV map
		pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight, m_pShaderReg);
		m_pHandTracker->SetViewPBODraw(HandTracker::UVMAP, pPBODraw); 

		m_pUVMapDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pUVMapDraw->GetTransformNode()->SetTranslation(VistaVector3D(-2,2,0));
		

		// ImageDraw: segmented color image
		// pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight, m_pShaderReg);
		// m_pHandTracker->SetViewPBODraw(HandTracker::COLOR_SEGMENTED, pPBODraw); 

		// m_pColorSegDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		// m_pColorSegDraw->GetTransformNode()->SetTranslation(VistaVector3D(-2,-1,0));

		// ImageDraw: segmented depth image
		pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight, m_pShaderReg);
		m_pHandTracker->SetViewPBODraw(HandTracker::DEPTH_SEGMENTED, pPBODraw); 

		m_pDepthSegDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pDepthSegDraw->GetTransformNode()->SetTranslation(VistaVector3D(0,2,0));

		// ImageDraw: segmented UV map
		// pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight, m_pShaderReg);
		// m_pHandTracker->SetViewPBODraw(HandTracker::UVMAP_SEGMENTED, pPBODraw); 

		// m_pUVMapSegDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		// m_pUVMapSegDraw->GetTransformNode()->SetTranslation(VistaVector3D(-2,1,0));
		
		// ImageDraw: rendered depth map
		TexturedQuadGLDraw *pTexDraw = new TexturedQuadGLDraw(
			m_pHandTracker->GetRenderedTextureId(), false, m_pShaderReg);
//		m_pHandTracker->SetViewPBODraw(HandTracker::DEPTH_PSO_RENDERED, pPBODraw); 

		m_pDepthRenderedDraw = new ImageDraw(m_pSceneTransform, pTexDraw, pSG);
		m_pDepthRenderedDraw->GetTransformNode()->SetTranslation(VistaVector3D(-2,0,0));

		// ImageDraw: camera depth map
		pTexDraw = new TexturedQuadGLDraw(
			m_pHandTracker->GetCameraTextureId(), false, m_pShaderReg);
//		m_pHandTracker->SetViewPBODraw(HandTracker::DEPTH_PSO_CAMERA, pPBODraw); 

		m_pDepthCameraDraw = new ImageDraw(m_pSceneTransform, pTexDraw, pSG);
		m_pDepthCameraDraw->GetTransformNode()->SetTranslation(VistaVector3D(0, 0,0));

		// ImageDraw: difference texture
		pTexDraw = new TexturedQuadGLDraw(
			m_pHandTracker->GetResultTextureId(),
			false, m_pShaderReg, "textured_uint");

		m_pDifferenceTextureDraw = new ImageDraw(m_pSceneTransform, pTexDraw, pSG);
		m_pDifferenceTextureDraw->GetTransformNode()->SetTranslation(VistaVector3D(2, 0,0));

		// // ImageDraw: union texture
		// pTexDraw = new TexturedQuadGLDraw(
		// 	m_pHandTracker->GetUnionTextureId(),
		// 	false, m_pShaderReg, "textured_uint8");

		// m_pUnionTextureDraw = new ImageDraw(m_pSceneTransform, pTexDraw, pSG);
		// m_pUnionTextureDraw->GetTransformNode()->SetTranslation(VistaVector3D(-2, -2,0));

		// // ImageDraw: intersection texture
		// pTexDraw = new TexturedQuadGLDraw(
		// 	m_pHandTracker->GetIntersectionTextureId(),
		// 	false, m_pShaderReg, "textured_uint8");

		// m_pIntersectionTextureDraw = new ImageDraw(m_pSceneTransform, pTexDraw, pSG);
		// m_pIntersectionTextureDraw->GetTransformNode()->SetTranslation(VistaVector3D(0, -2,0));

		// // ImageDraw for histogram
		// m_pDiagramDraw = new ImageDraw(m_pSceneTransform,
		// 							   m_pDepthHistogramHandler->GetDiagramTexture(),
		// 							   pSG);
		// m_pDiagramDraw->GetTransformNode()->SetTranslation(VistaVector3D(4,1,0));

		// m_pSystem->GetEventManager()->RegisterObserver(
		//   	m_pDepthHistogramHandler->GetHistogramUpdater(),
		//  	VistaSystemEvent::GetTypeId());

		//m_pDepthHistogramHandler->Enable(false);

		return true;
	}

	bool RHaPSODemo::Run() {
		return m_pSystem->Run();
	}

	void RHaPSODemo::FrameLoop() {

	}

	void RHaPSODemo::HandleEvent(VistaEvent *pEvent) {
		if(pEvent->GetType() == VistaSystemEvent::GetTypeId()) {
			if(pEvent->GetId() == VistaSystemEvent::VSE_POSTAPPLICATIONLOOP) {
				FrameLoop();
			}
		}
	}
}
