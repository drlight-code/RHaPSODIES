#ifndef _RHAPSODIES_SKINCLASSIFIERREDMATTER1
#define _RHAPSODIES_SKINCLASSIFIERREDMATTER1

#include "SkinClassifier.hpp"

namespace rhapsodies {
	class SkinClassifierRedMatter1 : public SkinClassifier {
	public:
		std::string GetName();
		bool IsSkinPixel(const unsigned char* rgb);

	private:
	};
}

#endif // _RHAPSODIES_SKINCLASSIFIERREDMATTER1
