#version 330
uniform sampler2D u_fragData0;
uniform sampler2D u_fragData1;
uniform float u_vpWidth;
uniform float u_vpHeight;
uniform  vec4 u_diffuseColor;

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
#endif
#endif

#else

vec4 oColor = vec4(0,0,0,u_diffuseColor.a);

#endif // EDGE

vec4 fetch_texture();

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
        if(currentDepth <= frontDepthBuffer || u_diffuseColor.a == 0.)
        {
            discard;
        }

#ifdef PIXEL_LIGHTING
        vec4 colorOut = lighting(normalize(oNormal_WS), oPosition_WS);
#else
        vec4 colorOut = oColor;
#endif
#ifndef EDGE
        colorOut *= fetch_texture();
#endif

        // Depth sent to the next peel
        frontDepth.r = currentDepth;

        colorOut.rgb *= colorOut.a;
        frontColorRG.r = packColor(colorOut.rg);
        frontColorBA.r = packColor(colorOut.ba);
    }
}
