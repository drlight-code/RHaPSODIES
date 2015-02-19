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

#include <cstring>
#include <iostream>

#include <GL/glew.h>

#include <VistaMath/VistaBoundingBox.h>

#include <ShaderRegistry.hpp>

#include "TexturedQuadGLDraw.hpp"

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
	TexturedQuadGLDraw::TexturedQuadGLDraw(GLuint texId,
										   ShaderRegistry *pShaderReg) :
		m_pShaderReg(pShaderReg),
		m_texId(texId)
	{
		glGenVertexArrays(1, &m_vaId);
		glBindVertexArray(m_vaId);

		const GLfloat afCoords[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			1.0f,  1.0f, 0.0f
		};
		glGenBuffers(1, &m_vbVertId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbVertId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(afCoords), afCoords, GL_STATIC_DRAW);

		const GLfloat afUV[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
		};
		glGenBuffers(1, &m_vbUVId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbUVId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(afUV), afUV, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	TexturedQuadGLDraw::~TexturedQuadGLDraw() {
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	bool TexturedQuadGLDraw::Do() {
		glEnable(GL_CULL_FACE);

		glBindVertexArray(m_vaId);

		glBindTexture(GL_TEXTURE_2D, m_texId);
		glUseProgram(m_pShaderReg->GetProgram("textured"));

		glBindBuffer(GL_ARRAY_BUFFER, m_vbVertId);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbUVId);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	bool TexturedQuadGLDraw::GetBoundingBox(VistaBoundingBox &bb) {
		bb.SetBounds( VistaVector3D(-1.0, -1.0, 0.0),
					  VistaVector3D( 1.0,  1.0, 0.0) );
		return true;
	}
}
