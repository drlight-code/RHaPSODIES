#ifndef _RHAPSODIES_SKINCLASSIFIERREDMATTER3
#define _RHAPSODIES_SKINCLASSIFIERREDMATTER3

#include <string>

#include "SkinClassifier.hpp"

namespace rhapsodies {
	class SkinClassifierRedMatter3 : public SkinClassifier {
	public:
		std::string GetName();
		bool IsSkinPixel(const unsigned char* rgb);
		
	private:
	};
}

#endif // _RHAPSODIES_SKINCLASSIFIERREDMATTER3
