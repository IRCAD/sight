#version 150

#ifdef DIFFUSE_TEX
uniform int u_useTextureAlpha;
#endif // DIFFUSE_TEX

#ifdef PIXEL_LIT
vec4 lighting(vec3 _normal, vec3 _position);
#endif // PIXEL_LIT

in PixelDataIn
{
#ifdef PIXEL_LIT

    vec3 oPosition_WS;
    vec3 oNormal_WS;
    vec4 oColor;

#else

#   ifdef FLAT
    flat vec4 oColor;
#   else
    vec4 oColor;
#   endif // FLAT

#endif // PIXEL_LIT

#ifdef DIFFUSE_TEX
    vec2 oTexCoord;
#endif // DIFFUSE_TEX

} pixelIn;


#ifdef DIFFUSE_TEX
uniform sampler2D u_texture;
#endif // DIFFUSE_TEX

vec4 getFragmentColor()
{

#ifdef PIXEL_LIT
    vec4 colorOut = lighting(normalize(pixelIn.oNormal_WS), pixelIn.oPosition_WS) * pixelIn.oColor;
#else
    vec4 colorOut = pixelIn.oColor;
#endif

#ifdef DIFFUSE_TEX
    vec4 colorTex = texture(u_texture, pixelIn.oTexCoord);

    if(u_useTextureAlpha == 0)
    {
        colorTex.a = 1.0;
    }

    colorOut *= colorTex;
#endif // DIFFUSE_TEX

    return colorOut;
}
