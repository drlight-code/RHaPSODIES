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
#include <VistaInterProcComm/Concurrency/VistaMutex.h>

#include <ShaderRegistry.hpp>

#include "ImagePBOOpenGLDraw.hpp"

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
	ImagePBOOpenGLDraw::ImagePBOOpenGLDraw(int width, int height,
						 ShaderRegistry *pShaderReg) :
		m_pShaderReg(pShaderReg),
		m_pDrawMutex(new VistaMutex),
		m_pboIndex(0),
		m_texUpdate(false),
		m_texWidth(width),
		m_texHeight(height)
	{
		glGenVertexArrays(1, &m_vaId);
		glBindVertexArray(m_vaId);

		unsigned char *texData =
			new unsigned char[m_texWidth*m_texHeight*3];

		// fill with raster pattern
		int raster_offset = 10;
		for(int i = 0; i < m_texWidth*m_texHeight*3; i++) {
			int px = i/3;
			if( i%3 == 0 && 
				(px%m_texWidth%raster_offset == 0 || 
				 px/m_texWidth%raster_offset==0) )
				texData[i] = 255;
		}

		glGenBuffers(2, m_pboIds);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,
					 m_pboIds[1-m_pboIndex]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER,
					 m_texWidth*m_texHeight*3,
					 0, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,
					 m_pboIds[m_pboIndex]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER,
					 m_texWidth*m_texHeight*3,
					 texData, GL_DYNAMIC_DRAW);
		delete [] texData;

		glGenTextures(1, &m_texId);

		m_pDrawMutex->Lock();

		glBindTexture(GL_TEXTURE_2D, m_texId);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
					 m_texWidth, m_texHeight, 0, GL_RGB,
					 GL_UNSIGNED_BYTE, 0);

		m_pPBO = glMapBuffer(GL_PIXEL_UNPACK_BUFFER,
		   					 GL_WRITE_ONLY);

		glTexParameteri(GL_TEXTURE_2D,
						GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,
						GL_TEXTURE_MIN_FILTER, GL_LINEAR/*_MIPMAP_LINEAR*/);
		//glGenerateMipmap(GL_TEXTURE_2D);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		m_pDrawMutex->Unlock();


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
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
		};
		glGenBuffers(1, &m_vbUVId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbUVId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(afUV), afUV, GL_STATIC_DRAW);
	}

	ImagePBOOpenGLDraw::~ImagePBOOpenGLDraw() {
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	bool ImagePBOOpenGLDraw::Do() {
		glEnable(GL_CULL_FACE);

		glBindTexture(GL_TEXTURE_2D, m_texId);

		m_pDrawMutex->Lock();
		if(m_texUpdate) {
			m_texUpdate = false;

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER,
						 m_pboIds[m_pboIndex]);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

			// glTexSubImage2D(GL_TEXTURE_2D, 0, 
			// 				0, 0, m_texWidth, m_texHeight,
			// 				GL_RGB, GL_UNSIGNED_BYTE, 0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
						 m_texWidth, m_texHeight, 0, GL_RGB,
						 GL_UNSIGNED_BYTE, 0);

			m_pboIndex = 1-m_pboIndex;
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER,
						 m_pboIds[m_pboIndex]);
			m_pPBO = glMapBuffer(GL_PIXEL_UNPACK_BUFFER,
								 GL_WRITE_ONLY);
		}
		m_pDrawMutex->Unlock();

		glUseProgram(m_pShaderReg->GetProgram("textured"));

		glBindBuffer(GL_ARRAY_BUFFER, m_vbVertId);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbUVId);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glUseProgram(0);
	}
	
	bool ImagePBOOpenGLDraw::GetBoundingBox(VistaBoundingBox &bb) {
		bb.SetBounds( VistaVector3D(-1.0, -1.0, 0.0),
					  VistaVector3D( 1.0,  1.0, 0.0) );
		return true;
	}

	bool ImagePBOOpenGLDraw::FillPBOFromBuffer(const void* pData,
									  int width, int height) {
		m_pDrawMutex->Lock();

		// we assume RGB888 memory layout here!
		memcpy(m_pPBO, pData, width*height*3);
		m_texUpdate = true;

		m_pDrawMutex->Unlock();
	}
}
