#ifndef _RHAPSODIES_SKINCLASSIFIERREDMATTER2
#define _RHAPSODIES_SKINCLASSIFIERREDMATTER2

#include <string>

#include "SkinClassifier.hpp"

namespace rhapsodies {
	class SkinClassifierRedMatter2 : public SkinClassifier {
	public:
		std::string GetName();
		bool IsSkinPixel(const unsigned char* rgb);
		
	private:
	};
}

#endif // _RHAPSODIES_SKINCLASSIFIERREDMATTER2
