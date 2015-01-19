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

#include <list>
#include <map>

#include <VistaAspects/VistaPropertyList.h>

namespace rhapsodies {
	class ImagePBOOpenGLDraw;
	class SkinClassifier;
	class HandModel;
	class HandModelView;
	
	class HandTracker {
	public:
		enum ViewType {
			COLOR,
			COLOR_SEGMENTED,
			DEPTH,
			DEPTH_SEGMENTED,
			UVMAP,
			UVMAP_SEGMENTED
		};

		HandTracker();
		virtual ~HandTracker();
		
		bool Initialize();
		void ReadConfig();

		void SetViewPBODraw(ViewType type,
							ImagePBOOpenGLDraw *pPBODraw);
		
		bool FrameUpdate(const unsigned char  *colorFrame,
						 const unsigned short *depthFrame,
						 const float          *uvMapFrame);

		SkinClassifier *GetSkinClassifier();
		void NextSkinClassifier();
		void PrevSkinClassifier();

		void ShowOpenCVImg();

	private:
		struct Config {
		public:
			int iDepthLimit;   // depth cutoff in millimeters
			int iErosionSize;  // erosion blob size
			int iDilationSize; // dilation blob size
		};

		void FilterSkinAreas(unsigned char *colorImage,
							 unsigned char *depthImage,
							 unsigned char *uvmapImage);

		void DepthToRGB(const unsigned short *depth,
						unsigned char *rgb);

		void UVMapToRGB(const float *uvmap,
						const unsigned short *depth,
						const unsigned char *color,
						unsigned char *rgb);

		typedef std::map<ViewType, ImagePBOOpenGLDraw*> MapPBO;
		MapPBO m_mapPBO;

		typedef std::list<SkinClassifier*> ListSkinCl;
		ListSkinCl m_lClassifiers;
		ListSkinCl::iterator m_itCurrentClassifier;

		bool m_bCameraUpdate;
		bool m_bShowImage;

		Config m_oConfig;

		HandModel *m_pHandModel;
		HandModelView *m_pHandModelView;

		unsigned char  m_pColorBuffer[320*240*3];
		unsigned short m_pDepthBuffer[320*240];

		unsigned char m_pSkinMap[320*240];

		// depth frame and uv map RGB buffers
		unsigned char pDepthRGBBuffer[320*240*3];
		unsigned char pUVMapRGBBuffer[320*240*3];
	};
}

#endif // _RHAPSODIES_HANDTRACKER
