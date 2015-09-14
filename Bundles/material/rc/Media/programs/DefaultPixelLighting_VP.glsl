#version 150

uniform mat4 u_worldViewProj;
uniform mat4 u_world;
uniform mat4 u_normalMatrix;

in vec4 position;
in vec3 normal;
in vec2 uv0;

out vec3 oPosition_WS;
out vec3 oNormal_WS;
out vec2 oTexCoord;

void main(void)
{
    gl_Position = u_worldViewProj * position;
    oPosition_WS = (u_world * position).xyz;
    oNormal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
    oTexCoord = uv0;
}
