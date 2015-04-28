#ifndef _RHAPSODIES_HANDGEOMETRY
#define _RHAPSODIES_HANDGEOMETRY

#include <cstddef>
#include <vector>

namespace rhapsodies {
  class HandGeometry {
    public:
	  HandGeometry();
	  
	  /**
	   * Extent indices are named as: FINGER_BONE
	   * 
	   * FINGER is one of:
	   * T (thumb)
	   * I (index)
	   * M (middle)
	   * R (ring)
	   * L (little)
	   *
	   * BONE is one of:
	   * MC (metacarpal)
	   * PP (proximal phalanx)
	   * MP (medial phalanx)
	   * DP (distal phalanx)
	   *
	   * @todo consider adding wrist/palm widths, bone diameters
	   */
	  enum Extent {
		  T_MC,
		  T_PP,
		  T_DP,
		  I_MC,
		  I_PP,
		  I_MP,
		  I_DP,
		  M_MC,
		  M_PP,
		  M_MP,
		  M_DP,
		  R_MC,
		  R_PP,
		  R_MP,
		  R_DP,
		  L_MC,
		  L_PP,
		  L_MP,
		  L_DP
	  };

	  /**
	   * Get a skeleton bone extent in millimeters.
	   * 
	   * @param eExt The bone extent to retreive.
	   * @return Length in millimeters.
	   */
	  float GetExtent(size_t eExt);
	  std::vector<float> &GetExtents();

	  /**
	   * Set a skeleton bone extent in millimeters.
	   * 
	   * @param eExt The bone extent to retreive.
	   * @param fLengthMm Length in millimeters.
	   */
	  void SetExtent(size_t eExt, float fLengthMm);

	  
    private:
	  std::vector<float> m_vecExtents;
  };
}

#endif // _RHAPSODIES_HANDGEOMETRY
