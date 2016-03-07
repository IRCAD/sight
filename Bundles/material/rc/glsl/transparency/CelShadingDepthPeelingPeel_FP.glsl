#version 410

// Uniforms
uniform sampler2D u_nearestDepthBuffer;
uniform float u_vpWidth;
uniform float u_vpHeight;
uniform vec4 u_diffuse;

// Input semantics
layout(location = 0) in vec3 inNormal_WS;

// Output render targets
layout(location = 0) out vec4 frontColor;
layout(location = 1) out vec4 frontDepth;
layout(location = 2) out vec4 bufferNormal;

// Extern functions
vec4 getFragmentColor();
vec4 packFloatToVec4(float value);
float unpackFloatFromVec4(vec4 value);
vec3 packNormal(vec3 normal);

vec3 computeCelShading(vec3 color)
{
    // TODO : share as parameter
    float celShadingFactor = 5.;
    return floor(pow(color,vec3(1./2.))*celShadingFactor)/celShadingFactor;
}

/** Peel shader :
  * Only write fragment ahead from next peel
  **/
void main()
{
    vec2 texCoord = gl_FragCoord.xy / vec2( u_vpWidth, u_vpHeight );

    // Front depth buffer
    float frontDepthBuffer = unpackFloatFromVec4(texture(u_nearestDepthBuffer, texCoord));

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

        // Depth sent to the next peel
        frontDepth = packFloatToVec4(currentDepth);
        frontColor = colorOut;

        bufferNormal.rgb = packNormal(normalize(inNormal_WS));
    }
}
