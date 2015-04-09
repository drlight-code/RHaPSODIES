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
		TexturedQuadGLDraw(0, false, true, pShaderReg),
		m_pboIndex(0),
		m_texWidth(width),
		m_texHeight(height),
		m_texUpdate(false) {
		unsigned char *texData =
			new unsigned char[m_texWidth*m_texHeight*3];

		// fill with raster pattern
		int raster_offset = 10;
		for(int i = 0; i < m_texWidth*m_texHeight*3; i++) {
			int px = i/3;
			if( i%3 == 0 && 
				(px%m_texWidth%raster_offset == 0 || 
				 px/m_texWidth%raster_offset == 0) )
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

		glBindTexture(GL_TEXTURE_2D, m_texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
					 m_texWidth, m_texHeight, 0, GL_RGB,
					 GL_UNSIGNED_BYTE, 0);

		m_pPBO = glMapBuffer(GL_PIXEL_UNPACK_BUFFER,
		   					 GL_WRITE_ONLY);

		glTexParameteri(GL_TEXTURE_2D,
						GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,
						GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	ImagePBOOpenGLDraw::~ImagePBOOpenGLDraw() {
	}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
	bool ImagePBOOpenGLDraw::Do() {
		if(m_texUpdate) {
			m_texUpdate = false;
			UpdateTexture();
		}
		return TexturedQuadGLDraw::Do();
	}

	
	void ImagePBOOpenGLDraw::UpdateTexture() {
		glBindTexture(GL_TEXTURE_2D, m_texId);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,
					 m_pboIds[m_pboIndex]);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		glTexSubImage2D(GL_TEXTURE_2D, 0, 
						0, 0, m_texWidth, m_texHeight,
						GL_RGB, GL_UNSIGNED_BYTE, 0);

		m_pboIndex = 1-m_pboIndex;
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER,
					 m_pboIds[m_pboIndex]);
		m_pPBO = glMapBuffer(GL_PIXEL_UNPACK_BUFFER,
							 GL_WRITE_ONLY);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	bool ImagePBOOpenGLDraw::FillPBOFromBuffer(const unsigned char* pData,
											   int width, int height) {
		// we assume RGB888 memory layout here!
		memcpy(m_pPBO, pData, width*height*3);
		m_texUpdate = true;

		return true;
	}
}
