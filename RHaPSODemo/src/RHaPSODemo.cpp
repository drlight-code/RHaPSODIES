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

#include <VistaTools/VistaProfiler.h>

#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>

#include <VistaKernel/EventManager/VistaEventManager.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <ImageDraw.hpp>
#include <ImageDrawUpdater.hpp>
#include <ShaderRegistry.hpp>
#include <HandTracker.hpp>

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
		m_camWidth(640), m_camHeight(480) {
	}

	RHaPSODemo::~RHaPSODemo() {
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

		success &= InitTracker();
   		success &= RegisterShaders();
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
											 640, RHaPSODemo::sRDIniFile);
		m_camHeight = oProf.GetTheProfileInt("CAMERAS", "RESOLUTION_Y",
											 480, RHaPSODemo::sRDIniFile);

		return true;
	}

	bool RHaPSODemo::InitTracker() {
		return m_pTracker->Initialize(m_camWidth,
									  m_camHeight);
	}

	bool RHaPSODemo::RegisterShaders() {
		m_pShaderReg->RegisterShader("vert_textured", GL_VERTEX_SHADER,   
									 "shaders/textured.vs");
		m_pShaderReg->RegisterShader("frag_textured", GL_FRAGMENT_SHADER,
									 "shaders/textured.fs");

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
		m_pSceneTransform->Translate(0, 0, -1);

		m_sDrawColor.pTransform = pSG->NewTransformNode(m_pSceneTransform);
		m_sDrawColor.pImageDraw = new ImageDraw(m_camWidth, m_camHeight,
												m_pShaderReg);
		m_sDrawColor.pGLNode = 
			pSG->NewOpenGLNode(m_sDrawColor.pTransform, 
							   m_sDrawColor.pImageDraw);
		m_sDrawColor.pTransform->AddChild(m_sDrawColor.pGLNode);
		m_sDrawColor.pImageDrawUpdater =
			new ImageDrawUpdater(&m_pTracker->GetDepthStream(),
								 m_sDrawColor.pImageDraw);

		return true;
	}

	bool RHaPSODemo::Run() {
		return m_pSystem->Run();
	}

}
