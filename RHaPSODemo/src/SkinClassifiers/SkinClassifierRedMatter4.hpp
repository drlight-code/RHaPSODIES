#ifndef _RHAPSODIES_SKINCLASSIFIERREDMATTER4
#define _RHAPSODIES_SKINCLASSIFIERREDMATTER4

#include <string>

#include "SkinClassifier.hpp"

namespace rhapsodies {
	class SkinClassifierRedMatter4 : public SkinClassifier {
	public:
		std::string GetName();
		bool IsSkinPixel(const unsigned char* rgb);
		
	private:
	};
}

#endif // _RHAPSODIES_SKINCLASSIFIERREDMATTER4
