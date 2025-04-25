#ifdef HYBRID

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "DepthPeelingCommon_FP.inc.glsl"

uniform sampler2D u_frontDepthBuffer;
#endif

uniform sampler2D u_occlusionDepthBuffer;

uniform vec4 u_viewport;

out vec4 FragColor;

void process()
{
    vec2 texCoord = gl_FragCoord.xy * u_viewport.zw;

#ifdef HYBRID
    float frontDepthBuffer = unpackFloatFromVec4(texture(u_frontDepthBuffer, texCoord));
#endif
    vec4 occlusionDepthBuffer = texture(u_occlusionDepthBuffer, texCoord);
    float currentDepth = gl_FragCoord.z;

#ifdef HYBRID
    if(frontDepthBuffer == 0. || currentDepth <= frontDepthBuffer || currentDepth > occlusionDepthBuffer.r)
#else
    if(currentDepth > occlusionDepthBuffer.r)
#endif
    {
        discard;
    }

    FragColor.r = getFragmentAlpha();
}
