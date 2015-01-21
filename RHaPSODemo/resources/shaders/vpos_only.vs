#version 330 compatibility

uniform mat4 gl_ModelViewProjectionMatrix;

layout(location = 0) in vec3 vertexPosition_modelspace;

void main(){
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vertexPosition_modelspace,1);
}
