#version 410

uniform mat4 u_worldViewProj;

// camera position in object space
//uniform vec3 u_cameraPos;

in vec4 position;
//in vec3 uv0;

void main(void)
{
    gl_Position = u_worldViewProj * position;
}
