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
layout(location = 0) out vec3 v_f3Normal_Ws;
#   endif

#   ifdef PIXEL_LIT
layout(location = 1) out vec3 v_f3Position_Ws;
#   endif

#   ifdef FLAT
layout(location = 2) flat out vec4 v_f4Color;
#   else
layout(location = 2) out vec4 v_f4Color;
#   endif // FLAT

#   ifdef DIFFUSE_TEX
layout(location = 3) out vec2 v_f2TexCoord;
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

#   if defined(PIXEL_LIT) || defined(CEL_SHADING)
    v_f3Normal_Ws = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
#   endif

#   ifdef PIXEL_LIT
    v_f3Position_Ws = (u_world * position).xyz;

#       ifdef VERTEX_COLOR
    v_f4Color = colour;
#       else
    v_f4Color = vec4(1.,1.,1.,1.);
#       endif // VERTEX_COLOR

#   else
#       ifdef AMBIENT
    v_f4Color = vec4(u_ambient.rgb + u_diffuse.rgb, u_diffuse.a);
#       else
    vec3 position_WS = (u_world * position).xyz;
    vec3 normal_WS = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
    v_f4Color = lighting(normal_WS, position_WS);
#       endif // AMBIENT

#       ifdef VERTEX_COLOR
    v_f4Color *= colour;
#       endif // VERTEX_COLOR

#   endif // PIXEL_LIT

#   ifdef DIFFUSE_TEX
    v_f2TexCoord = uv0;
#   endif // DIFFUSE_TEX

#endif // R2VB

}
