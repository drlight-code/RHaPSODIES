/*============================================================================*/
/*                                 VistaFlowLib                               */
/*               Copyright (c) 1998-2011 RWTH Aachen University               */
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
// $Id: VtrShadowDecorator_frag.glsl 34582 2013-04-16 11:22:17Z sf458740 $
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform int u_iWidth;
uniform int u_iHeight;
uniform int u_iShadowSize;

uniform ivec2 u_v2ShadowDir;
uniform ivec2 u_v2Offset;

uniform vec4 u_v4TextColor;
uniform vec4 u_v4ShadowColor;

uniform sampler2D texSamplerTMU0;

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	vec2 v2Cood = gl_FragCoord.xy - u_v2Offset;
	
	vec4 v4Color = texture2D( texSamplerTMU0, vec2( v2Cood.x/u_iWidth, v2Cood.y/u_iHeight ) );
	float fAlpha = v4Color.a;

	v2Cood -= u_v2ShadowDir;
	
	float fSum = 0;
	for(int x = -u_iShadowSize+1; x < u_iShadowSize; ++x)
	{
		for(int y = -u_iShadowSize+1; y < u_iShadowSize; ++y)
		{
			if(x*x + y*y < u_iShadowSize*u_iShadowSize)
			{
				float fX = (v2Cood.x+x) / u_iWidth;
				float fY = (v2Cood.y+y) / u_iHeight;
				fSum += texture2D( texSamplerTMU0, vec2( fX, fY ) ).a;
			}
		}
	}
	
	fSum = clamp( fSum/(4*u_iShadowSize*u_iShadowSize), 0.0, 1.0 );
	
	v4Color = mix( u_v4ShadowColor*fSum, u_v4TextColor*v4Color, fAlpha );

	//gl_FragColor = vec4( gl_FragCoord.x/u_iWidth, gl_FragCoord.y/u_iHeight, 0.0f, 1.0f);
	gl_FragColor = v4Color;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
