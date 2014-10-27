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
#include <sstream>

#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>

#include <Vfl2DDiagrams/Diagrams/V2dDiagramDefault.h>
#include <Vfl2DDiagrams/V2dDiagramTextureVista.h>
#include <Vfl2DDiagrams/Data/V2dDataSeriesTypes.h>
#include <Vfl2DDiagrams/DiagramComponents/V2dContinuousAxis.h>
#include <Vfl2DDiagrams/DiagramComponents/V2dDiscreteAxis.h>

#include <ImagePBOOpenGLDraw.hpp>
#include <HistogramUpdater.hpp>

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
	CameraFrameDepthHandler::CameraFrameDepthHandler(ImagePBOOpenGLDraw *pDraw) :
		m_iHistNumBins(64),
		m_iHistDrawCounter(0),
		m_iHistDrawInterval(15), // update histogram every half second at 30fps
		CameraFrameHandler(pDraw) {
		m_pDiagram = new V2dDiagramDefault(
			V2dDiagramDefault::AT_NOMINAL,
			V2dDiagramDefault::AT_CONTINUOUS_INT,
			V2dDiagramDefault::DT_BARS_VERTICAL);
		m_pDiagramTexture = new V2dDiagramTextureVista(1024, m_pDiagram);
		m_pDiagramTexture->SetScaleUniform(1.6);
		m_pDiagramTexture->SetUseMipMaps(false);

		m_pDataSeries = new V2dDataSeriesIntContinuousOverString();
		m_pDataSeries->SetName("depth histogram");

		m_pDataSeries->AddDataPoint("0", 50);
		for(int i = 1; i < m_iHistNumBins; i++) {
			std::stringstream ss;
			ss << "bin " << i;
			m_pDataSeries->AddDataPoint(ss.str(), 100);
		}
		m_pDiagram->AddData(m_pDataSeries);
		
		V2dIntAxisContinuous *pAxisY =
			dynamic_cast<V2dIntAxisContinuous*>(m_pDiagram->GetAxisY());
		pAxisY->SetAutoScaleMode(V2dIntAxisContinuous::ASM_MANUAL_EXACT);
		pAxisY->SetMin(0);
		pAxisY->SetMax(40000);

		V2dStringAxis *pAxisX =
			dynamic_cast<V2dStringAxis*>(m_pDiagram->GetAxisX());
		pAxisX->SetLabelOrientation(V2dStringAxis::LO_VERTICAL);

		m_pHistUpdater = new HistogramUpdater(m_pDiagram);

		// openni::VideoMode vm = pStream->getVideoMode();
		// m_pBuffer =	new unsigned char[vm.getResolutionX()*
		// 							  vm.getResolutionY()*3];
	}

	CameraFrameDepthHandler::~CameraFrameDepthHandler() {
		Enable(false);

		delete [] m_pBuffer;
		delete m_pHistUpdater;
		delete m_pDataSeries;
		delete m_pDiagram;
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	void CameraFrameDepthHandler::onNewFrame() {
		// openni::VideoFrameRef frame;
		// stream.readFrame(&frame);

		// int iWidth  = frame.getWidth();
		// int iHeight = frame.getHeight();

		// std::vector<int> vecBins(m_iHistNumBins);
		// int minVal = stream.getMinPixelValue()+1; // exclude 0
		// int maxVal = stream.getMaxPixelValue();
		// int binWidth = (maxVal - minVal) / (m_iHistNumBins-1);

		// bool bUpdateHistogram = false;
		// if(++m_iHistDrawCounter % m_iHistDrawInterval == 0) {
		// 	bUpdateHistogram = true;
		// 	m_iHistDrawCounter = 0;
		// }

		// unsigned short* pData = (unsigned short*)frame.getData();
		// for(int i = 0; i < iWidth*iHeight; i++) {
		// 	unsigned short val = pData[i];

		// 	// convert to RGB888 buffer
		// 	if(val > 0) {
		// 		m_pBuffer[3*i] = m_pBuffer[3*i+1] = 
		// 			255 - val / 40;
		// 	}
		// 	else {
		// 		m_pBuffer[3*i] = 200;
		// 		m_pBuffer[3*i+1] = m_pBuffer[3*i+2] = 0;
		// 	}

		// 	if(bUpdateHistogram) {
		// 		// calculate histogram
		// 		if(val == 0)
		// 			vecBins[0]++;
		// 		else {
		// 			vecBins[(val-1)/binWidth+1]++;
		// 		}
		// 	}
		// }

		// if(bUpdateHistogram) {
		// 	m_pDataSeries->ReplaceDataPointsY(0, 0, m_iHistNumBins, vecBins);
		// 	m_pHistUpdater->GetThreadEvent()->SignalEvent();
		// }
		
		// GetPBODraw()->FillPBOFromBuffer(m_pBuffer,
		// 								frame.getWidth(),
		// 								frame.getHeight());
	}

	V2dDiagramTextureVista *CameraFrameDepthHandler::GetDiagramTexture() {
		return m_pDiagramTexture;
	}

	HistogramUpdater *CameraFrameDepthHandler::GetHistogramUpdater() {
		return m_pHistUpdater;
	}
}
