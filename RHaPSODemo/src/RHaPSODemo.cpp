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
		m_pTracker(new HandTracker) {
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

	bool RHaPSODemo::Initialize(int argc, char** argv) {
		bool success = true;

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


		if(!m_pSystem->Init(argc, argv)) {
			success = false;
		}

		glewInit();

		if(!InitTracker())
			success = false;

		if(!RegisterShaders())
			success = false;

		if(!CreateScene())
			success = false;

		return success;
	}

	bool RHaPSODemo::InitTracker() {
		return m_pTracker->Initialize();
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
		m_pShaderReg->RegisterUniform("textured", "mat_mvp");

		return true;
	}

	bool RHaPSODemo::CreateScene() {
		VistaSceneGraph *pSG = m_pSystem->GetGraphicsManager()->GetSceneGraph();

		// create global scene transform
		m_pTransformNode = pSG->NewTransformNode(pSG->GetRoot());

		m_pRawImageDraw = new ImageDraw(m_pShaderReg);
		m_pRawImageGLNode = pSG->NewOpenGLNode(m_pTransformNode, m_pRawImageDraw);
		m_pTransformNode->AddChild(m_pRawImageGLNode);

		m_pTransformNode->Translate(0, 0, -1);

		VistaDisplayManager *pDM = m_pSystem->GetDisplayManager();

		// pDM->GetDisplaySystem()->GetDisplaySystemProperties()->
		// 	SetViewerPosition(VistaVector3D(0,0,1));
		// pDM->GetDisplaySystem()->GetReferenceFrame()->
		// 	SetTranslation(VistaVector3D(0,0,10));
		
		return true;
	}

	bool RHaPSODemo::Run() {
		return m_pSystem->Run();
	}

}
