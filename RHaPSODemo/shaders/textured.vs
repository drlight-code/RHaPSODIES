#version 330 compatibility

uniform mat4 gl_ModelViewProjectionMatrix;
//mat4 mvp = mat4(1);

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vertexPosition_modelspace,1);
    UV = vertexUV;
}
