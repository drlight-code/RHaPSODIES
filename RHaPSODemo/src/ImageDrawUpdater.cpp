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

#include <ImageDraw.hpp>

#include "ImageDrawUpdater.hpp"

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
	ImageDrawUpdater::ImageDrawUpdater(openni::VideoStream *pStream,
									   ImageDraw *pDraw) :
		m_pImageDraw(pDraw) 
	{
		std::cout << "added frame listener to video stream!"
				  << std::endl;
		pStream->addNewFrameListener(this);
	}

	ImageDrawUpdater::~ImageDrawUpdater() {}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	void ImageDrawUpdater::onNewFrame(openni::VideoStream &stream) {
		openni::VideoFrameRef frame;
		stream.readFrame(&frame);

		if(stream.getVideoMode().getPixelFormat() ==
		   openni::PIXEL_FORMAT_RGB888) {
			m_pImageDraw->FillPBOFromBuffer(frame.getData(),
											frame.getWidth(),
											frame.getHeight());
		}
		else if(stream.getVideoMode().getPixelFormat() ==
				openni::PIXEL_FORMAT_DEPTH_1_MM) {
			// for now we allocate an RGB888 array for the texture
			// @todo is there a more clever way to do this?!

			unsigned short* pData = (unsigned short*)(frame.getData());

			unsigned int depthbufsize = frame.getWidth()*frame.getHeight();
			unsigned char buf[3*depthbufsize];

			for(int i = 0; i < depthbufsize; i++) {
				buf[3*i] = buf[3*i+1] = buf[3*i+2] =
					255 - pData[i] / 40;
			}
			m_pImageDraw->FillPBOFromBuffer(buf,
											frame.getWidth(),
											frame.getHeight());
			
		}
	}
}
