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
#ifdef R2VB
    vec3 oNormal;

#   ifdef VERTEX_COLOR
    vec4 oColor;
#   endif // VERTEX_COLOR

#else
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

    #   ifdef CEL_SHADING
    vec3 normal_VS;
    #   endif // CEL_SHADING

    #endif // PIXEL_LIT

#endif // R2VB

    #ifdef DIFFUSE_TEX
    vec2 oTexCoord;
    #endif // DIFFUSE_TEX
} vertexOut;

#ifndef NONE
vec4 lighting(vec3 _normal, vec3 _position);
#endif // NONE

void main(void)
{
#ifdef R2VB
    gl_Position = position;
    vertexOut.oNormal = normal;

#   ifdef VERTEX_COLOR
    vertexOut.oColor = colour;
#   endif // VERTEX_COLOR

#   ifdef DIFFUSE_TEX
    vertexOut.oTexCoord = uv0;
#   endif // DIFFUSE_TEX

#else

    gl_Position = u_worldViewProj * position;

#   ifdef CEL_SHADING
    // TODO: merge with oNormal_WS
    normal_VS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
#   endif

#   ifdef PIXEL_LIT
    vertexOut.oPosition_WS = (u_world * position).xyz;
    vertexOut.oNormal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;

#       ifdef VERTEX_COLOR

    // We could skip this division, however r2vb in Ogre doesn't allow us to output the color in the correct format.
    // So we use VET_UBYTE4 for both pipelines (r2vb/regular) to avoid a new #define.
    vertexOut.oColor = colour/255.f;
#       else
    vertexOut.oColor = vec4(1.,1.,1.,1.);
#       endif // VERTEX_COLOR

#   else
#       ifdef NONE
    vertexOut.oColor = vec4(1.,1.,1.,1.);
#       else
    vec3 position_WS = (u_world * position).xyz;
    vec3 normal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
    vertexOut.oColor = lighting(normal_WS, position_WS);
#       endif // NONE

#       ifdef VERTEX_COLOR
    vertexOut.oColor *= colour;
#       endif // VERTEX_COLOR

#   endif // PIXEL_LIT

#   ifdef DIFFUSE_TEX
    vertexOut.oTexCoord = uv0;
#   endif // DIFFUSE_TEX

#endif // R2VB

}
