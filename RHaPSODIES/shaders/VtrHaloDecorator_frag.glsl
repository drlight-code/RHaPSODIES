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
// $Id: VtrHaloDecorator_frag.glsl 34582 2013-04-16 11:22:17Z sf458740 $
/*============================================================================*/
/* UNIFORM VARIABLES														  */
/*============================================================================*/
uniform int u_iWidth;
uniform int u_iHeight;
uniform int u_iHaloSize;

uniform vec4 u_v4TextColor;
uniform vec4 u_v4HaloColor;

uniform sampler2D texSamplerTMU0;

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/
void main(void)
{
	vec2 v2Cood = gl_FragCoord.xy - vec2(u_iHaloSize);
	
	float fAlpha = 0;
	for(int x = -u_iHaloSize+1; x < u_iHaloSize; ++x)
	{
		for(int y = -u_iHaloSize+1; y < u_iHaloSize; ++y)
		{
			if(x*x + y*y < u_iHaloSize*u_iHaloSize)
			{
				float fX = (v2Cood.x+x) / u_iWidth;
				float fY = (v2Cood.y+y) / u_iHeight;
				fAlpha += texture2D( texSamplerTMU0, vec2( fX, fY ) ).a;
			}
		}
	}
	
	fAlpha = clamp( fAlpha, 0.0, 1.0 );

	vec4 v4Color = texture2D( texSamplerTMU0, vec2( v2Cood.x/u_iWidth, v2Cood.y/u_iHeight ) );
	
	float f = 1 - v4Color.a;
	
	v4Color = mix( u_v4TextColor*v4Color, u_v4HaloColor, f );
	
	v4Color.a   *= fAlpha;
	//vec4 v4Color = vec4( gl_FragCoord.x/u_iWidth, gl_FragCoord.y/u_iHeight, 0.0f, 1.0f);
	gl_FragColor = v4Color;
}
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
