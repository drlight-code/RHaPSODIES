#include <cstdlib>

#include "SkinClassifierRedMatter0.hpp"

namespace rhapsodies {
	std::string SkinClassifierRedMatter0::GetName() {
		return "SkinClassifierRedMatter0";
	}

	bool SkinClassifierRedMatter0::IsSkinPixel(
		const unsigned char* rgb) {
		int R = rgb[0];
		int G = rgb[1];
		int B = rgb[2];

		int max_value = 0;
		int min_value = 255;

		for( int i = 0 ; i < 3 ; i++ ) {
			if(rgb[i] > max_value)
				max_value = rgb[i];
			if(rgb[i] < min_value)
				min_value = rgb[i];							   
		}

		if( ((R > 95) && (G > 40) && (B < 20) &&
			 (max_value-min_value > 15) &&
			 (abs(R-G) > 15) && (R > G) && (R > B)) ||
			((R > 220) && (G > 210) && (B > 170) && (abs(R - G) <= 15) &&
			 (R > B) && (G > B)) )
			return true;
		return false;
	}
}
