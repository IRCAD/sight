#version 410

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#ifdef PIXEL_LIT
#include "Lighting.inc.glsl"
#endif // PIXEL_LIT

// This defines getFragmentColor() and getFragmentAlpha()
#include "MaterialColor_FP.inc.glsl"

#include "Transparency.inc.glsl"

void main(void)
{
    processFragment();
}
