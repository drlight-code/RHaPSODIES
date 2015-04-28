#include <string>

#include "SkinClassifierRedMatter5.hpp"

namespace rhapsodies {
	std::string SkinClassifierRedMatter5::GetName() {
		return "SkinClassifierRedMatter5";
	}

	bool SkinClassifierRedMatter5::IsSkinPixel(const unsigned char* rgb) {
		int R = rgb[0];
		int G = rgb[1];
		int B = rgb[2];

		int sum = R+G+B;
		
		if( (double(G)/double(B) - double(R)/double(G) <= -0.0905) &&
			(double(G*sum)/double(B*(R-G)) >3.4857) &&
			(double(sum*sum*sum)/double(3*G*R*R) <= 7.397) &&
			(double(sum)/double(9*R) - 0.333 > -0.0976) )
			return true;

		return false;
	}
}
