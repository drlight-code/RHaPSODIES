#version 430 compatibility

out int instance_id;

const int ARRAY_SIZE = 2816; // 64*2*22

layout(std430, binding = 10) buffer TransformBlock {
  mat4 model_transform[ARRAY_SIZE];
};

layout(location=0) in vec3 vertexPosition_modelspace;

void main(){
	instance_id = gl_InstanceID;
	gl_Position = gl_ModelViewProjectionMatrix *
		model_transform[gl_InstanceID] * vec4(vertexPosition_modelspace,1);
}
