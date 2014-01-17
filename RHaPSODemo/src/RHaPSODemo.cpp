/*============================================================================*/
/*                  Copyright (c) 2014 RWTH Aachen University                 */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id: $

#include <VistaTools/VistaProfiler.h>
#include <VistaKernel/VistaSystem.h>

#include "RHaPSODemo.hpp"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
namespace {
  const std::string gRDIniFile = "configfiles/rhapsodemo.ini";
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

namespace rhapsodies {
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
  RHaPSODemo::RHaPSODemo() :
    m_pSystem(new VistaSystem) {
  }

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

  bool RHaPSODemo::Initialize(int argc, char** argv) {
    bool success = true;

    VistaProfiler oProf;

    // read the ini file names from dispatch ini
    m_pSystem->SetIniFile(
      oProf.GetTheProfileString("APPLICATION", "MAININI",
                                "vista.ini",
                                gRDIniFile) );
    m_pSystem->SetDisplayIniFile(
      oProf.GetTheProfileString("APPLICATION", "DISPLAYINI",
                                "vista.ini",
                                gRDIniFile) );
    m_pSystem->SetClusterIniFile(
      oProf.GetTheProfileString("APPLICATION", "CLUSTERINI",
                                "vista.ini",
                                gRDIniFile) );
    m_pSystem->SetInteractionIniFile(
      oProf.GetTheProfileString("APPLICATION", "INTERACTIONINI",
                                "vista.ini",
                                gRDIniFile) );


    if(!m_pSystem->Init(argc, argv)) {
      success = false;
    }

    return success;
  }

  bool RHaPSODemo::Run() {
    return m_pSystem->Run();
  }

}
