#version 150

uniform mat4 u_worldViewProj;
uniform mat4 u_world;
uniform mat4 u_normalMatrix;

in vec4 position;
in vec3 normal;
in vec2 uv0;
in vec4 colour;

out vec2 oTexCoord;
#ifdef FLAT
flat out vec4 oColor;
#else
out vec4 oColor;
#endif // FLAT


vec4 lighting(vec3 _normal, vec3 _position);

void main(void)
{
    gl_Position = u_worldViewProj * position;

    vec3 position_WS = (u_world * position).xyz;
    vec3 normal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;

    oColor = lighting(normal_WS, position_WS) * colour;
    oTexCoord = uv0;
}
