#version 330

uniform mat4 u_worldViewProj;
uniform mat4 u_world;
uniform mat4 u_normalMatrix;

in vec4 position;
in vec3 normal;

#ifdef DIFFUSE_TEX
in vec2 uv0;
#endif // DIFFUSE_TEX

#ifdef VERTEX_COLOR
in vec4 colour;
#endif // VERTEX_COLOR

out VertexDataOut
{
    #ifdef PIXEL_LIT
    vec3 oPosition_WS;
    vec3 oNormal_WS;
    vec4 oColor;
    #else
    #   ifdef FLAT
    flat vec4 oColor;
    #   else
    vec4 oColor;
    #   endif // FLAT

    #ifdef CEL_SHADING
    vec3 normal_VS;
    #endif
    #endif // PIXEL_LIT

#ifdef DIFFUSE_TEX
    vec2 oTexCoord;
#endif // DIFFUSE_TEX
} vertexOut;

vec4 lighting(vec3 _normal, vec3 _position);

void main(void)
{
    gl_Position = u_worldViewProj * position;
#ifdef CEL_SHADING
    // TODO: merge with oNormal_WS
    normal_VS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
#endif

#ifdef PIXEL_LIT
    vertexOut.oPosition_WS = (u_world * position).xyz;
    vertexOut.oNormal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;

#ifdef VERTEX_COLOR
    vertexOut.oColor = colour;
#else
    vertexOut.oColor = vec4(1.,1.,1.,1.);
#endif // VERTEX_COLOR

#else
    vec3 position_WS = (u_world * position).xyz;
    vec3 normal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
    vertexOut.oColor = lighting(normal_WS, position_WS);

#ifdef VERTEX_COLOR
    vertexOut.oColor *= colour;
#endif // VERTEX_COLOR

#endif // PIXEL_LIT

#ifdef DIFFUSE_TEX
    vertexOut.oTexCoord = uv0;
#endif // DIFFUSE_TEX
}
