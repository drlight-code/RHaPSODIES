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

#include <cmath>
#include <iostream>

#include <VistaBase/VistaStreamUtils.h>

#include <RHaPSODemo.hpp>
#include <ImagePBOOpenGLDraw.hpp>

#include "HandTracker.hpp"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
namespace {
	float MapRangeExp(float value) {
		// map range 0-1 exponentially
		float base = 0.01;
		float ret = (1 - pow(base, value))/(1-base);
		
		// clamp
		ret = ret < 0.0 ? 0.0 : ret;
		ret = ret > 1.0 ? 1.0 : ret;

		return ret;
	}
}

namespace rhapsodies {
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	bool HandTracker::Initialize() {
		vstr::out() << "Initializing RHaPSODIES HandTracker" << std::endl;

		return true;
	}
	
	void HandTracker::SetViewPBODraw(ViewType type,
									 ImagePBOOpenGLDraw *pPBODraw) {
		m_mapPBO[type] = pPBODraw;
	}

	bool HandTracker::FrameUpdate(const unsigned short *depthFrame,
								  const unsigned char  *colorFrame) {
		int iWidth  = 320;
		int iHeight = 240;
		
		ImagePBOOpenGLDraw *pPBODraw = m_mapPBO[DEPTH];
		if(pPBODraw) {
			unsigned char pBuffer[iWidth*iHeight*3];
			DepthToRGB(depthFrame, pBuffer);
			pPBODraw->FillPBOFromBuffer(pBuffer, 320, 240);
		}

		pPBODraw = m_mapPBO[COLOR];
		if(pPBODraw) {
			pPBODraw->FillPBOFromBuffer(colorFrame,	320, 240);
		}

		return true;
	}

	void HandTracker::DepthToRGB(const unsigned short *depth,
					unsigned char *rgb) {
		for(int i = 0 ; i < 76800 ; i++) {
			unsigned short val = depth[i];

			if(val > 0) {
				float linearvalue = val/4000.0;
				float mappedvalue = MapRangeExp(linearvalue);

				rgb[3*i] = rgb[3*i+1] = 255*(1-mappedvalue);
			}
			else {
				rgb[3*i+1] = 200;
				rgb[3*i+0] = rgb[3*i+2] = 0;
			}
		}
	}
}
