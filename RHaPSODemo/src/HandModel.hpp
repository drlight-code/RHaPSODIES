#ifndef _RHAPSODIES_HANDMODEL
#define _RHAPSODIES_HANDMODEL

namespace rhapsodies {
  class HandModel {
	  std::vector<float> m_vecJointAngles;
	  VistaVector3D   m_vPosition;
	  VistaQuaternion m_qOrientation;

  public:
	  enum Joints {
		  
	  }
  };
}

#endif // _RHAPSODIES_HANDMODEL
