#version 330

uniform mat4 u_worldViewProj;
uniform mat4 u_world;
uniform mat4 u_normalMatrix;

in vec4 position;
in vec3 normal;
in vec2 uv0;

out vec3 normal_VS;
out vec2 oTexCoord;

#ifdef PIXEL_LIGHTING
out vec3 oPosition_WS;
out vec3 oNormal_WS;
#else
#ifdef FLAT
flat out vec4 oColor;
#else
out vec4 oColor;
#endif

vec4 lighting(vec3 _normal, vec3 _position);
#endif

void main(void)
{
    gl_Position = u_worldViewProj * position;
    normal_VS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
    oTexCoord = uv0;

#ifdef PIXEL_LIGHTING
    oPosition_WS = (u_world * position).xyz;
    oNormal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
#else
    vec3 position_WS = (u_world * position).xyz;
    vec3 normal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
    oColor = lighting(normal_WS, position_WS);
#endif
}
