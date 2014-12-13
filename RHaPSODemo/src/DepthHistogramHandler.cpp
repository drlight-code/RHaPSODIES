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

#include <limits>
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

#include "DepthHistogramHandler.hpp"

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
	DepthHistogramHandler::DepthHistogramHandler(ImagePBOOpenGLDraw *pDraw) :
		m_pDraw(pDraw),
		m_iHistNumBins(20),
		m_iHistDrawCounter(0),
		m_iHistDrawInterval(15)
	{
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
		pAxisY->SetRange(0, 20000);
		
		V2dStringAxis *pAxisX =
			dynamic_cast<V2dStringAxis*>(m_pDiagram->GetAxisX());
		pAxisX->SetLabelOrientation(V2dStringAxis::LO_VERTICAL);

		m_pHistUpdater = new HistogramUpdater(m_pDiagram);
	}

	DepthHistogramHandler::~DepthHistogramHandler() {
		Enable(false);

		delete m_pHistUpdater;
		delete m_pDataSeries;
		delete m_pDiagram;
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	V2dDiagramTextureVista *DepthHistogramHandler::GetDiagramTexture() {
		return m_pDiagramTexture;
	}

	HistogramUpdater *DepthHistogramHandler::GetHistogramUpdater() {
		return m_pHistUpdater;
	}

	ImagePBOOpenGLDraw *DepthHistogramHandler::GetPBODraw() {
		return m_pDraw;
	}

	bool DepthHistogramHandler::Enable(bool bEnable) {
		m_bEnabled = bEnable;
		return true;
	}

	bool DepthHistogramHandler::isEnabled() {
		return m_bEnabled;
	}

	void DepthHistogramHandler::ProcessFrame(const unsigned short* pFrame) {
		int iWidth  = 320;
		int iHeight = 240;

		// calculate depth histogram
		std::vector<int> vecBins(m_iHistNumBins);
		int minVal = 1;
		int maxVal = 2000;

		// add 1 for "rounding up" so we stay within index range
		int binWidth = (maxVal - minVal) / (m_iHistNumBins-1) + 1;

		bool bUpdateHistogram = false;
		if(++m_iHistDrawCounter % m_iHistDrawInterval == 0) {
			bUpdateHistogram = true;
			m_iHistDrawCounter = 0;
		}

		if(bUpdateHistogram) {
			int iters = iWidth*iHeight;
			for(int i = 0; i < iters; i++) {
				unsigned short val = pFrame[i];
				if(val == 0)
					vecBins[0]++;
				else {
					val = val > maxVal ? maxVal : val;
					val = val < minVal ? minVal : val;
					
					size_t iBin = (val-minVal)/binWidth+1;
					vecBins[iBin]++;
				}

				m_pDataSeries->ReplaceDataPointsY(0, 0, m_iHistNumBins, vecBins);
				m_pHistUpdater->GetThreadEvent()->SignalEvent();
			}
		}
	}
}
