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

#ifndef _RHAPSODIES_DRAWMUTEXUPDATER
#define _RHAPSODIES_DRAWMUTEXUPDATER

#include <VistaKernel/EventManager/VistaEventObserver.h>

class VistaThreadEvent;
class V2dDiagramDefault;

namespace rhapsodies {
	class HistogramUpdater : public VistaEventObserver {
		V2dDiagramDefault *m_pDiag;
		VistaThreadEvent *m_pThreadEvent;

	public:
		HistogramUpdater(V2dDiagramDefault *pDiag);
		~HistogramUpdater();

		virtual void Notify(const VistaEvent *pEvent);

		VistaThreadEvent *GetThreadEvent();
	};
}

#endif // _RHAPSODIES_DRAWMUTEXUPDATER
