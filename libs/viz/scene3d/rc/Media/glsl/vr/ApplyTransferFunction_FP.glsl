#version 150

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "TransferFunction.inc.glsl"

uniform sampler3D u_image;
uniform sampler1D u_s1TFTexture;
uniform vec3 u_f3TFWindow;

uniform float u_sliceDepth;
uniform float u_sampleDistance;

in vec2 uv;

out vec4 fragColor;

//-----------------------------------------------------------------------------

void main(void)
{
    vec4 sampledColor = sampleTransferFunction(texture(u_image, vec3(uv, u_sliceDepth)).r, u_s1TFTexture, u_f3TFWindow);
    sampledColor.a = -log(1-min(sampledColor.a, 0.999));
    fragColor = sampledColor;
}
