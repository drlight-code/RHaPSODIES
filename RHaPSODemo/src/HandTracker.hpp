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

#ifndef _RHAPSODIES_HANDTRACKER
#define _RHAPSODIES_HANDTRACKER

#include <OpenNI.h>

namespace rhapsodies {
  class HandTracker {
	  openni::Device m_oDevice;
	  openni::VideoStream m_oDStream, m_oCStream;
	  int m_camWidth, m_camHeight;

	  void PrintVideoModes(openni::SensorType);
	  void PrintVideoMode(const openni::VideoMode&);
	  void PrintStreamInfo(const openni::VideoStream&);
	  void PrintPixelFormat(openni::PixelFormat);

  public:
	  bool Initialize(int width, int height);

	  openni::VideoStream& GetDepthStream();
	  openni::VideoStream& GetColorStream();
  };
}

#endif // _RHAPSODIES_HANDTRACKER
