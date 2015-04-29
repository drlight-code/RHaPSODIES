#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int instances_per_viewport;
in int instance_id[];

out int gl_ViewportIndex;
				  
void main() {
	gl_ViewportIndex = instance_id[0] / instances_per_viewport;
	for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}
