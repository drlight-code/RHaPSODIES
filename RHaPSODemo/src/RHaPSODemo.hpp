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

class VistaSystem;
class VistaTransformNode;
class VistaOpenGLNode;

namespace rhapsodies {
	class ImageDraw;
	class ImageDrawUpdater;
	class DrawMutexHandler;
	class ShaderRegistry;
	class HandTracker;

	class RHaPSODemo {
		int m_camWidth, m_camHeight;

		struct ImageDrawQuad {
			VistaTransformNode *pTransform;
			VistaOpenGLNode *pGLNode;
			ImageDraw *pImageDraw;
			ImageDrawUpdater *pImageDrawUpdater;
		};

		VistaSystem *m_pSystem;
		ShaderRegistry *m_pShaderReg;
		HandTracker *m_pTracker;

		VistaTransformNode *m_pSceneTransform;

		ImageDrawQuad m_sDrawColor;
		
		bool InitTracker();
		bool RegisterShaders();
		bool CreateScene();

		bool ParseConfig();

	public:
		RHaPSODemo();
		~RHaPSODemo();

		bool Initialize(int argc, char** argv);
		bool Run();

		static const std::string sRDIniFile;
	};
}

#endif // _RHAPSODIES_RHAPSODEMO
