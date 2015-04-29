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

class VistaPropertyList;

class VistaSystem;
class VistaTransformNode;
class VistaOpenGLNode;
class VistaAxes;
class VistaSimpleTextOverlay;

class DepthHistogramHandler;

namespace rhapsodies {
	class ShaderRegistry;

	class HandRenderer;
	class HandRenderDraw;
	class HandTracker;

	class IDebugView;

	class ImageDraw;
	class HistogramUpdater;

	class RHaPSODemo : public VistaEventHandler {
	public:
		RHaPSODemo();
		~RHaPSODemo();

		bool Initialize(int argc, char** argv);
		bool Run();

		// VistaEventHandler interface
		virtual void HandleEvent(VistaEvent *pEvent);

	private:
		bool InitTracker();
		bool CreateScene();

		void ReadConfig();
		bool CheckForConfigSection(const VistaPropertyList &oPropList,
								   const std::string &sSectionName);

		int m_camWidth, m_camHeight;

		VistaSystem *m_pSystem;
		ShaderRegistry *m_pShaderReg;

		HandRenderDraw *m_pHandRenderDraw;
		HandTracker    *m_pHandTracker;

		VistaTransformNode *m_pSceneTransform;
		VistaTransformNode *m_pDiagramTransform;

		VistaTransformNode *m_pHandModelTransform;
		VistaOpenGLNode *m_pHandModelGLNode;

		VistaTransformNode *m_pAxesTransform;
		VistaAxes *m_pAxes;

		ImageDraw *m_pDiagramDraw;
		ImageDraw *m_pColorDraw, *m_pColorSegDraw;
		ImageDraw *m_pDepthDraw, *m_pDepthSegDraw;
		ImageDraw *m_pUVMapDraw, *m_pUVMapSegDraw;

		ImageDraw *m_pDepthRenderedDraw;
		ImageDraw *m_pDepthCameraDraw;

		ImageDraw *m_pDifferenceTextureDraw;
		ImageDraw *m_pUnionTextureDraw;
		ImageDraw *m_pIntersectionTextureDraw;

		VistaSimpleTextOverlay *m_pTextOverlay;
		IDebugView *m_pDebugView;
		
		DepthHistogramHandler *m_pDepthHistogramHandler;

		bool m_bFrameRecording;	   
	};
}

#endif // _RHAPSODIES_RHAPSODEMO
