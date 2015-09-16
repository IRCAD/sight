#version 330
in vec3 normal_VS;
uniform vec4 u_diffuse;

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

/** Init shader :
  * Draw a front render of object
  **/
void main()
{
    if( u_diffuse.a == 0.)
    {
        discard;
    }
    // Draw front color buffer
#ifdef PIXEL_LIGHTING
    vec4 colorOut = lighting(normalize(oNormal_WS), oPosition_WS);
#else
    vec4 colorOut = oColor;
#endif
    colorOut.rgb = computeCelShading(colorOut.rgb);

    colorOut.rgb *= colorOut.a;
    colorOut.a =  1.-colorOut.a;
    bufferColorRG.r = packColor(colorOut.rg);
    bufferColorBA.r = packColor(colorOut.ba);

    // Empty target peel depth buffer
    bufferDepth.r = gl_FragCoord.z;
    bufferNormal.r = packNormal(normalize(normal_VS) * 0.5 + 0.5);
}
