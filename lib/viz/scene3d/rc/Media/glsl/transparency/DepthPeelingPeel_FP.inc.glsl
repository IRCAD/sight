// Uniforms
layout(binding=2) uniform sampler2D u_nearestDepthBuffer;
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

    // Front depth buffer (stored as R32F)
    float frontDepthBuffer = texture(u_nearestDepthBuffer, texCoord).r;

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

        // Depth sent to the next peel (R channel of PF_FLOAT32_R)
        frontDepth = vec4(currentDepth, 0.0, 0.0, 1.0);
        frontColor = colorOut;
    }
}
