/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#ifndef _RHAPSODIES_RHAPSODEMO
#define _RHAPSODIES_RHAPSODEMO

#include <VistaKernel/EventManager/VistaEventHandler.h>

class VistaMutex;
class VistaSystem;
class VistaTransformNode;
class VistaOpenGLNode;
class VistaDepthSenseDriver;
class VistaDeviceSensor;

class DepthHistogramHandler;

namespace rhapsodies {
	class ImageDraw;
	class ShaderRegistry;
	class HandTracker;
	class HistogramUpdater;

	class RHaPSODemo : public VistaEventHandler {
		int m_camWidth, m_camHeight;

		VistaSystem *m_pSystem;
		ShaderRegistry *m_pShaderReg;
		VistaDepthSenseDriver *m_pDriver;
		VistaDeviceSensor *m_pColorSensor;
		VistaDeviceSensor *m_pDepthSensor;
		int m_iDepthMeasures, m_iColorMeasures;

		HandTracker *m_pTracker;

		VistaTransformNode *m_pSceneTransform;
		VistaTransformNode *m_pDiagramTransform;

		ImageDraw *m_pDiagramDraw;
		ImageDraw *m_pColorDraw;
		ImageDraw *m_pDepthDraw;
		ImageDraw *m_pUVMapDraw;

		DepthHistogramHandler *m_pDepthHistogramHandler;

		VistaMutex *m_pDrawMutex;

		bool InitTracker();
		bool RegisterShaders();
		bool CreateScene();

		bool ParseConfig();

	public:
		RHaPSODemo();
		~RHaPSODemo();

		bool Initialize(int argc, char** argv);
		bool Run();

		void FrameLoop();

		// VistaEventHandler interface
		virtual void HandleEvent(VistaEvent *pEvent);

		static const std::string sRDIniFile;
	};
}

#endif // _RHAPSODIES_RHAPSODEMO
