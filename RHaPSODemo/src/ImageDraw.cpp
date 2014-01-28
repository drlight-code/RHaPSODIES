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

#include <VistaKernel/GraphicsManager/VistaSceneGraph.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/GraphicsManager/VistaOpenGLNode.h>

#include <ImagePBOOpenGLDraw.hpp>

#include "ImageDraw.hpp"

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

namespace rhapsodies {
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
	ImageDraw::ImageDraw(VistaGroupNode *pParentNode,
						 VistaSceneGraph *pSG,
						 ShaderRegistry *pShaderReg,
						 int width, int height) {
		m_pTransform = pSG->NewTransformNode(pParentNode);
		m_pOGLDraw = new ImagePBOOpenGLDraw(width, height,
											pShaderReg);
		m_pOGLNode = pSG->NewOpenGLNode(m_pTransform, 
										m_pOGLDraw);
		m_pTransform->AddChild(m_pOGLNode);
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	ImagePBOOpenGLDraw *ImageDraw::GetImagePBODraw() {
		return m_pOGLDraw;
	}

	VistaTransformNode *ImageDraw::GetTransformNode() {
		return m_pTransform;
	}
}
