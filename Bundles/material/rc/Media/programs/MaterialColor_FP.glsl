#version 150

#ifdef NEGATO

vec4 negato();

#else

#   ifdef PIXEL_LIT
vec4 lighting(vec3 _normal, vec3 _position);
#   endif // PIXEL_LIT

#   ifndef EDGE
uniform sampler2D u_texture;
#   endif // EDGE

in PixelDataIn
{

#   ifdef PIXEL_LIT

    vec3 oPosition_WS;
    vec3 oNormal_WS;
    vec4 oColor;

#   else

#       ifdef FLAT
    flat vec4 oColor;
#       else
    vec4 oColor;
#       endif // FLAT

#   endif // PIXEL_LIT

#   ifndef EDGE
    vec2 oTexCoord;
#   endif // EDGE

} pixelIn;


#endif // NEGATO

vec4 getMaterialColor()
{
#ifdef NEGATO
    vec4 colorOut = negato();
#else

#   ifdef PIXEL_LIT
        vec4 colorOut = lighting(normalize(pixelIn.oNormal_WS), pixelIn.oPosition_WS) * pixelIn.oColor;
#   else
        vec4 colorOut = pixelIn.oColor;
#   endif

#   ifndef EDGE
        colorOut *= texture(u_texture, pixelIn.oTexCoord);
#   endif

#endif // NEGATO
    return colorOut;
}
