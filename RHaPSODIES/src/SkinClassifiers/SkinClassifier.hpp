#ifndef _RHAPSODIES_SKINCLASSIFIER
#define _RHAPSODIES_SKINCLASSIFIER

#include <string>

namespace rhapsodies {
  class SkinClassifier {
  public:
	  virtual ~SkinClassifier() {};
	  
	  virtual std::string GetName()=0;
	  virtual bool IsSkinPixel(const unsigned char* rgb)=0;
  private:
  };
}

#endif // _RHAPSODIES_SKINCLASSIFIER
