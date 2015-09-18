#version 150

#ifdef NEGATO

vec4 negato();

#else

#   ifdef PIXEL_LIGHTING
in vec3 oPosition_WS;
in vec3 oNormal_WS;
in vec4 oColor;

vec4 lighting(vec3 _normal, vec3 _position);

#   else

#       ifdef FLAT
flat in vec4 oColor;
#       else
in vec4 oColor;
#       endif // FLAT

#   endif // PIXEL_LIGHTING

#   ifndef EDGE
in vec2 oTexCoord;
uniform sampler2D u_defaultTex;
#   endif // EDGE

#endif // NEGATO

vec4 getMaterialColor()
{
#ifdef NEGATO
    vec4 colorOut = negato();
#else

#   ifdef PIXEL_LIGHTING
        vec4 colorOut = lighting(normalize(oNormal_WS), oPosition_WS) * oColor;
#   else
        vec4 colorOut = oColor;
#   endif

#   ifndef EDGE
        colorOut *= texture(u_defaultTex, oTexCoord);
#   endif

#endif // NEGATO
    return colorOut;
}
