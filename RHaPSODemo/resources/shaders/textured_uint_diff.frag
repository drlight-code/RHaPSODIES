#version 430 core

in vec2 UV;
out vec4 color;

layout(binding=0) uniform usampler2D texSampler;

void main(){
  	float value = texture(texSampler, UV)[0] / float(0xffffu);
	color = vec4(value, value, value, 0);
}
