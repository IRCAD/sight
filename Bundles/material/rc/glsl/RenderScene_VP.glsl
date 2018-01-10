#version 410

uniform mat4 u_worldViewProj;
uniform mat4 u_world;
uniform mat4 u_normalMatrix;

#ifdef AUTOSTEREO
uniform mat4 u_worldView;
uniform mat4 u_proj;
#endif // AUTOSTEREO

in vec4 position;
in vec3 normal;

#ifdef VERTEX_COLOR
in vec4 colour;
#endif // VERTEX_COLOR

#ifdef DIFFUSE_TEX
in vec2 uv0;
#endif // DIFFUSE_TEX

#ifdef R2VB

layout(location = 0) out vec3 vNormal;
#   ifdef VERTEX_COLOR
layout(location = 1) out vec4 vColor;
#   endif // VERTEX_COLOR
#   ifdef DIFFUSE_TEX
layout(location = 2) out vec2 vTexCoord;
#   endif // DIFFUSE_TEX

#else

#if defined(PIXEL_LIT) || defined(CEL_SHADING)
layout(location = 0) out vec3 outNormal_WS;
#   endif

#   ifdef PIXEL_LIT
layout(location = 1) out vec3 outPosition_WS;
#   endif

#   ifdef FLAT
flat layout(location = 2) out vec4 outColor;
#   else
layout(location = 2) out vec4 outColor;
#   endif // FLAT

#   ifdef DIFFUSE_TEX
layout(location = 3) out vec2 outTexCoord;
#   endif // DIFFUSE_TEX
#endif // R2VB


#ifndef AMBIENT
vec4 lighting(vec3 _normal, vec3 _position);
#else
uniform vec4 u_ambient;
uniform vec4 u_diffuse;
#endif // AMBIENT

void main(void)
{
#ifdef R2VB
    gl_Position = position;
    vNormal = normal;

#   ifdef VERTEX_COLOR
    vColor = colour/255.f;
#   endif // VERTEX_COLOR

#   ifdef DIFFUSE_TEX
    vTexCoord = uv0;
#   endif // DIFFUSE_TEX

#else

#ifdef AUTOSTEREO
    gl_Position = u_proj * u_worldView * position;
#else
    gl_Position = u_worldViewProj * position;
#endif

#   if defined(PIXEL_LIT) || defined(CEL_SHADING)
    outNormal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
#   endif

#   ifdef PIXEL_LIT
    outPosition_WS = (u_world * position).xyz;

#       ifdef VERTEX_COLOR

    // We could skip this division, however r2vb in Ogre doesn't allow us to output the color in the correct format.
    // So we use VET_UBYTE4 for both pipelines (r2vb/regular) to avoid a new #define.
    outColor = colour/255.f;
#       else
    outColor = vec4(1.,1.,1.,1.);
#       endif // VERTEX_COLOR

#   else
#       ifdef AMBIENT
    outColor = vec4(u_ambient.rgb + u_diffuse.rgb, u_diffuse.a);
#       else
    vec3 position_WS = (u_world * position).xyz;
    vec3 normal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
    outColor = lighting(normal_WS, position_WS);
#       endif // AMBIENT

#       ifdef VERTEX_COLOR
    outColor *= colour/255.f;
#       endif // VERTEX_COLOR

#   endif // PIXEL_LIT

#   ifdef DIFFUSE_TEX
    outTexCoord = uv0;
#   endif // DIFFUSE_TEX

#endif // R2VB

}
