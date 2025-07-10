#version 450

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#if defined(FLAT) || defined(PHONG)
#include "Lighting.inc.glsl"
#endif

// This defines getFragmentColor() and getFragmentAlpha()
#include "MaterialColor_FP.inc.glsl"

#include "Transparency.inc.glsl"

void main(void)
{
    processFragment();
}
