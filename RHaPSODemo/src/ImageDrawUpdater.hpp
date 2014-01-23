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

#ifndef _RHAPSODIES_IMAGEDRAWUPDATER
#define _RHAPSODIES_IMAGEDRAWUPDATER

#include <OpenNI.h>

namespace rhapsodies {
	class ImageDraw;

	class ImageDrawUpdater :
		public openni::VideoStream::NewFrameListener 
	{
		ImageDraw *m_pImageDraw;

	public:
		ImageDrawUpdater(openni::VideoStream *pStream,
						 ImageDraw *pDraw);
		virtual ~ImageDrawUpdater();

		virtual void onNewFrame(openni::VideoStream &);
  };
}

#endif // _RHAPSODIES_IMAGEDRAWUPDATER
