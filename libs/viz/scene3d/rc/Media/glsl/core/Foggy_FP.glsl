#version 410

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#ifdef PIXEL_LIT
#include "Lighting.inc.glsl"
#endif // PIXEL_LIT

// Uniforms
#ifdef DIFFUSE_TEX
uniform int u_useTextureAlpha;
#endif // DIFFUSE_TEX

// Input semantics
layout(location = 0) in vec3 v_f3Normal_Ws;

#ifdef PIXEL_LIT
layout(location = 1) in vec3 v_f3Position_Ws;
 #endif

#ifdef FLAT
layout(location = 2) flat in vec4 v_f4Color;
#   else
layout(location = 2) in vec4 v_f4Color;
#   endif // FLAT

layout(location = 3) in vec2 v_f2TexCoord;

#ifdef DIFFUSE_TEX
uniform sampler2D u_texture;
#endif // DIFFUSE_TEX

uniform vec2 u_fogParams;
uniform vec4 u_fogColor;
uniform int u_fog;

// Compute illumination or forward illumination received from the vertex shader
vec4 getFragmentColor()
{

#ifdef PIXEL_LIT
    vec4 colorOut = lighting(normalize(v_f3Normal_Ws), v_f3Position_Ws) * v_f4Color;
#else
    vec4 colorOut = v_f4Color;
#endif

#ifdef DIFFUSE_TEX
    vec4 colorTex = texture(u_texture, v_f2TexCoord);

    if(u_useTextureAlpha == 0)
    {
        colorTex.a = 1.0;
    }

    colorOut *= colorTex;
#endif // DIFFUSE_TEX

    // Fog computation
    float distance = gl_FragCoord.z/gl_FragCoord.w;

    float fogFactor = (u_fogParams.y - distance)/(u_fogParams.y - u_fogParams.x);
    fogFactor = clamp( fogFactor, 0.0, 1.0 );

    vec4 finalColor = mix(u_fogColor, colorOut, fogFactor);
    return finalColor;
}

#include "Transparency.inc.glsl"

void main(void)
{
    processFragment();
}
