#version 150

uniform mat4 u_worldViewProj;

in vec4 position;
in vec2 uv0;

out vec2 oTexCoord;

void main(void)
{
    gl_Position = u_worldViewProj * position;
    oTexCoord = uv0;
}
