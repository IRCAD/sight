#version 450

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#ifdef PHONG
uniform mat4 u_world;
#endif

#if defined(PHONG) || defined(CEL_SHADING)
uniform mat4 u_normalMatrix;
#endif

#if defined(AUTOSTEREO) || defined(FLAT)
uniform mat4 u_worldView;
#endif

#ifdef AUTOSTEREO
uniform mat4 u_proj;
#else
uniform mat4 u_worldViewProj;
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

#   if defined(PHONG) || defined(CEL_SHADING)
layout(location = 0) out vec3 v_f3Normal_Ws;
#   endif

#   if defined(PHONG)
layout(location = 1) out vec3 v_f3Position_Ws;
#   elif defined(FLAT)
layout(location = 1) out vec3 v_f3Position_Vs;
#   endif

layout(location = 2) out vec4 v_f4Color;

#   ifdef DIFFUSE_TEX
layout(location = 3) out vec2 v_f2TexCoord;
#   endif // DIFFUSE_TEX
#endif // R2VB


#if defined(FLAT) || defined(PHONG)
vec4 lighting(vec3 _normal, vec3 _position);
#else
uniform vec4 u_f4LightAmbientCol;
uniform vec4 u_ambient;
uniform vec4 u_diffuse;
#endif // FLAT || PHONG

void main(void)
{
#ifdef R2VB
    gl_Position = position;
    vNormal = normal;

#   ifdef VERTEX_COLOR
    vColor = colour;
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

#   if defined(PHONG) || defined(CEL_SHADING)
    v_f3Normal_Ws = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
#   endif

#   if defined(PHONG)
    v_f3Position_Ws = (u_world * position).xyz;

#       ifdef VERTEX_COLOR
    v_f4Color = colour;
#       else
    v_f4Color = vec4(1.,1.,1.,1.);
#       endif // VERTEX_COLOR

#   else
#       ifdef FLAT
    v_f3Position_Vs = (u_worldView * position).xyz;
    v_f4Color = vec4(1.,1.,1.,1.);
#       else
    v_f4Color = vec4(u_f4LightAmbientCol.rgb * u_ambient.rgb + u_diffuse.rgb, u_diffuse.a);
#       endif

#       ifdef VERTEX_COLOR
    v_f4Color *= colour;
#       endif // VERTEX_COLOR

#   endif // PHONG || CEL_SHADING

#   ifdef DIFFUSE_TEX
    v_f2TexCoord = uv0;
#   endif // DIFFUSE_TEX

#endif // R2VB

}
