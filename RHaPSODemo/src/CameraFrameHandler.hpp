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

#ifndef _RHAPSODIES_CAMERAFRAMEHANDLER
#define _RHAPSODIES_CAMERAFRAMEHANDLER

#include <OpenNI.h>

namespace rhapsodies {
	class ImagePBOOpenGLDraw;

	class CameraFrameHandler :
		public openni::VideoStream::NewFrameListener {
		ImagePBOOpenGLDraw *m_pDraw;
		openni::VideoStream *m_pStream;
		bool m_bEnabled;

	public:
		CameraFrameHandler(openni::VideoStream *pStream,
						   ImagePBOOpenGLDraw *pDraw);
		virtual ~CameraFrameHandler();

		bool Enable(bool bEnable);
		bool isEnabled();

		ImagePBOOpenGLDraw *GetPBODraw();
  };
}

#endif // _RHAPSODIES_CAMERAFRAMEHANDLER
