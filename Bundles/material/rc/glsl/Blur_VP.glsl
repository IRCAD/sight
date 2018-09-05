#version 330

in vec4 vertex;
in vec2 uv0;

out vec2 texCoord;

uniform mat4 worldViewProj;

//----------------------------------------------

void main()
{
    gl_Position = worldViewProj * vertex;
    texCoord = uv0;
}