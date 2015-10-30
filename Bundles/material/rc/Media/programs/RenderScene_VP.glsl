#version 330

uniform mat4 u_worldViewProj;
uniform mat4 u_world;
uniform mat4 u_normalMatrix;

in vec4 position;
in vec3 normal;
in vec2 uv0;

#ifdef VERTEX_COLOR
in vec4 colour;
#endif

#ifdef CEL_SHADING
out vec3 normal_VS;
#endif

out vec2 oTexCoord;

#ifdef PIXEL_LIT
out vec3 oPosition_WS;
out vec3 oNormal_WS;
out vec4 oColor;
#else
#   ifdef FLAT
flat out vec4 oColor;
#   else
out vec4 oColor;
#   endif // FLAT

vec4 lighting(vec3 _normal, vec3 _position);
#endif // PIXEL_LIT

void main(void)
{
    gl_Position = u_worldViewProj * position;
#ifdef CEL_SHADING
    // TODO: merge with oNormal_WS
    normal_VS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
#endif

#ifdef PIXEL_LIT
    oPosition_WS = (u_world * position).xyz;
    oNormal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;

#ifdef VERTEX_COLOR
    oColor = colour;
#else
    oColor = vec4(1.,1.,1.,1.);
#endif

#else
    vec3 position_WS = (u_world * position).xyz;
    vec3 normal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
    oColor = lighting(normal_WS, position_WS);

#ifdef VERTEX_COLOR
    oColor *= colour;
#endif

#endif // PIXEL_LIT

    oTexCoord = uv0;
}
