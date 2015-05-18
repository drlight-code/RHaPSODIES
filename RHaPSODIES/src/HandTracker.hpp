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

#include "DebugView.hpp"

class VistaRandomNumberGenerator;

namespace rhapsodies {
	class ImagePBOOpenGLDraw;
	class SkinClassifier;

	class HandModel;
	class HandGeometry;
	class HandRenderer;

	class IDebugView;

	class Particle;
	class ParticleSwarm;

	class CameraFrameRecorder;
	class CameraFramePlayer;
	
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
		
		HandModel *GetHandModelLeft();
		HandModel *GetHandModelRight();
		HandGeometry *GetHandGeometry();

		GLuint GetRenderedTextureId();
		GLuint GetCameraTextureId();

		GLuint GetResultTextureId();
		GLuint GetDifferenceTextureId();
		GLuint GetUnionTextureId();
		GLuint GetIntersectionTextureId();
		
		void SetDebugView(IDebugView *pDebugView);
		void SetViewPBODraw(ViewType type,
							ImagePBOOpenGLDraw *pPBODraw);
		void SetHandRenderer(HandRenderer *pRenderer);

		bool Initialize();
		
		void ReadConfig();
		void PrintConfig();

		bool FrameUpdate(const unsigned char  *colorFrame,
						 const unsigned short *depthFrame,
						 const float          *uvMapFrame);

		SkinClassifier *GetSkinClassifier();
		void NextSkinClassifier();
		void PrevSkinClassifier();

		void ShowOpenCVImg();
		void ToggleSkinMap();

		void ToggleFrameRecording();
		void ToggleFramePlayback();

		void StartTracking();
		void StopTracking();
		bool IsTracking();
		
	private:
		struct Config {
		public:
			int iDepthLimit;   // depth cutoff in millimeters
			unsigned int iErosionSize;  // erosion blob size
			unsigned int iDilationSize; // dilation blob size

			unsigned int iPSOGenerations; // max pso generation count

			std::string sRecordingFile;
			bool bLoop;

			bool bAutoTracking;

			float fPenaltyMin;
			float fPenaltyMax;
			float fPenaltyStart;

			unsigned int iViewportBatch;
		};

		bool HasGLComputeCapabilities();

		bool InitSkinClassifiers();
		bool InitRendering();
		bool InitGpuPSO();
		bool InitReduction();
		bool InitParticleSwarm();

		void SetToInitialPose(Particle &oParticle);
		void PerformPSOTracking();
		void PerformStartPoseMatch();
		
		void ProcessCameraFrames(
			const unsigned char  *colorFrame,
			const unsigned short *depthFrame,
			const float          *uvMapFrame);

		void DepthToRGB(const unsigned short *depth,
						unsigned char *rgb);

		void UVMapToRGB(const float *uvmap,
						const unsigned short *depth,
						const unsigned char *color,
						unsigned char *rgb);

		void FilterSkinAreas();

		void ResourcesBind();
		void ResourcesUnbind();
		
		void UploadCameraDepthMap();
		void SetupProjection();

		void UploadHandModels();
		void GenerateTransforms();

		void ReduceDepthMaps();
		void GpuPSOStep();
		void UpdateScores();
		void UpdateBestMatch();
		
		float Penalty(HandModel& oModelLeft,
					  HandModel& oModelRight,
					  float fDiff,
					  float fUnion,
					  float fIntersection);
		float PenaltyFromReduction(float fDiff,
								   float fUnion,
								   float fIntersection);
		float PenaltyPrior(HandModel& oModel);
		float PenaltyNormalize(float fPenalty);
						  
		void WriteDebug(IDebugView::Slot eSlot,
						std::string sMessage);

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

		ShaderRegistry *m_pShaderReg;
		
		HandGeometry *m_pHandGeometry;
		HandRenderer *m_pHandRenderer;

		unsigned char  *m_pColorBuffer;
		unsigned short *m_pDepthBuffer;
		unsigned int   *m_pDepthBufferUInt;
		float          *m_pUVMapBuffer;

		unsigned char m_pSkinMap[320*240];

		// depth frame and uv map RGB buffers
		unsigned char m_pDepthRGBBuffer[320*240*3];
		unsigned char m_pUVMapRGBBuffer[320*240*3];

		std::vector<float> m_vViewportData;

		IDebugView *m_pDebugView;

		GLuint m_idRenderedTexture;
		GLuint m_idRenderedTextureFBO;

		GLuint m_idCameraTexture;		
		GLuint m_idCameraTexturePBO;
		void *m_pCameraTexturePBO;

		GLuint m_idGenerateTransformsProgram;

		GLuint m_idPrepareReductionTexturesProgram;
		GLuint m_idReductionXProgram;
		GLuint m_idReductionYProgram;

		GLuint m_idUpdateScoresProgram;
		GLuint m_idUpdateGBestProgram;
		GLuint m_idUpdateSwarmProgram;

		// GLuint m_idResultTexture;
		// GLuint m_idFinalResultTexture;
		GLuint m_idDifferenceTexture;
		// GLuint m_idUnionTexture;
		// GLuint m_idIntersectionTexture;

		enum TextureIndex {
			DIFFERENCE,
			UNION,
			INTERSECTION
		};
		
		GLuint m_idReductionTextures320x256[3];
		GLuint m_idReductionTextures40x32[3];
		GLuint m_idReductionTextures5x4[3];
		GLuint m_idReductionTextures1x1[3];

		GLuint m_idResultPBO;
		unsigned int *m_pResultBuffer;

		GLuint m_idSSBOHandModels;
		GLuint m_idSSBOHandModelsIBest;
		GLuint m_idSSBOHandModelsGBest;
		GLuint m_idSSBOHandModelsVelocity;
		GLuint m_idSSBOHandGeometry;
		GLuint m_idSSBORandom;
		GLuint m_idSSBODebug;
		
		GLint  m_locColorUniform;
		GLuint m_idColorFragProgram;

		bool m_bFrameRecording;
		bool m_bFramePlayback;
		CameraFrameRecorder *m_pFrameRecorder;
		CameraFramePlayer   *m_pFramePlayer;

		bool m_bTrackingEnabled;
		Particle *m_pParticleBest;
		ParticleSwarm *m_pSwarm;

		VistaRandomNumberGenerator *m_pRNG;
		GLint m_locRandomOffsetUniform;
	};
}

#endif // _RHAPSODIES_HANDTRACKER
