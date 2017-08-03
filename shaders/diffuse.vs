#version 330 core

layout(location = 0) in vec2 vertexPosition_screenspace;

void main()
{
    gl_Position.xyz = vec3(vertexPosition_screenspace, 0);
    gl_Position.w = 1.0;
}