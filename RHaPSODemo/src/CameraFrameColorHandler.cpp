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

#include "CameraFrameColorHandler.hpp"

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
	CameraFrameColorHandler::CameraFrameColorHandler(openni::VideoStream *pStream,
													 ImagePBOOpenGLDraw *pDraw) :
		m_pStream(pStream),
		m_pDraw(pDraw) {
		pStream->addNewFrameListener(this);
	}

	CameraFrameColorHandler::~CameraFrameColorHandler() {
		m_pStream->removeNewFrameListener(this);
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	void CameraFrameColorHandler::onNewFrame(openni::VideoStream &) {
		openni::VideoFrameRef frame;
		m_pStream->readFrame(&frame);

		m_pDraw->FillPBOFromBuffer(frame.getData(),
								   frame.getWidth(),
								   frame.getHeight());
	}
}