#ifndef _RHAPSODIES_SKINCLASSIFIERLOGOPPONENTYIQ
#define _RHAPSODIES_SKINCLASSIFIERLOGOPPONENTYIQ

#include "SkinClassifier.hpp"

namespace rhapsodies {
	class SkinClassifierLogOpponentYIQ : public SkinClassifier {
	public:
		std::string GetName();
		bool IsSkinPixel(const unsigned char* rgb);

	private:
	};
}

#endif // _RHAPSODIES_SKINCLASSIFIERLOGOPPONENTYIQ
