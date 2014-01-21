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

#ifndef _RHAPSODIES_IMAGERENDERNODE
#define _RHAPSODIES_IMAGERENDERNODE

#include <GL/gl.h>

#include <VistaKernel/GraphicsManager/VistaOpenGLDraw.h>

namespace rhapsodies {
	class ShaderRegistry;

	class ImageDraw : public IVistaOpenGLDraw {
		GLuint m_vaId;
		GLuint m_vbVertId;
		GLuint m_vbUVId;

		GLuint m_texId;
		unsigned int m_texWidth;
		unsigned int m_texHeight;
		unsigned char *m_texData;

		ShaderRegistry *m_pShaderReg;

	public:
		ImageDraw(ShaderRegistry *pShaderReg);
		~ImageDraw();

		virtual bool Do();
		virtual bool GetBoundingBox(VistaBoundingBox &bb);
  };
}

#endif // _RHAPSODIES_IMAGERENDERNODE
