#version 430 compatibility

out int instance_id;

const int ARRAY_SIZE = 2816; // 64*2*22

layout(std430, binding = 10) buffer TransformBlock {
  mat4 model_transform[ARRAY_SIZE];
};

// uniform int instances_per_viewport;

layout(location=0) in vec3 vertexPosition_modelspace;

void main(){
    // AMD_vertex_shader_viewport_index should allow us to set the viewport
	// index directly in the vertex shader (on AMD only). However not even
	// that works with current catalyst driver (15.200).
	// gl_ViewportIndex = gl_InstanceID / instances_per_viewport;

	instance_id = gl_InstanceID;
	gl_Position = gl_ModelViewProjectionMatrix *
		model_transform[gl_InstanceID] * vec4(vertexPosition_modelspace,1);
}
