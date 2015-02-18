#version 330 compatibility

uniform mat4 gl_ModelViewProjectionMatrix;
uniform mat4 model_transform;

layout(location = 0) in vec3 vertexPosition_modelspace;

void main(){
	gl_Position = gl_ModelViewProjectionMatrix *
		model_transform * vec4(vertexPosition_modelspace,1);
	// gl_Position = gl_ModelViewProjectionMatrix *
	// 	vec4(vertexPosition_modelspace,1);
}
