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

#include <VistaTools/VistaProfiler.h>

#include <RHaPSODemo.hpp>

#include "HandTracker.hpp"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace {

}


namespace rhapsodies {
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	bool HandTracker::Initialize(int width, int height) {
		m_camWidth = width;
		m_camHeight = height;

		bool success = true;
		openni::Status rc = openni::STATUS_OK;

		const char* deviceURI = openni::ANY_DEVICE;

		// initialize openni
		rc = openni::OpenNI::initialize();

		if(rc != openni::STATUS_OK) {
			std::cerr << "HandTracker: OpenNI failed to initialize! Aborting."
					  << std::endl;
			return false;
		}

		// open camera device (first found for now)
		rc = m_oDevice.open(deviceURI);
		if (rc != openni::STATUS_OK)
		{
			std::cerr << "HandTracker: Device open failed:"
					  << std::endl << openni::OpenNI::getExtendedError()
					  << std::endl;
			openni::OpenNI::shutdown();
			return false;
		}
		std::cout << "device opened: " << m_oDevice.getDeviceInfo().getName()
				  << std::endl;


		// enumerate video modes
		PrintVideoModes(openni::SENSOR_DEPTH);
		PrintVideoModes(openni::SENSOR_COLOR);

		// create depth videostream
		rc = m_oDStream.create(m_oDevice, openni::SENSOR_DEPTH);
		if (rc == openni::STATUS_OK)
		{
			// set depth stream format
			openni::VideoMode vMode;
			vMode.setResolution(m_camWidth, m_camHeight);
			vMode.setFps(30);
			vMode.setPixelFormat(openni::PIXEL_FORMAT_DEPTH_1_MM);
			m_oDStream.setVideoMode(vMode);

			rc = m_oDStream.start();
			if (rc != openni::STATUS_OK)
			{
				std::cerr << "HandTracker: Couldn't start depth stream:"
						  << std::endl << openni::OpenNI::getExtendedError()
						  << std::endl;
				m_oDStream.destroy();
			}
		}
		else
		{
			std::cerr << "HandTracker: Couldn't find depth stream:"
					  << std::endl << openni::OpenNI::getExtendedError()
					  << std::endl;
		}

		// create color videostream
		rc = m_oCStream.create(m_oDevice, openni::SENSOR_COLOR);
		if (rc == openni::STATUS_OK)
		{
			// set color stream format
			openni::VideoMode vMode;
			vMode.setResolution(m_camWidth, m_camHeight);
			vMode.setFps(30);
			vMode.setPixelFormat(openni::PIXEL_FORMAT_RGB888);
			m_oCStream.setVideoMode(vMode);

			rc = m_oCStream.start();
			if (rc != openni::STATUS_OK)
			{
				std::cerr << "HandTracker: Couldn't start color stream:"
						  << std::endl << openni::OpenNI::getExtendedError()
						  << std::endl;
				m_oCStream.destroy();
			}
		}
		else
		{
			std::cerr << "HandTracker: Couldn't find color stream:"
					  << std::endl <<openni::OpenNI::getExtendedError()
					  << std::endl;
		}

		if (!m_oDStream.isValid() || !m_oCStream.isValid())
		{
			std::cerr << "HandTracker: No valid streams. Aborting." << std::endl;
			openni::OpenNI::shutdown();
			return false;
		}

		std::cout << "==================================================" << std::endl;
		std::cout << "* depth stream info" << std::endl;
		PrintStreamInfo(m_oDStream);
		std::cout << "* color stream info" << std::endl;
		PrintStreamInfo(m_oCStream);
		std::cout << "==================================================" << std::endl;

		return true;
	}

	openni::VideoStream& HandTracker::GetDepthStream() {
		return m_oDStream;
	}
	
	openni::VideoStream& HandTracker::GetColorStream() {
		return m_oCStream;
	}

	void HandTracker::PrintVideoModes(openni::SensorType type) {
		const openni::SensorInfo *pSInfo = 
			m_oDevice.getSensorInfo(type);

		const openni::Array<openni::VideoMode> &aModes =
			pSInfo->getSupportedVideoModes();

		std::cout << "available video modes:" << std::endl
				  << "----------------------------------" << std::endl;
		for(int i = 0; i < aModes.getSize() ; i++) {
			const openni::VideoMode &vMode = aModes[i];
			std::cout << "* video mode " << i << ":" << std::endl;
			PrintVideoMode(vMode);			
		}
	}

	void HandTracker::PrintVideoMode(const openni::VideoMode &vMode) {
		std::cout << "resolution: "
				  << vMode.getResolutionX() << "x"
				  << vMode.getResolutionY() << std::endl;
		std::cout << "fps: " << vMode.getFps() << std::endl;
		PrintPixelFormat(vMode.getPixelFormat());
	}

	void HandTracker::PrintPixelFormat(openni::PixelFormat pf) {
		std::string sPFormat = "unknown";
		switch(pf) {
		case openni::PIXEL_FORMAT_DEPTH_1_MM:
			sPFormat = "DEPTH_1_MM";
			break;
		case openni::PIXEL_FORMAT_DEPTH_100_UM:
			sPFormat = "DEPTH_100_UM";
			break;
		case openni::PIXEL_FORMAT_SHIFT_9_2:
			sPFormat = "SHIFT_9_2";
			break;
		case openni::PIXEL_FORMAT_SHIFT_9_3:
			sPFormat = "SHIFT_9_3";
			break;
		case openni::PIXEL_FORMAT_RGB888:
			sPFormat = "RGB888";
			break;
		case openni::PIXEL_FORMAT_YUV422:
			sPFormat = "YUV422";
			break;
		case openni::PIXEL_FORMAT_GRAY8:
			sPFormat = "GRAY8";
			break;
		case openni::PIXEL_FORMAT_GRAY16:
			sPFormat = "GRAY16";
			break;
		case openni::PIXEL_FORMAT_JPEG:
			sPFormat = "JPEG";
			break;
		}
		std::cout << "pixel format: " << sPFormat << std::endl;
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
		
		PrintPixelFormat(vm.getPixelFormat());

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
