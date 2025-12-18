
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
layout(location = 2) out vec4 bufferNormal;

vec3 computeCelShading(vec3 color)
{
    // TODO : share as parameter
    float celShadingFactor = 5.;
    return floor(pow(color,vec3(1./2.))*celShadingFactor)/celShadingFactor;
}

/** Peel shader :
  * Only write fragment ahead from next peel
  **/
void process()
{
    vec2 texCoord = gl_FragCoord.xy * u_viewport.zw;

    // Front depth buffer
    // Front depth buffer (stored as R32F)
    float frontDepthBuffer = texture(u_nearestDepthBuffer, texCoord).r;

    float currentDepth = gl_FragCoord.z;

    // Disallow repetition of last passes
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

        // Check if current fragment is the nearest from last front peel by depth comparaison
        // if (yes), draws fragment as current nearest peel
        if( currentDepth <= frontDepthBuffer || u_diffuse.a == 0.)
        {
            discard;
        }

        vec4 colorOut = getFragmentColor();
        colorOut.rgb = computeCelShading(colorOut.rgb);

        colorOut.rgb *= colorOut.a;

        // Depth sent to the next peel (R channel of PF_FLOAT32_R)
        frontDepth = vec4(currentDepth, 0.0, 0.0, 1.0);

        frontColor = colorOut;

        bufferNormal.rgb = packNormal(normalize(v_f3Normal_Ws));
    }
}
