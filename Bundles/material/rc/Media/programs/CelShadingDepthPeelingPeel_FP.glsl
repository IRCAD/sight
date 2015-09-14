#version 330
uniform sampler2D u_bufferDepth;
uniform float u_vpWidth;
uniform float u_vpHeight;
uniform vec4 u_diffuseColor;
in vec3 normal_VS;

#ifndef EDGE

#ifdef PIXEL_LIGHTING
in vec3 oPosition_WS;
in vec3 oNormal_WS;
in vec3 oLight_WS;

vec4 lighting(vec3 _normal, vec3 _position);
#else

#ifdef FLAT
flat in vec4 oColor;
#else
in vec4 oColor;
#endif // FLAT

#endif // PIXEL_LIGHTING

vec4 fetch_texture();

#else

vec4 oColor = vec4(0,0,0,u_diffuseColor.a);

#endif // EDGE

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
    float frontDepthBuffer = texture(u_bufferDepth, texCoord).r;

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
        if( currentDepth <= frontDepthBuffer || u_diffuseColor.a == 0.)
        {
            discard;
        }

        // Depth sent to the next peel
        bufferDepth.r = currentDepth;

#ifdef PIXEL_LIGHTING
        vec4 colorOut = lighting(normalize(oNormal_WS), oPosition_WS);
#else
        vec4 colorOut = oColor;
#endif
        colorOut.rgb = computeCelShading(colorOut.rgb);
#ifndef EDGE
        colorOut *= fetch_texture();
#endif

        colorOut.rgb *= colorOut.a;
        bufferColorRG.r = packColor(colorOut.rg);
        bufferColorBA.r = packColor(colorOut.ba);

        bufferNormal.r = packNormal(normalize(normal_VS) * 0.5 + 0.5);
    }
}
