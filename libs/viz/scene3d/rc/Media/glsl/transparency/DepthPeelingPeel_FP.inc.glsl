#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "DepthPeelingCommon_FP.inc.glsl"

// Uniforms
uniform sampler2D u_nearestDepthBuffer;
uniform vec4 u_viewport;

// Output render targets
layout(location = 0) out vec4 frontColor;
layout(location = 1) out vec4 frontDepth;

/** Peel shader :
  * Only write fragment ahead from next peel
  **/
void process()
{
    vec2 texCoord = gl_FragCoord.xy * u_viewport.zw;

    // Front depth buffer
    float frontDepthBuffer = unpackFloatFromVec4(texture(u_nearestDepthBuffer, texCoord));

    float currentDepth = gl_FragCoord.z;

    if(frontDepthBuffer == 0.)
    {
        frontColor = vec4(0,0,0,0);
    }
    else
    {
        if(frontDepthBuffer >= 1.)
        {
            frontDepthBuffer = 0.;
        }

        vec4 colorOut = getFragmentColor();

        // Disallow repetition of last passes
        // Check if current fragment is the nearest from last front peel by depth comparaison
        // if (yes), draws fragment as current nearest peel
        if(currentDepth <= frontDepthBuffer || colorOut.a == 0.)
        {
            discard;
        }

        colorOut.rgb *= colorOut.a;

        // Depth sent to the next peel
        frontDepth = packFloatToVec4(currentDepth);
        frontColor = colorOut;
    }
}
