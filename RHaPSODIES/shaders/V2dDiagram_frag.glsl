uniform sampler2D u_base;
uniform sampler2D u_lookup;
uniform vec2	  u_lookup_coords;
uniform vec4	  u_color;

/*============================================================================*/
/* SHADER MAIN																  */
/*============================================================================*/

void main()
{
	vec4 v4TexCol = texture2D(u_base, gl_TexCoord[0].xy);
	vec4 v4LookupCol = texture2D(u_lookup, u_lookup_coords);
	
	gl_FragColor = v4LookupCol * v4TexCol *	u_color;

}



	
/*============================================================================*/
/* END OF FILE																  */
/*============================================================================*/
