#version 330

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "DepthPeelingCommon_FP.inc.glsl"

out vec4 FragColor;

vec4 packFloatToVec4(float value);

void main()
{
    FragColor = packFloatToVec4(1.0);
}
