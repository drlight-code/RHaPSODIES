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
#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VflTextRendering/VtrFontManager.h>
#include <VistaOGLExt/VistaShaderRegistry.h>

#include <Vfl2DDiagrams/V2dGlobalConfig.h>
#include <Vfl2DDiagrams/Diagrams/V2dDiagramDefault.h>
#include <Vfl2DDiagrams/V2dDiagramTextureVista.h>

#include <ImageDraw.hpp>
#include <ImagePBOOpenGLDraw.hpp>

#include <DepthHistogramHandler.hpp>

#include <ShaderRegistry.hpp>
#include <HistogramUpdater.hpp>

#include <HandTracker.hpp>
#include <HandTrackingNode.hpp>

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
namespace {
	template<class T>
	class PropertyGetAsString : public VdfnPortFactory::StringGet
	{
	public:
		typedef std::string (*Convert)( const T& );
		PropertyGetAsString( Convert pFConvert = NULL )
		: m_pFC( pFConvert )
		{
			if( m_pFC == NULL )
				m_pFC = &VistaConversion::ToString<T>;
		}

		virtual bool GetStringValue(IVistaPropertyGetFunctor *pF,
										const VistaSensorMeasure *pMeasure,
										std::string &strOut)
		{
			IVistaMeasureTranscode::TTranscodeValueGet<T> *pGet
				= dynamic_cast<IVistaMeasureTranscode::TTranscodeValueGet<T>*>(pF);
			if(pGet != NULL)
			{
				T value;
				if(pGet->GetValue(pMeasure, value))
				{
					strOut = m_pFC(value);
					return true;
				}
				return false;
			}

			IVistaMeasureTranscode::TTranscodeIndexedGet<T> *pGetIdx
				= dynamic_cast<IVistaMeasureTranscode::TTranscodeIndexedGet<T>*>(pF);
			if(pGetIdx == NULL)
				return false;

			unsigned int n=0;
			while(n<256)
			{
				T value;
				if( pGetIdx->GetValueIndexed(pMeasure, value, n) == true )
				{
					strOut += std::string(" ") + m_pFC(value);
				}
				else
					return true;
				++n;
			}

			return false;
		}

	private:
		Convert m_pFC;
	};
	
