#ifndef _RHAPSODIES_SKINCLASSIFIERREDMATTER0
#define _RHAPSODIES_SKINCLASSIFIERREDMATTER0

#include <SkinClassifiers/SkinClassifier.hpp>

namespace rhapsodies {
	class SkinClassifierRedMatter0 : public SkinClassifier {
	public:
		std::string GetName();
		bool IsSkinPixel(const unsigned char* rgb);

	private:
	};
}

#endif // _RHAPSODIES_SKINCLASSIFIERREDMATTER0
