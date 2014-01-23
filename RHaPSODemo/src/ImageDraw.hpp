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

class VistaMutex;

namespace rhapsodies {
	class ShaderRegistry;

	class ImageDraw : public IVistaOpenGLDraw {
		GLuint m_vaId;

		GLuint m_vbVertId;
		GLuint m_vbUVId;

		GLuint m_pboIds[2];
		unsigned char m_pboIndex;
		void *m_pPBO;

		GLuint m_texId;
		unsigned int m_texWidth;
		unsigned int m_texHeight;
		bool m_texUpdate;

		ShaderRegistry *m_pShaderReg;

		VistaMutex *m_pDrawMutex;

	public:
		ImageDraw(int width, int height,
				  ShaderRegistry *pShaderReg);
		~ImageDraw();

		virtual bool Do();
		virtual bool GetBoundingBox(VistaBoundingBox &bb);

		bool FillPBOFromBuffer(const void*,
							   int width, int height);
  };
}

#endif // _RHAPSODIES_IMAGERENDERNODE
