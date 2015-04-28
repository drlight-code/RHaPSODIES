#ifndef _RHAPSODIES_SKINCLASSIFIERREDMATTER5
#define _RHAPSODIES_SKINCLASSIFIERREDMATTER5

#include <string>

#include "SkinClassifier.hpp"

namespace rhapsodies {
	class SkinClassifierRedMatter5 : public SkinClassifier {
	public:
		std::string GetName();
		bool IsSkinPixel(const unsigned char* rgb);

	private:
	};
}

#endif // _RHAPSODIES_SKINCLASSIFIERREDMATTER5
