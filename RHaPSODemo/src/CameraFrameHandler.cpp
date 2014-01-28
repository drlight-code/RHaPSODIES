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

#include <iostream>

#include <ImagePBOOpenGLDraw.hpp>

#include "CameraFrameHandler.hpp"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

namespace rhapsodies {
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
	CameraFrameHandler::CameraFrameHandler(openni::VideoStream *pStream,
										   ImagePBOOpenGLDraw *pDraw) :
		m_pStream(pStream),
		m_pDraw(pDraw) {
		openni::VideoMode vm = pStream->getVideoMode();
		if(vm.getPixelFormat() == openni::PIXEL_FORMAT_DEPTH_1_MM) {
			m_pBuffer =	new unsigned char[vm.getResolutionX()*
										  vm.getResolutionY()*3];
		}
		pStream->addNewFrameListener(this);
	}

	CameraFrameHandler::~CameraFrameHandler() {
		openni::VideoMode vm = m_pStream->getVideoMode();
		if(vm.getPixelFormat() == openni::PIXEL_FORMAT_DEPTH_1_MM) {
			delete [] m_pBuffer;
		}
		m_pStream->removeNewFrameListener(this);
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	void CameraFrameHandler::onNewFrame(openni::VideoStream &) {
		openni::VideoFrameRef frame;
		m_pStream->readFrame(&frame);

		const void *pPBOData = frame.getData();

		if(frame.getVideoMode().getPixelFormat() == 
		   openni::PIXEL_FORMAT_DEPTH_1_MM) {
			unsigned short* pData = (unsigned short*)frame.getData();
			for(int i = 0; i < frame.getWidth()*frame.getHeight(); i++) {
				if(pData[i] > 0) {
					m_pBuffer[3*i] = m_pBuffer[3*i+1] = m_pBuffer[3*i+2] =
						255 - pData[i] / 40;
				}
				else {
					m_pBuffer[3*i] = 200;
					m_pBuffer[3*i+1] = m_pBuffer[3*i+2] = 0;
				}
			}
			pPBOData = m_pBuffer;
		}

		m_pDraw->FillPBOFromBuffer(pPBOData,
								   frame.getWidth(),
								   frame.getHeight());
	}
}
