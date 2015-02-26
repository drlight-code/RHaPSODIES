#version 330 compatibility

uniform mat4 gl_ModelViewProjectionMatrix;

const int ARRAY_SIZE = 2816; // 64*2*22

// The name of the block is used for finding the index location only
layout (std140) uniform TransformBlock {
  mat4 model_transform[ARRAY_SIZE];
};

layout(location = 0) in vec3 vertexPosition_modelspace;

void main(){
	gl_Position = gl_ModelViewProjectionMatrix *
		model_transform[gl_InstanceID] * vec4(vertexPosition_modelspace,1);
	// gl_Position = gl_ModelViewProjectionMatrix *
	// 	vec4(vertexPosition_modelspace,1);
}
