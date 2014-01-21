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

#include <iostream>

#include <GL/glew.h>

#include <VistaMath/VistaBoundingBox.h>

#include <ShaderRegistry.hpp>

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
	ImageDraw::ImageDraw(ShaderRegistry *pShaderReg) :
		m_pShaderReg(pShaderReg) 
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

		glGenTextures(1, &m_texId);

		m_texWidth  = 128;
		m_texHeight = 128;
		m_texData = new unsigned char[m_texWidth*m_texHeight*3];

		for(int i = 0; i < m_texWidth*m_texHeight*3; i++) {
			m_texData[i] = i/3%128*2;
		}

		glBindTexture(GL_TEXTURE_2D, m_texId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
					 m_texWidth, m_texHeight, 0, GL_BGR,
					 GL_UNSIGNED_BYTE, m_texData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		float afUV[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};

		glGenBuffers(1, &m_vbUVId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbUVId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(afUV), afUV, GL_STATIC_DRAW);
	}

	ImageDraw::~ImageDraw() {
		delete m_texData;
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	bool ImageDraw::Do() {
		glUseProgram(m_pShaderReg->GetProgram("textured"));

		glDisable(GL_CULL_FACE);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbVertId);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbUVId);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	bool ImageDraw::GetBoundingBox(VistaBoundingBox &bb) {
		bb.SetBounds( VistaVector3D(-1.0, -1.0, 0.0),
					  VistaVector3D( 1.0,  1.0, 0.0) );
		return true;
	}
}
