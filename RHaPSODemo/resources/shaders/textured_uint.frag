#version 430 core

in vec2 UV;
out vec4 color;
layout(binding=0) uniform usampler2D texSampler;

void main(){
  	uvec4 value = texture(texSampler, UV).rgba;
	color = vec4(1,0,0,1) - vec4(value)/float(0xffffffffu);
}
