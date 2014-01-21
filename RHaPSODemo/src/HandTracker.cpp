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

#include <OpenNI.h>

#include "HandTracker.hpp"

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

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	bool HandTracker::Initialize() {
		bool success=true;
		openni::Status rc = openni::STATUS_OK;

		const char* deviceURI = openni::ANY_DEVICE;

		openni::Device device;
		openni::VideoStream depth, color;

		rc = openni::OpenNI::initialize();

		if(rc != openni::STATUS_OK) {
			std::cerr << "HandTracker: OpenNI failed to initialize! Aborting."
					  << std::endl;
			return false;
		}

		rc = device.open(deviceURI);
		if (rc != openni::STATUS_OK)
		{
			std::cerr << "HandTracker: Device open failed:"
					  << std::endl << openni::OpenNI::getExtendedError()
					  << std::endl;
			openni::OpenNI::shutdown();
			return false;
		}
		std::cout << "device opened: " << device.getDeviceInfo().getName()
				  << std::endl;

		rc = depth.create(device, openni::SENSOR_DEPTH);
		if (rc == openni::STATUS_OK)
		{
			rc = depth.start();
			if (rc != openni::STATUS_OK)
			{
				std::cerr << "HandTracker: Couldn't start depth stream:"
						  << std::endl << openni::OpenNI::getExtendedError()
						  << std::endl;
				depth.destroy();
			}
		}
		else
		{
			std::cerr << "HandTracker: Couldn't find depth stream:"
					  << std::endl << openni::OpenNI::getExtendedError()
					  << std::endl;
		}

		rc = color.create(device, openni::SENSOR_COLOR);
		if (rc == openni::STATUS_OK)
		{
			rc = color.start();
			if (rc != openni::STATUS_OK)
			{
				std::cerr << "HandTracker: Couldn't start color stream:"
						  << std::endl << openni::OpenNI::getExtendedError()
						  << std::endl;
				color.destroy();
			}
		}
		else
		{
			std::cerr << "HandTracker: Couldn't find color stream:"
					  << std::endl <<openni::OpenNI::getExtendedError()
					  << std::endl;
		}

		if (!depth.isValid() || !color.isValid())
		{
			std::cerr << "HandTracker: No valid streams. Aborting." << std::endl;
			openni::OpenNI::shutdown();
			return false;
		}

		std::cout << "==================================================" << std::endl;
		std::cout << "depth stream info" << std::endl;
		PrintStreamInfo(depth);
		std::cout << "color stream info" << std::endl;
		PrintStreamInfo(color);
		std::cout << "==================================================" << std::endl;

		return true;
	}

	void HandTracker::PrintStreamInfo(const openni::VideoStream& stream) {
		// camera settings

		// sensor info

		// video mode
		openni::VideoMode vm = stream.getVideoMode();
		std::cout << "video mode:" << std::endl
							<< "resolution: x=" << vm.getResolutionX()
							<< " y=" << vm.getResolutionY() << std::endl
							<< "fps: " << vm.getFps() << std::endl;

		int originx=0, originy=0, width=0, height=0;
		stream.getCropping(&originx, &originy, &width, &height);
		std::cout << "cropping: originx=" << originx
							<< " originy=" << originy
							<< " width=" << width
							<< " height=" << height << std::endl;

		std::cout << "fov: h=" << stream.getHorizontalFieldOfView()
							<< " v=" << stream.getVerticalFieldOfView()
							<< std::endl;

		std::cout << "pixel values: min=" << stream.getMinPixelValue()
							<< " max=" << stream.getMaxPixelValue()
							<< std::endl;

		std::cout << "mirror: " << stream.getMirroringEnabled() << std::endl;
	}

}
