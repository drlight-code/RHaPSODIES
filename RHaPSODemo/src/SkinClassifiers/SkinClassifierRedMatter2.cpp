#include "SkinClassifierRedMatter2.hpp"

namespace rhapsodies {
	std::string SkinClassifierRedMatter2::GetName() {
		return "SkinClassifierRedMatter2";
	}

	bool SkinClassifierRedMatter2::IsSkinPixel(const unsigned char* rgb) {
		int R = rgb[0];
		int G = rgb[1];
		int B = rgb[2];

		int sum = R+G+B;
		
		if( (double(3*B*R*R) / double(sum*sum*sum) > 0.1276) &&
			(double(R*B+G*G) / double(G*B) > 2.14) &&
			(double(sum)/double(3*R)+double(R-G)/double(sum) < 2.7775) )
			return true;

		return false;

	}
}
