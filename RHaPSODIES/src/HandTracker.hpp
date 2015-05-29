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

#include <map>

#include <VistaAspects/VistaPropertyList.h>

#include "DebugView.hpp"

class VistaRandomNumberGenerator;
class VistaBasicProfiler;

namespace rhapsodies {
	class SkinClassifier;

	class HandModel;
	class HandGeometry;
	class HandRenderer;

	class IDebugView;

	class Particle;
	class ParticleSwarm;

	class CameraFrameRecorder;
	class CameraFramePlayer;
	class CameraFrameFilter;
	
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
		void SetHandRenderer(HandRenderer *pRenderer);

		bool Initialize();
		
		void ReadConfig();
		void PrintConfig();

		bool FrameUpdate(const unsigned char  *colorFrame,
						 const unsigned short *depthFrame,
						 const float          *uvMapFrame);

		void NextSkinClassifier();
		void PrevSkinClassifier();

		void ToggleFrameRecording();
		void ToggleFramePlayback();

		void StartTracking();
		void StopTracking();
		bool GetIsTracking();
		
	private:
		struct Config {
		public:
			int iDepthLimit;   // depth cutoff in millimeters
			unsigned int iErosionSize;  // erosion blob size
			unsigned int iDilationSize; // dilation blob size

			std::string              sRecordingFile;
			std::vector<std::string> vecPlaybackFiles;
			unsigned int             iIterations;
			std::string              sCondition;
			bool                     bLoop;

			float fPenaltyMin;
			float fPenaltyMax;
			float fPenaltyStart;
			bool bAutoTracking;
			float fSmoothingFactor;
			
			unsigned int iViewportBatch;

			unsigned int iPSOGenerations;
			float fPhiCognitiveBegin;
			float fPhiCognitiveEnd;
			unsigned int iKeepKBest;
		};

		bool HasGLComputeCapabilities();

		bool InitFrameFilter();
		bool InitRendering();
		bool InitGpuPSO();
		bool InitReduction();
		bool InitParticleSwarm();
		bool InitOutputModel();
		bool InitEvaluation();

		void SetToInitialPose(Particle &oParticle);
		void PerformPSOTracking();
		void PerformStartPoseMatch();

		void FrameRecordingAndPlayback(
			const unsigned char  *colorFrame,
			const unsigned short *depthFrame,
			const float          *uvMapFrame);


		void ResourcesBind();
		void ResourcesUnbind();
		
		void UploadCameraDepthMap();
		void SetupProjection();

		void UploadHandModels();
		void DownloadHandModels();
		
		void GenerateTransforms();

		void ReduceDepthMaps();
		void UpdateScores();
		void UpdateSwarm(float fPhiCognitive, float fPhiSocial);

		void UpdateOutputModel();
		void SmoothInterpolateModel(float fSmoothingFactor,
									HandModel *pModelNew,
									HandModel *pModelOld);

		void EvaluationStep();
		
		float PenaltyNormalize(float fPenalty);
						  
		void WriteDebug(IDebugView::Slot eSlot,
						std::string sMessage);

		bool m_bCameraUpdate;

		Config m_oConfig;
		VistaPropertyList m_oCameraIntrinsics;

		ShaderRegistry *m_pShaderReg;
		
		HandGeometry *m_pHandGeometry;
		HandRenderer *m_pHandRenderer;

		unsigned char  *m_pColorBuffer;
		unsigned short *m_pDepthBuffer;
		float          *m_pUVMapBuffer;

		std::vector<float> m_vViewportData;

		IDebugView *m_pDebugView;

		GLuint m_idRenderedTexture;
		GLuint m_idRenderedTextureFBO;

		GLuint m_idCameraTexture;		
		GLuint m_idCameraTexturePBO;

		GLuint m_idGenerateTransformsProgram;

		GLuint m_idPrepareReductionTexturesProgram;

		GLuint m_idReduction0DifferenceProgram;
		GLuint m_idReduction1DifferenceProgram;
		GLuint m_idReduction2DifferenceProgram;
		GLuint m_idReduction0UnionProgram;
		GLuint m_idReduction1UnionProgram;
		GLuint m_idReduction2UnionProgram;
		GLuint m_idReduction0IntersectionProgram;
		GLuint m_idReduction1IntersectionProgram;
		GLuint m_idReduction2IntersectionProgram;

		GLuint m_idUpdateScoresProgram;
		GLuint m_idUpdateGBestProgram;
		GLuint m_idUpdateSwarmProgram;
		GLint m_locPhiCognitiveUniform;
		GLint m_locPhiSocialUniform;

		GLuint m_idDifferenceTexture;

		enum TextureIndex {
			DIFFERENCE,
			UNION,
			INTERSECTION
		};
		
		GLuint m_idReductionTextures320x256[3];
		GLuint m_idReductionTextures40x32[3];
		GLuint m_idReductionTextures5x4[3];
		GLuint m_idReductionTextures1x1[3];

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

		CameraFrameFilter *m_pFrameFilter;

		bool m_bTrackingEnabled;
		ParticleSwarm *m_pSwarm;

		HandModel *m_pHandModelLeft;
		HandModel *m_pHandModelRight;

		VistaRandomNumberGenerator *m_pRNG;
		GLint m_locRandomOffsetUniform;

		VistaBasicProfiler *m_pProfiler;
	};
}

#endif // _RHAPSODIES_HANDTRACKER
