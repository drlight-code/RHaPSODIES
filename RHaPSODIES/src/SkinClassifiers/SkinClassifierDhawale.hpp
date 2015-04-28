#ifndef _RHAPSODIES_SKINCLASSIFIERDHAWALE
#define _RHAPSODIES_SKINCLASSIFIERDHAWALE

#include <string>

#include "SkinClassifier.hpp"

namespace rhapsodies {
	class SkinClassifierDhawale : public SkinClassifier {
	public:
		std::string GetName();
		bool IsSkinPixel(const unsigned char* rgb);

	private:
	};
}

#endif // _RHAPSODIES_SKINCLASSIFIERDHAWALE
