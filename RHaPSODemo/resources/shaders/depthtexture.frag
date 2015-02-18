#version 330 core

//in vec2 TexCoord0;

smooth in vec3 vNormal;
smooth in vec3 vWorldPos;
in mat4 ProjectionMatrix;

uniform sampler2D uDepthTex;
//uniform float uWidth;
//uniform float uHeight;

out vec4 color;

void main() {
//    vec2 res = gl_FragCoord.xy / vec2(uWidth, uHeight);
	vec2 res = gl_FragCoord.xy / vec2(320, 240);
    color = texture(uDepthTex, res);
}
