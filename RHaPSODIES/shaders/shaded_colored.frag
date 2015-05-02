#version 430 compatibility

uniform vec3 color_in;

in vec3 position_viewspace;
in vec3 normal_viewspace;

out vec4 color;

vec4 ShadeFragmentPhong(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor);
vec4 ShadeFragmentGooch(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor);

uniform float	u_fAlpha = 0.25;
uniform float	u_fBeta	 = 0.5;

uniform vec3	u_v3LightDir	= normalize( vec3( -0.5, -0.5, -0.5 ) );

float g_fb = 1.0-u_fAlpha;
float g_fy = 1.0-u_fBeta;

void main(){
	color = ShadeFragmentGooch(normalize(normal_viewspace),
						  position_viewspace,
						  vec4(color_in, 1));
//	color.rgb = normalize(normal_viewspace) * 0.5 + vec3(0.5);
}


vec4 ShadeFragmentGooch(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor)
{
	vec3 v3CoolColor	= u_fAlpha * v4MatColor.rgb + vec3(  0.0,  0.0, g_fb );
	vec3 v3WarmColor	= u_fBeta  * v4MatColor.rgb + vec3( g_fy, g_fy,  0.0 );	

	float	fNDotL		= dot( u_v3LightDir, v3Normal );
	vec3	v3Reflect 	= reflect( u_v3LightDir, v3Normal );
	float	fRDotP		= dot( v3Reflect, normalize(-v3Position));
	float	fSpecular	= pow( max( fRDotP, 0.0 ), 32.0 );

	vec4 v4Color;
	v4Color.rgb  = mix( v3WarmColor, v3CoolColor, ((1.0+fNDotL)/2.0) );
	v4Color.rgb += 0.5 * fSpecular;
	v4Color.a	 = v4MatColor.a;

	return v4Color;
}


vec4 ShadeFragmentPhong(vec3 v3Normal, vec3 v3Position, vec4 v4MatColor)
{
	vec3 v3Ambient  = vec3(0.0);
	vec3 v3Diffuse   = vec3(0.0);
	vec3 v3Specular = vec3(0.0);
	for(int i = 0; i<gl_MaxLights; ++i)
	{
		vec3 v3LightDir;
		if( gl_LightSource[i].position.w == 0 )
		{
			v3LightDir = normalize(gl_LightSource[i].position.xyz);
		}
		else
		{
			v3LightDir = normalize(gl_LightSource[i].position.xyz-v3Position);
		}

		float fNDotL    = dot( v3LightDir, v3Normal );
		vec3  v3Reflect = reflect( -v3LightDir, v3Normal );
		float fRDotP    = dot( v3Reflect, normalize(-v3Position) );
		float fSpecular = pow( max( fRDotP, 0.0 ), 32.0 );

		v3Ambient += gl_LightSource[i].ambient.rgb;
		v3Diffuse  += gl_LightSource[i].diffuse.rgb  * max( fNDotL, 0 );
		v3Specular+= gl_LightSource[i].specular.rgb * fSpecular;

	}
	
	vec4 v4Color;
	v4Color.rgb  = v4MatColor.rgb * (v3Ambient + v3Diffuse);
	v4Color.rgb += v3Specular;
	v4Color.a    = v4MatColor.a;

	return v4Color;
}

