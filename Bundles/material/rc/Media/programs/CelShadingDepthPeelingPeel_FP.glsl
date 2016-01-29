#version 330
uniform sampler2D u_nearestDepthBuffer;
uniform float u_vpWidth;
uniform float u_vpHeight;
uniform vec4 u_diffuse;
in vec3 normal_VS;

vec4 getMaterialColor();

layout(location = 0) out vec4 bufferColorRG;
layout(location = 1) out vec4 bufferDepth;
layout(location = 2) out vec4 bufferColorBA;
layout(location = 3) out vec4 bufferNormal;

float packColor(vec2 color);
float packNormal(vec3 normal);

vec3 computeCelShading(vec3 color)
{
    // TODO : share as parameter
    float celShadingFactor = 5.;
    return floor(pow(color,vec3(1./4.))*celShadingFactor)/celShadingFactor;
}

/** Peel shader :
  * Only write fragment ahead from next peel
  **/
void main()
{
    vec2 texCoord = gl_FragCoord.xy / vec2( u_vpWidth, u_vpHeight );

    // Front depth buffer
    float frontDepthBuffer = texture(u_nearestDepthBuffer, texCoord).r;

    float currentDepth = gl_FragCoord.z;

    // Disallow repetition of last passes
    if(frontDepthBuffer == 0.)
    {
        bufferColorRG.r = 0.;
        bufferColorBA.r = 0.;
    }
    else
    {
        if(frontDepthBuffer == 1.)
        {
            frontDepthBuffer = 0.;
        }

        // Check if current fragment is the nearest from last front peel by depth comparaison
        // if (yes), draws fragment as current nearest peel
        if( currentDepth <= frontDepthBuffer || u_diffuse.a == 0.)
        {
            discard;
        }

        // Depth sent to the next peel
        bufferDepth.r = currentDepth;

        vec4 colorOut = getMaterialColor();
        colorOut.rgb = computeCelShading(colorOut.rgb);

        colorOut.rgb *= colorOut.a;
        bufferColorRG.r = packColor(colorOut.rg);
        bufferColorBA.r = packColor(colorOut.ba);

        bufferNormal.r = packNormal(normalize(normal_VS) * 0.5 + 0.5);
    }
}
