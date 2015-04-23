#include <string>
#include <algorithm>

#include "SkinClassifierDhawale.hpp"

namespace rhapsodies {
	std::string SkinClassifierDhawale::GetName() {
		return "SkinClassifierDhawale";
	}

	bool SkinClassifierDhawale::IsSkinPixel(const unsigned char* rgb) {
		int R = rgb[0];
		int G = rgb[1];
		int B = rgb[2];

		int sum = R+G+B;
		int max = std::max( {R, G, B} );
		int min = std::min( {R, G, B} );
		
		if(R > 40 && G > 20 && B > 10 &&
		   (max - min > 10) &&
		   abs(R-G) > 10 && R > G && R > B)
			return true;

		
		return false;
	}
}
