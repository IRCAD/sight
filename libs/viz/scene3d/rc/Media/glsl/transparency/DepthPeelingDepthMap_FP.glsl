#version 330

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "DepthPeelingCommon_FP.inc.glsl"

out vec4 FragColor;
uniform vec4 u_diffuse;

vec4 packFloatToVec4(float value);

void main()
{
    if(u_diffuse.a == 0.)
        discard;
    FragColor = packFloatToVec4(gl_FragCoord.z);
}
