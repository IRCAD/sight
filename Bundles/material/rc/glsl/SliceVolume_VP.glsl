#version 150

uniform mat4 u_worldViewProj;

#ifdef PREINTEGRATION
uniform vec3 u_cameraPos;
#endif

in vec4 position;
in vec3 uv0;

out vec3 vs_uvw;

#ifdef PREINTEGRATION
out vec3 vs_cameraDir;
#endif

void main(void)
{
    vs_uvw = uv0;

#ifdef PREINTEGRATION
    vs_cameraDir = vec3(position) - u_cameraPos;
#endif

    gl_Position = u_worldViewProj * position;
}
