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
#include <cstring>
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

	bool HandTracker::FrameUpdate(const unsigned char  *colorFrame,
								  const unsigned short *depthFrame) {
		int iWidth  = 320;
		int iHeight = 240;

		// create writable copy of sensor measurement buffers
		unsigned char  colorBuffer[320*240*3];
		unsigned short depthBuffer[320*240];

		memcpy(colorBuffer, colorFrame, 320*240*3);
		memcpy(depthBuffer, depthFrame, 320*240*2);

		FilterSkinAreas(colorBuffer, depthBuffer);
		
		ImagePBOOpenGLDraw *pPBODraw = m_mapPBO[DEPTH];
		if(pPBODraw) {
			unsigned char pBuffer[iWidth*iHeight*3];
			DepthToRGB(depthBuffer, pBuffer);
			pPBODraw->FillPBOFromBuffer(pBuffer, 320, 240);
		}

		pPBODraw = m_mapPBO[COLOR];
		if(pPBODraw) {
			pPBODraw->FillPBOFromBuffer(colorBuffer, 320, 240);
		}

		return true;
	}

	void HandTracker::FilterSkinAreas(unsigned char  *colorFrame,
									  unsigned short *depthFrame) {

		// detects skin. loops through the pixels in the image, if the
		// pixel is skin then it leaves that pixel unchanged, or else it will    
		// color that pixel black. (Modified LOG). First, convert RGB
		// color space to IRgBy color space using this formula:

 		int R, G, B;
		for(size_t pixel = 0 ; pixel < 76800 ; pixel++) {
			R = colorFrame[3*pixel];
			G = colorFrame[3*pixel+1];
			B = colorFrame[3*pixel+2];

            // I= L(G);
			double Rg = log(R) - log(G);
			double By = log(B) - (log(G) +log(R)) / 2.0 ;

            // 20 <= I <= 90 from YIQ Color Space
			int I = int(0.5957*R - 0.2745*G - 0.3213*B);

			// ?
			int H = atan2(Rg,By) * (180.0 / 3.141592654);

			if (I >= 20 && I <= 90 && (H >= 100 && H <= 150) ) 
			{ 
				//skin; 
			} 
			else 
			{ 
				//non skin
				colorFrame[3*pixel] =
					colorFrame[3*pixel+1] =
					colorFrame[3*pixel+2] = 0;
				depthFrame[pixel] = 0;												 
			} 
		} 		
	}

	void HandTracker::DepthToRGB(const unsigned short *depth,
					unsigned char *rgb) {
		for(int i = 0 ; i < 76800 ; i++) {
			unsigned short val = depth[i];

			if(val > 0) {
				float linearvalue = val/2000.0;
				float mappedvalue = MapRangeExp(linearvalue);

				rgb[3*i] = rgb[3*i+1] = 255*(1-mappedvalue);
			}
			else {
				rgb[3*i+1] = 0;
				rgb[3*i+0] = rgb[3*i+2] = 0;
			}
		}
	}
}
