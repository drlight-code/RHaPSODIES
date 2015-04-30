#version 430 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D texSampler;

void main(){
  	color = texture( texSampler, UV ).rgba;
}