	template<typename T>
	void RegisterDefaultPortAndFunctorAccess( VdfnPortFactory* pFac )
	{
		pFac->AddPortAccess( typeid(T).name(),
					new VdfnPortFactory::CPortAccess(
							new VdfnTypedPortCreate<T>,
							new TVdfnTranscodePortSet<T>,
							new VdfnTypedPortStringGet<T> ) );

		pFac->AddFunctorAccess( typeid(T).name(),
					new VdfnPortFactory::CFunctorAccess(
							new VdfnTypedPortTypeCompareCreate<T>,
							new TActionSet<T>,
							new VdfnTypedPortCreate<T>,
							new PropertyGetAsString<T> ) );
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


namespace rhapsodies {
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
	RHaPSODemo::RHaPSODemo() :
		m_camWidth(320), m_camHeight(240),
		m_pSystem(new VistaSystem),
		m_pShaderReg(new ShaderRegistry),
		m_pTracker(new HandTracker),
		m_pSceneTransform(NULL), m_pDiagramTransform(NULL),
		m_pDiagramDraw(NULL), 
		m_pColorDraw(NULL),
		m_pColorSegDraw(NULL),
		m_pDepthDraw(NULL), 
		m_pDepthSegDraw(NULL), 
		m_pUVMapDraw(NULL),
		m_pUVMapSegDraw(NULL),
		m_pDepthHistogramHandler(NULL),
		m_pDrawMutex(new VistaMutex)
	{
	}

	RHaPSODemo::~RHaPSODemo() {
		delete m_pDepthHistogramHandler;

		delete m_pColorDraw;
		delete m_pColorSegDraw;
		delete m_pDepthDraw;
		delete m_pDepthSegDraw;
		delete m_pUVMapDraw;
		delete m_pUVMapSegDraw;
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

		VtrFontManager::GetInstance()
			->SetFontDirectory("resources/fonts/");
		VistaShaderRegistry::GetInstance()
			.AddSearchDirectory("resources/shaders/");
		V2dGlobalConfig::GetInstance()
			->SetDefaultFont("FreeSans.ttf");

   		success &= RegisterShaders();
		success &= InitTracker();
		success &= CreateScene();

		// register port and functor access for pointer types. this
		// needs to be cleanly integrated into the baselibs, with a
		// TVdfnPortSerializerAdapter specialization for pointer types
		// which inhibits serialization of pointers (e.g. by throwing
		// an exception).
		VdfnPortFactory *pPortFac = VdfnPortFactory::GetSingleton();
		RegisterDefaultPortAndFunctorAccess<const unsigned char*>( pPortFac );
		RegisterDefaultPortAndFunctorAccess<const unsigned short*>( pPortFac );
		
		// register tracking node with DFN
		VdfnNodeFactory *pNodeFac = VdfnNodeFactory::GetSingleton();
		pNodeFac->SetNodeCreator( "HandTracker",
								  new HandTrackingNodeCreate(m_pTracker) );
		
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
		m_pSceneTransform->Translate(-1, 0, -3.0);

		// ImageDraw for color image
		ImagePBOOpenGLDraw *pPBODraw = 
			new ImagePBOOpenGLDraw(m_camWidth, m_camHeight,
								   m_pShaderReg, m_pDrawMutex);
		m_pTracker->SetViewPBODraw(HandTracker::COLOR, pPBODraw); 

		m_pColorDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pColorDraw->GetTransformNode()->SetTranslation(VistaVector3D(-2,1,0));

		// ImageDraw for segmented color image
		pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight,
										  m_pShaderReg, m_pDrawMutex);
		m_pTracker->SetViewPBODraw(HandTracker::COLOR_SEGMENTED, pPBODraw); 

		m_pColorSegDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pColorSegDraw->GetTransformNode()->SetTranslation(VistaVector3D(-2,-1,0));

		// ImageDraw for depth image
		pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight,
										  m_pShaderReg, m_pDrawMutex);
		m_pTracker->SetViewPBODraw(HandTracker::DEPTH, pPBODraw); 

		m_pDepthDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pDepthDraw->GetTransformNode()->SetTranslation(VistaVector3D(0,1,0));
		m_pDepthHistogramHandler = new DepthHistogramHandler(pPBODraw);

		// ImageDraw for segmented depth image
		pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight,
										  m_pShaderReg, m_pDrawMutex);
		m_pTracker->SetViewPBODraw(HandTracker::DEPTH_SEGMENTED, pPBODraw); 

		m_pDepthSegDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pDepthSegDraw->GetTransformNode()->SetTranslation(VistaVector3D(0,-1,0));

		// ImageDraw for UV map
		pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight,
										  m_pShaderReg, m_pDrawMutex);
		m_pTracker->SetViewPBODraw(HandTracker::UV_MAP, pPBODraw); 

		m_pUVMapDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pUVMapDraw->GetTransformNode()->SetTranslation(VistaVector3D(2,1,0));

		// ImageDraw for segmented UV map
		pPBODraw = new ImagePBOOpenGLDraw(m_camWidth, m_camHeight,
										  m_pShaderReg, m_pDrawMutex);
		m_pTracker->SetViewPBODraw(HandTracker::UV_MAP_SEGMENTED, pPBODraw); 

		m_pUVMapSegDraw = new ImageDraw(m_pSceneTransform, pPBODraw, pSG);
		m_pUVMapSegDraw->GetTransformNode()->SetTranslation(VistaVector3D(2,-1,0));
		
		// ImageDraw for histogram
		m_pDiagramDraw = new ImageDraw(m_pSceneTransform,
									   m_pDepthHistogramHandler->GetDiagramTexture(),
									   pSG);
		m_pDiagramDraw->GetTransformNode()->SetTranslation(VistaVector3D(4,1,0));

		m_pSystem->GetEventManager()->RegisterObserver(
		  	m_pDepthHistogramHandler->GetHistogramUpdater(),
		 	VistaSystemEvent::GetTypeId());

		m_pDepthHistogramHandler->Enable(true);

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
