#version 430 core

in vec2 UV;
out vec4 color;

layout(binding=0) uniform usampler2D texSampler;

void main(){
  	uvec4 value = texture(texSampler, UV).rgba;
	uvec4 value_out = uvec4(value[0], value[0], 0, 0xffffffffu);
	
	color = vec4(value_out)/float(0xffffffffu);
}
