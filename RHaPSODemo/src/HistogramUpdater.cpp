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

#include <VistaInterProcComm/Concurrency/VistaThreadEvent.h>

#include <VistaKernel/EventManager/VistaEvent.h>
#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <Vfl2DDiagrams/Diagrams/V2dDiagramDefault.h>

#include "HistogramUpdater.hpp"

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
	HistogramUpdater::HistogramUpdater(V2dDiagramDefault *pDiag) :
		m_pDiag(pDiag),
		m_pThreadEvent(new VistaThreadEvent(false)) {
	}
	
	HistogramUpdater::~HistogramUpdater() {
		delete m_pThreadEvent;
	}
	
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	void HistogramUpdater::Notify(const VistaEvent *pEvent) {
		if(pEvent->GetType() == VistaSystemEvent::GetTypeId()) {
			if(pEvent->GetId() == VistaSystemEvent::VSE_PREGRAPHICS) {
				// check if depth handler thread signaled us
				if(m_pThreadEvent->WaitForEvent(0)) {
					// update diagram texture
					m_pDiag->DataUpdated();
				}
			}
		}
	}

	VistaThreadEvent *HistogramUpdater::GetThreadEvent() {
		return m_pThreadEvent;
	}
}
