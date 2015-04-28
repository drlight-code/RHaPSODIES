#include <string>

#include "SkinClassifierRedMatter4.hpp"

namespace rhapsodies {
	std::string SkinClassifierRedMatter4::GetName() {
		return "SkinClassifierRedMatter4";
	}

	bool SkinClassifierRedMatter4::IsSkinPixel(const unsigned char* rgb) {
		int R = rgb[0];
		int G = rgb[1];
		int B = rgb[2];

		int sum = R+G+B;

		if( (double(B) / double(G) < 1.249) &&
			(double(sum)/double(3*R) > 0.696) &&
			(0.3333-double(B)/double(sum) > 0.014) &&
			(double(G)/double(3*sum) < 0.108) )
			return true;

		return false;
	}
}
