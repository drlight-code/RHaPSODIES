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

#include "CameraFrameDepthHandler.hpp"

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
	CameraFrameDepthHandler::CameraFrameDepthHandler(openni::VideoStream *pStream,
													 ImagePBOOpenGLDraw *pDraw) :
		m_pStream(pStream),
		m_pDraw(pDraw) {
		openni::VideoMode vm = pStream->getVideoMode();
		m_pBuffer =	new unsigned char[vm.getResolutionX()*
									  vm.getResolutionY()*3];
		pStream->addNewFrameListener(this);
	}

	CameraFrameDepthHandler::~CameraFrameDepthHandler() {
		delete [] m_pBuffer;
		m_pStream->removeNewFrameListener(this);
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	void CameraFrameDepthHandler::onNewFrame(openni::VideoStream &) {
		openni::VideoFrameRef frame;
		m_pStream->readFrame(&frame);

		unsigned short* pData = (unsigned short*)frame.getData();
		for(int i = 0; i < frame.getWidth()*frame.getHeight(); i++) {
			if(pData[i] > 0) {
				m_pBuffer[3*i] = m_pBuffer[3*i+1] = /*m_pBuffer[3*i+2] =*/
					255 - pData[i] / 40;
			}
			else {
				m_pBuffer[3*i] = 200;
				m_pBuffer[3*i+1] = m_pBuffer[3*i+2] = 0;
			}
		}
		m_pDraw->FillPBOFromBuffer(m_pBuffer,
								   frame.getWidth(),
								   frame.getHeight());
	}
}
