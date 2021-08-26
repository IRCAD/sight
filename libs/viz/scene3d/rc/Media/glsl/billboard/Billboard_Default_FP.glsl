#version 410

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

uniform sampler2D u_texture;

in vec4 oColor;
in vec2 oTexCoord;

vec4 getFragmentColor()
{
    return oColor * texture(u_texture, oTexCoord);
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
