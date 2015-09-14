#version 330
uniform mat4 u_worldViewProj;
in vec4 vertex;

void main()
{
    gl_Position = u_worldViewProj * vertex;
}
