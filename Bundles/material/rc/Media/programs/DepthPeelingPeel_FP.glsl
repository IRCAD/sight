#version 330
uniform sampler2D u_fragData0;
uniform float u_vpWidth;
uniform float u_vpHeight;
uniform vec4 u_diffuse;

#ifndef EDGE

#ifdef NEGATO

vec4 negato();

#else

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

#endif // NEGATO

#else

vec4 oColor = vec4(0,0,0,u_diffuse.a);

#endif // EDGE

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

    float currentDepth = gl_FragCoord.z;

    if(frontDepthBuffer == 0.)
    {
        frontColorRG.r = 0.;
        frontColorBA.r = 0.;
    }
    else
    {
        if(frontDepthBuffer == 1.)
        {
            frontDepthBuffer = 0.;
        }

        // Disallow repetition of last passes
        // Check if current fragment is the nearest from last front peel by depth comparaison
        // if (yes), draws fragment as current nearest peel
        if( currentDepth <= frontDepthBuffer || u_diffuse.a == 0.)
        {
            discard;
        }

#ifdef NEGATO
        vec4 colorOut = negato();
#else

#ifdef PIXEL_LIGHTING
        vec4 colorOut = lighting(normalize(oNormal_WS), oPosition_WS);
#else
        vec4 colorOut = oColor;
#endif
#ifndef EDGE
        colorOut *= fetch_texture();
#endif

#endif // NEGATO

        // Depth sent to the next peel
        frontDepth.r = currentDepth;

        colorOut.rgb *= colorOut.a;
        frontColorRG.r = packColor(colorOut.rg);
        frontColorBA.r = packColor(colorOut.ba);
    }
}
