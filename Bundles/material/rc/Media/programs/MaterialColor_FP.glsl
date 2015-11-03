#version 150

#ifdef NEGATO

vec4 negato();

#else

#   ifdef PIXEL_LIT
vec4 lighting(vec3 _normal, vec3 _position);
#   endif // PIXEL_LIT

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

#ifdef DIFFUSE_TEX
    vec2 oTexCoord;
#endif // DIFFUSE_TEX

} pixelIn;


#endif // NEGATO

#ifdef DIFFUSE_TEX
uniform sampler2D u_texture;
#endif // DIFFUSE_TEX

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

#ifdef DIFFUSE_TEX
        colorOut *= texture(u_texture, pixelIn.oTexCoord);
#endif // DIFFUSE_TEX

#endif // NEGATO
    return colorOut;
}
