#include <string>

#include "SkinClassifierRedMatter1.hpp"

namespace rhapsodies {
	std::string SkinClassifierRedMatter1::GetName() {
		return "SkinClassifierRedMatter1";
	}

	bool SkinClassifierRedMatter1::IsSkinPixel(const unsigned char* rgb) {
		int R = rgb[0];
		int G = rgb[1];
		int B = rgb[2];

		int sum = R+G+B;
		
		if( (double(R)/double(B) > 1.185) &&
			(double(R*B)/double(sum*sum) > 0.107) &&
			(double(R*G)/double(sum*sum) > 0.112) )
			return true;

		return false;
	}
}
