#include <string>

#include "SkinClassifierRedMatter3.hpp"

namespace rhapsodies {
	std::string SkinClassifierRedMatter3::GetName() {
		return "SkinClassifierRedMatter3";
	}

	bool SkinClassifierRedMatter3::IsSkinPixel(const unsigned char* rgb) {
		int R = rgb[0];
		int G = rgb[1];
		int B = rgb[2];

		int sum = R+G+B;

		// @todo G/G is obviously a bug. have to try other
		// combinations or email author (Maxim Barsuk). *probably* it
		// is R/G since we are interested in the red hue range
		if( (double(G) / double(G) - double(R) / double(B) <= -0.0905) &&
			(double(sum) / double(3*R) + double(R - G) / double(sum) <= 0.9498) )
			return true;

		return false;		
	}
}
