#include <cmath>

#include "SkinClassifierLogOpponentYIQ.hpp"

namespace rhapsodies {
	std::string SkinClassifierLogOpponentYIQ::GetName() {
		return "SkinClassifierLogOpponentYIQ";
	}
	
	bool SkinClassifierLogOpponentYIQ::IsSkinPixel(
		const unsigned char* rgb) {
		int R = rgb[0];
		int G = rgb[1];
		int B = rgb[2];

		// convert to log-opponent color space
		// I= L(G);
		double Rg = log(R) - log(G);
		double By = log(B) - (log(G) +log(R)) / 2.0 ;

		// Hue in log-opponent space
		int H = atan2(Rg,By) * (180.0 / 3.141592654);

		// Intensity in YIQ color space
		int I = int(0.5957*R - 0.2745*G - 0.3213*B);

		if(I >= 20 && I <= 90 && (H >= 100 && H <= 150))
			return true;

		return false;
	}
}
