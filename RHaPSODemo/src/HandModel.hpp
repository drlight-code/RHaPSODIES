#ifndef _RHAPSODIES_HANDMODEL
#define _RHAPSODIES_HANDMODEL

#include <vector>

#include <VistaBase/VistaVector3D.h>
#include <VistaBase/VistaQuaternion.h>

namespace rhapsodies {
  class HandModel {
  public:
	  HandModel();

	  /**
	   * JointDOF indices are named as: FINGER_JOINT[_DIRECTION]
	   * 
	   * FINGER is one of:
	   * T (thumb)
	   * I (index)
	   * M (middle)
	   * R (ring)
	   * L (little)
	   *
	   * JOINT is one of:
	   * CMC (carpometacarpal)
	   * MCP (metacarpophalangeal)
	   * IP  (interphalangeal)
	   * PIP (proximal interphalangeal)
	   * DIP (distal interphalangeal)
	   *
	   * DIRECTION optional and one of:
	   * A (abduction)
	   * F (flexion)
	   */
	  enum JointDOF {
		  T_CMC_F,
		  T_CMC_A,
		  T_MCP,
		  T_IP,
		  I_MCP_F,
		  I_MCP_A,
		  I_PIP,
		  I_DIP,
		  M_MCP_F,
		  M_MCP_A,
		  M_PIP,
		  M_DIP,
		  R_MCP_F,
		  R_MCP_A,
		  R_PIP,
		  R_DIP,
		  L_MCP_F,
		  L_MCP_A,
		  L_PIP,
		  L_DIP,
		  JOINTDOF_LAST
	  };

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

	  enum HandType {
		  LEFT_HAND,
		  RIGHT_HAND
	  };

	  void SetPosition(const VistaVector3D &vPos);
	  VistaVector3D GetPosition();
	  
	  void SetOrientation(const VistaQuaternion &vPos);
	  VistaQuaternion GetOrientation();

	  void SetType(HandType eType);
	  HandType GetType();
	  
	  /**
	   * Get a skeleton joint angle in degrees.
	   * 
	   * @param eDOF The joint DOF to retreive.
	   * @return Angle in degrees.
	   */
	  float GetJointAngle(size_t eDOF);

	  /**
	   * Set a skeleton joint angle in degrees.
	   * 
	   * @param eDOF The joint DOF to set.
	   * @param fAngleDegrees Angle in degrees.
	   */
	  void SetJointAngle(size_t eDOF, float fAngleDegrees);

	  /**
	   * Get a skeleton bone extent in millimeters.
	   * 
	   * @param eExt The bone extent to retreive.
	   * @return Length in millimeters.
	   */
	  float GetExtent(size_t eExt);

	  /**
	   * Set a skeleton bone extent in millimeters.
	   * 
	   * @param eExt The bone extent to retreive.
	   * @param fLengthMm Length in millimeters.
	   */
	  void SetExtent(size_t eExt, float fLengthMm);

	  
	  /**
	   * Randomize the angular DOFs.
	   */
	  void Randomize();

	  
  private:
	  VistaVector3D   m_vPosition;
	  VistaQuaternion m_qOrientation;

	  HandType m_eType;

	  std::vector<float> m_vecJointAngles;
	  std::vector<float> m_vecExtents;
  };
}

#endif // _RHAPSODIES_HANDMODEL
