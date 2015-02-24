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

#include <GL/glcorearb.h>

#include <VistaAspects/VistaPropertyList.h>

namespace rhapsodies {
	class ImagePBOOpenGLDraw;
	class SkinClassifier;

	class HandModel;
	class HandModelRep;
	class HandRenderer;
	
	class HandTracker {
	public:
		enum ViewType {
			COLOR,
			COLOR_SEGMENTED,
			DEPTH,           // native camera depth map
			DEPTH_SEGMENTED, // segmented camera depth map
			DEPTH_PSO_RENDERED,  // tiled rendered depth map
			DEPTH_PSO_CAMERA,    // tiled camera depth map
			UVMAP,
			UVMAP_SEGMENTED
		};

		HandTracker();
		virtual ~HandTracker();
		
		void SetViewPBODraw(ViewType type,
							ImagePBOOpenGLDraw *pPBODraw);

		HandModel *GetHandModelLeft();
		HandModel *GetHandModelRight();
		HandModelRep *GetHandModelRep();

		void SetHandRenderer(HandRenderer *pRenderer);

		GLuint GetDepthTextureId();
		GLuint GetCameraTextureId();
		
		bool Initialize();
		void ReadConfig();

		void ProcessCameraFrames(
			const unsigned char  *colorFrame,
			const unsigned short *depthFrame,
			const float          *uvMapFrame);

		void PerformPSOTracking();
		
		bool FrameUpdate(const unsigned char  *colorFrame,
						 const unsigned short *depthFrame,
						 const float          *uvMapFrame);

		SkinClassifier *GetSkinClassifier();
		void NextSkinClassifier();
		void PrevSkinClassifier();

		void ShowOpenCVImg();
		void ToggleSkinMap();

		void RandomizeModels();

	private:
		struct Config {
		public:
			int iDepthLimit;   // depth cutoff in millimeters
			unsigned int iErosionSize;  // erosion blob size
			unsigned int iDilationSize; // dilation blob size

			unsigned int iPSOGenerations; // max pso generation count
		};

		void FilterSkinAreas();

		void DepthToRGB(const unsigned short *depth,
						unsigned char *rgb);

		void UVMapToRGB(const float *uvmap,
						const unsigned short *depth,
						const unsigned char *color,
						unsigned char *rgb);

		void PrintConfig();

		typedef std::map<ViewType, ImagePBOOpenGLDraw*> MapPBO;
		MapPBO m_mapPBO;

		typedef std::list<SkinClassifier*> ListSkinCl;
		ListSkinCl m_lClassifiers;
		ListSkinCl::iterator m_itCurrentClassifier;

		bool m_bCameraUpdate;
		bool m_bShowImage;
		bool m_bShowSkinMap;

		Config m_oConfig;
		VistaPropertyList m_oCameraIntrinsics;

		HandModel *m_pHandModelLeft;
		HandModel *m_pHandModelRight;
		HandModelRep *m_pHandModelRep;
		
		HandRenderer *m_pHandRenderer;

		unsigned char  m_pColorBuffer[320*240*3];
		unsigned short m_pDepthBuffer[320*240];

		unsigned char m_pSkinMap[320*240];

		// depth frame and uv map RGB buffers
		unsigned char m_pDepthRGBBuffer[320*240*3];
		unsigned char m_pUVMapRGBBuffer[320*240*3];

		GLuint m_idDepthTexture;
		GLuint m_idDepthTextureFBO;

		GLuint m_idCameraTexture;		
		GLuint m_idCameraTexturePBO;
		void *m_pCameraTexturePBO;
	};
}

#endif // _RHAPSODIES_HANDTRACKER
