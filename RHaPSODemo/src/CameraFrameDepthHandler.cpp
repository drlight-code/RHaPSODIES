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

#include <Vfl2DDiagrams/Diagrams/V2dDiagramDefault.h>
#include <Vfl2DDiagrams/V2dDiagramTextureVista.h>
#include <Vfl2DDiagrams/Data/V2dDataSeriesTypes.h>

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
		CameraFrameHandler(pStream, pDraw) {
		m_pDiagram = new V2dDiagramDefault(
			V2dDiagramDefault::AT_NOMINAL,
			V2dDiagramDefault::AT_CONTINUOUS,
			V2dDiagramDefault::DT_BARS_VERTICAL);
		m_pDiagramTexture = new V2dDiagramTextureVista(1024, m_pDiagram);

		m_pDataSeries = new V2dDataSeriesFloatOverString();
		m_pDataSeries->SetName("depth histogram");
		m_pDataSeries->AddDataPoint("fake", 0.0f);
		
		m_pDiagram->AddData(m_pDataSeries);

		openni::VideoMode vm = pStream->getVideoMode();
		m_pBuffer =	new unsigned char[vm.getResolutionX()*
									  vm.getResolutionY()*3];
	}

	CameraFrameDepthHandler::~CameraFrameDepthHandler() {
		delete [] m_pBuffer;
		delete m_pDataSeries;
		delete m_pDiagramTexture;
		delete m_pDiagram;
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	void CameraFrameDepthHandler::onNewFrame(openni::VideoStream &stream) {
		openni::VideoFrameRef frame;
		stream.readFrame(&frame);

		int nBins = 15;
		int zeroes = 0;
		int aBins[nBins];
		int minVal = stream.getMinPixelValue()+1;
		int maxVal = stream.getMaxPixelValue();
		int binWidth = (maxVal - minVal) / nBins;

		unsigned short* pData = (unsigned short*)frame.getData();
		for(int i = 0; i < frame.getWidth()*frame.getHeight(); i++) {
			unsigned short val = pData[i];

			// convert to RGB888 buffer
			if(val > 0) {
				m_pBuffer[3*i] = m_pBuffer[3*i+1] = 
					255 - val / 40;
			}
			else {
				m_pBuffer[3*i] = 200;
				m_pBuffer[3*i+1] = m_pBuffer[3*i+2] = 0;
			}
			// calculate histogram
			if(val == 0)
				zeroes++;
			else {
				aBins[(val-1)/binWidth]++;
			}			
		}
		m_pDataSeries->AddDataPoint("0", zeroes);
		for(int i = 0; i < nBins; i++) {
			m_pDataSeries->AddDataPoint("bin", aBins[i]);
		}
		
		GetPBODraw()->FillPBOFromBuffer(m_pBuffer,
										frame.getWidth(),
										frame.getHeight());
	}

	V2dDiagramTextureVista *CameraFrameDepthHandler::GetDiagramTexture() {
		return m_pDiagramTexture;
	}
}
