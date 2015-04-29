#version 430 core

in vec2 UV;
out vec4 color;

layout(binding=0) uniform usampler2D texSampler;

void main(){
  	uvec4 value = texture(texSampler, UV).rgba;
	color = vec4(value[0], value[0], 0, 0);
}
