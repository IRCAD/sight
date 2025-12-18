#version 420

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

uniform sampler2D u_texture;

in vec4 oColor;
in vec2 oTexCoord;

#ifdef VERTEX_NORMAL
in vec3 oPosition_Ws;
in vec3 oNormal_Ws;
#include "Lighting.inc.glsl"
#endif // VERTEX_NORMAL

vec4 getFragmentColor()
{
    vec4 color = oColor;
#ifdef VERTEX_NORMAL
    color = lighting(normalize(oNormal_Ws), oPosition_Ws);
#endif // VERTEX_NORMAL

    return color * texture(u_texture, oTexCoord);
}

float getFragmentAlpha()
{
    return oColor.a * texture(u_texture, oTexCoord).a;
}

#include "Transparency.inc.glsl"

void main(void)
{
    processFragment();
}
