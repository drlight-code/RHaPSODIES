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

class VistaSystem;
class VistaTransformNode;
class VistaOpenGLNode;

class VistaPropertyList;

class DepthHistogramHandler;

// @todo: remove this hack! properly integrate into baselibs!
class IVistaDeSerializer;
IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const unsigned char* val );
IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const unsigned short* val );
IVistaDeSerializer &operator>> ( IVistaDeSerializer & ser, const float* val );



namespace rhapsodies {
	class ImageDraw;
	class ShaderRegistry;
	class HandModel;
	class HandModelView;
	class HandTracker;
	class HistogramUpdater;

	class RHaPSODemo : public VistaEventHandler {
		int m_camWidth, m_camHeight;

		VistaSystem *m_pSystem;
		ShaderRegistry *m_pShaderReg;

		HandModel     *m_pHandModel;
		HandModelView *m_pHandModelView;
		HandTracker   *m_pTracker;

		VistaTransformNode *m_pSceneTransform;
		VistaTransformNode *m_pDiagramTransform;

		VistaTransformNode *m_pHandModelTransform;
		VistaOpenGLNode *m_pHandModelGLNode;

		ImageDraw *m_pDiagramDraw;
		ImageDraw *m_pColorDraw, *m_pColorSegDraw;
		ImageDraw *m_pDepthDraw, *m_pDepthSegDraw;
		ImageDraw *m_pUVMapDraw, *m_pUVMapSegDraw;

		DepthHistogramHandler *m_pDepthHistogramHandler;

		bool InitTracker();
		bool RegisterShaders();
		bool CreateScene();

		void ReadConfig();
		bool CheckForConfigSection(const VistaPropertyList &oPropList,
								   const std::string &sSectionName);

	public:
		RHaPSODemo();
		~RHaPSODemo();

		bool Initialize(int argc, char** argv);
		bool Run();

		void FrameLoop();

		// VistaEventHandler interface
		virtual void HandleEvent(VistaEvent *pEvent);

		static const std::string sRDIniFile;
		static const std::string sAppSectionName;
		static const std::string sCameraSectionName;
		static const std::string sTrackerSectionName;
	};
}

#endif // _RHAPSODIES_RHAPSODEMO
