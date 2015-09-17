#version 330
uniform sampler2D u_fragData0;
uniform sampler2D u_fragData1;
uniform float u_vpWidth;
uniform float u_vpHeight;
uniform  vec4 u_diffuse;

vec4 getMaterialColor();

layout(location = 0) out vec4 frontColorRG;
layout(location = 1) out vec4 frontDepth;
layout(location = 2) out vec4 frontColorBA;

float packColor(vec2 color);

/** Peel shader :
  * Only write fragment ahead from next peel
  **/
void main()
{
    vec2 texCoord = gl_FragCoord.xy / vec2( u_vpWidth, u_vpHeight );

    // Front depth buffer
    float frontDepthBuffer = texture(u_fragData0, texCoord).r;
    float backDepthBuffer = texture(u_fragData1, texCoord).r;

    float currentDepth = gl_FragCoord.z;


    // Disallow repetition of last passes
    if(backDepthBuffer == frontDepthBuffer)
    {
        if(currentDepth == frontDepthBuffer)
        {
            frontColorRG.r = 0.;
            frontColorBA.r = 0.;
            frontDepth.r = currentDepth;
        }
        else
        {
            discard;
        }
    }
    else
    {
        // Check if current fragment is the nearest from last front peel by depth comparaison
        // if (yes), draws fragment as current nearest peel
        if(currentDepth <= frontDepthBuffer || u_diffuse.a == 0.)
        {
            discard;
        }

        vec4 colorOut = getMaterialColor();

        // Depth sent to the next peel
        frontDepth.r = currentDepth;

        colorOut.rgb *= colorOut.a;
        frontColorRG.r = packColor(colorOut.rg);
        frontColorBA.r = packColor(colorOut.ba);
    }
}
