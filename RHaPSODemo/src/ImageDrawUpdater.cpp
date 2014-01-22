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

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	void ImageDrawUpdater::onNewFrame (openni::VideoStream &) {
		// std::cout << "new frame buddy!" << std::endl;
		// std::cout << "i'm not your buddy, guy!" << std::endl;
		// std::cout << "i'm not your guy, friend!" << std::endl;
		// std::cout << "i'm not your friend, buddy!" << std::endl;
	}
}
