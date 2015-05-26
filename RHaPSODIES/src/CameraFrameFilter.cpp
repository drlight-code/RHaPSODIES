#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "SkinClassifiers/SkinClassifierLogOpponentYIQ.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter0.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter1.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter2.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter3.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter4.hpp"
#include "SkinClassifiers/SkinClassifierRedMatter5.hpp"
#include "SkinClassifiers/SkinClassifierDhawale.hpp"

#include "CameraFrameFilter.hpp"

namespace {
	// float MapRangeExp(float value) {
	// 	// map range 0-1 exponentially
	// 	float base = 0.01;
	// 	float ret = (1 - pow(base, value))/(1-base);
		
	// 	// clamp
	// 	ret = ret < 0.0 ? 0.0 : ret;
	// 	ret = ret > 1.0 ? 1.0 : ret;

	// 	return ret;
	// }

	inline float WorldToScreenProjective(
		float zWorld, float zNear=0.1f, float zFar=1.1f) {
		return ((zNear + zFar - 2.0f*zNear*zFar/zWorld /
				 (zFar - zNear) + 1.0f) / 2.0f);
	}

	inline float WorldToScreenLinear(
		float zWorld, float zNear=0.1f, float zFar=1.1f) {
		return (zWorld - zNear) / (zFar - zNear);
	}
}

namespace rhapsodies {
	CameraFrameFilter::CameraFrameFilter(int iDilationSize,
										 int iErosionSize,
										 int iDepthLimit) :
		m_iDilationSize(iDilationSize),
		m_iErosionSize(iErosionSize),
		m_iDepthLimit(iDepthLimit) {

	}

	CameraFrameFilter::~CameraFrameFilter() {
		for(ListSkinCl::iterator it = m_lClassifiers.begin() ;
			it != m_lClassifiers.end() ; ++it) {
			delete *it;
		}
	}

	bool CameraFrameFilter::InitSkinClassifiers() {
		SkinClassifierLogOpponentYIQ *pSkinLOYIQ =
			new SkinClassifierLogOpponentYIQ;
		m_lClassifiers.push_back(pSkinLOYIQ);

		SkinClassifier *pSkinCl = new SkinClassifierRedMatter0;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter1;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter2;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter3;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter4;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierRedMatter5;
		m_lClassifiers.push_back(pSkinCl);

		pSkinCl = new SkinClassifierDhawale;
		m_lClassifiers.push_back(pSkinCl);
		
		m_itCurrentClassifier = --m_lClassifiers.end();

		return true;
	}

