#version 410

uniform mat4 u_worldViewProj;

// camera position in object space
uniform vec3 u_cameraPos;

in vec4 position;
in vec3 uv0;

out vec3 vs_uvw;

// Ray entry point and direction in object space.
out vec4 vs_rayEntryPoint;
out vec3 vs_cameraDir;

void main(void)
{
    vs_uvw = uv0;
    vs_cameraDir = vec3(position) - u_cameraPos;

    vs_rayEntryPoint = position;

    gl_Position = u_worldViewProj * position;
}