	void CameraFrameFilter::ProcessFrames(
		unsigned char  *colorFrame,
		unsigned short *depthFrame,
		float          *uvMapFrame) {

		UVMapToRGB(colorFrame,
				   depthFrame,
				   uvMapFrame,
				   m_pUVMapRGBBuffer);
		
		for(size_t pixel = 0 ; pixel < 76800 ; pixel++) {
			// don't filter the color map since it is considerably expensive
			// if( (*m_itCurrentClassifier)->IsSkinPixel(colorImage+3*pixel) ) {
			// 	// yay! skin!
			// }
			// else {
			// 	colorImage[3*pixel+0] = 0;
			// 	colorImage[3*pixel+1] = 0;
			// 	colorImage[3*pixel+2] = 0;
			// }
			
			if( (*m_itCurrentClassifier)->IsSkinPixel(
					m_pUVMapRGBBuffer+3*pixel) ) {
				m_pSkinMap[pixel] = 255;
			}
			else {
			 	m_pSkinMap[pixel] = 0;
			}
		}

		// dilate the skin map with opencv
		cv::Mat image = cv::Mat(240, 320, CV_8UC1, m_pSkinMap);
		cv::Mat image_processed;

		cv::Mat erode_element = getStructuringElement(
			cv::MORPH_ELLIPSE,
			cv::Size(m_iErosionSize, m_iErosionSize),
			cv::Point(m_iErosionSize/2));
		cv::Mat dilate_element = getStructuringElement(
			cv::MORPH_ELLIPSE,
			cv::Size(m_iDilationSize, m_iDilationSize),
			cv::Point(m_iDilationSize/2));

		cv::erode(image, image_processed, erode_element);
		image = image_processed.clone();
		cv::dilate(image, image_processed, dilate_element);

		// need to copy the frame since we write in different memory
		// layout than we read and would be corrupting the array
		// otherwise.
		unsigned short depthFrameCopy[320*240];
		memcpy(depthFrameCopy, depthFrame, 320*240*sizeof(unsigned short));
		
		short iDepthValue = 0x7fff;
		for(size_t pixel = 0 ; pixel < 76800 ; pixel++) {
			if(image_processed.data[pixel] == 0) {
				iDepthValue = 0x7fff;
			}
			else {
				// transform depth value range, in millimeters:
				// 100mm  -> 0
				// 1100mm -> ffffffff
				// @todo get rid of hard coding
				
				unsigned short zWorldMM = depthFrameCopy[pixel];
				float zScreen = 1.0f;

				// valid values [100,1100]
				if( zWorldMM >= 100 && zWorldMM <= 1100 ) {
//					zScreen = WorldToScreenProjective(float(zWorldMM)/1000.0f);
					zScreen = WorldToScreenLinear(float(zWorldMM)/1000.0f);
				}
				// we correct for a more or less static 10cm depth offset here
				// @todo get this right in accordance to cam specs!
				iDepthValue = (zScreen+0.1) * 0x7fff;
			}
			int targetRow = 240 - 1 - (pixel/320);
			int targetCol = pixel % 320;

			depthFrame[320*targetRow + targetCol] = iDepthValue;
		}
	}	

	void CameraFrameFilter::DepthToRGB(const unsigned short *depth,
									   unsigned char *rgb) {
		for(int i = 0 ; i < 76800 ; i++) {
			unsigned short val = depth[i];

			if(val > 0) {
				float linearvalue = val/2000.0;
//				float mappedvalue = MapRangeExp(linearvalue);
				float mappedvalue = linearvalue;

				rgb[3*i+0] = 255*(1-mappedvalue);
				rgb[3*i+1] = 255*(1-mappedvalue);
				rgb[3*i+2] = 0;
			}
			else {
				rgb[3*i+0] = 0;
				rgb[3*i+1] = 0;
				rgb[3*i+2] = 0;
			}
		}
	}

	void CameraFrameFilter::UVMapToRGB(
		const unsigned char *color,
		const unsigned short *depth,
		const float *uvmap,
		unsigned char *rgb) {

		int color_index_x, color_index_y, color_index;
		float invalid = -std::numeric_limits<float>::max();

		for(int i = 0 ; i < 76800 ; i++) {
			color_index_x = 320*uvmap[2*i+0];
			color_index_y = 240*uvmap[2*i+1];
			color_index = 320*color_index_y + color_index_x;

			if(uvmap[2*i+0] != invalid &&
			   uvmap[2*i+1] != invalid &&
			   depth[i] < m_iDepthLimit) {

				rgb[3*i+0] = color[3*color_index+0];
				rgb[3*i+1] = color[3*color_index+1];
				rgb[3*i+2] = color[3*color_index+2];
			}
			else {
				rgb[3*i+0] = 200;
				rgb[3*i+1] = 0;
				rgb[3*i+2] = 200;
			}
 		}
	}

	unsigned char* CameraFrameFilter::GetUVMapRGB() {
		return m_pUVMapRGBBuffer;
	}

	SkinClassifier *CameraFrameFilter::GetSkinClassifier() {
		return *m_itCurrentClassifier;
	}
	
	void CameraFrameFilter::NextSkinClassifier() {
		m_itCurrentClassifier++;
		if(m_itCurrentClassifier == m_lClassifiers.end())
			m_itCurrentClassifier = m_lClassifiers.begin();
	}

	void CameraFrameFilter::PrevSkinClassifier() {
		if(m_itCurrentClassifier == m_lClassifiers.begin())
			m_itCurrentClassifier = m_lClassifiers.end();
		m_itCurrentClassifier--;
	}
}
