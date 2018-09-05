#version 410

uniform vec4 u_diffuse;

// Uniforms
#ifdef DIFFUSE_TEX
uniform int u_useTextureAlpha;
#endif // DIFFUSE_TEX

// Extern functions
#ifdef PIXEL_LIT
vec4 lighting(vec3 _normal, vec3 _position);
#endif // PIXEL_LIT

// Input semantics
layout(location = 0) in vec3 inNormal_WS;

#ifdef PIXEL_LIT
layout(location = 1) in vec3 inPosition_WS;
 #endif

#ifdef FLAT
layout(location = 2) flat in vec4 inColor;
#   else
layout(location = 2) in vec4 inColor;
#   endif // FLAT

#ifdef DIFFUSE_TEX
layout(location = 3) in vec2 inTexCoord;

uniform sampler2D u_texture;
#endif // DIFFUSE_TEX

// Compute illumination or forward illumination received from the vertex shader
vec4 getFragmentColor()
{

#ifdef PIXEL_LIT
    vec4 colorOut = lighting(normalize(inNormal_WS), inPosition_WS) * inColor;
#else
    vec4 colorOut = inColor;
#endif

#ifdef DIFFUSE_TEX
    vec4 colorTex = texture(u_texture, inTexCoord);

    if(u_useTextureAlpha == 0)
    {
        colorTex.a = 1.0;
    }

    colorOut *= colorTex;
#endif // DIFFUSE_TEX

    return colorOut;
}

// Compute alpha channel
float getFragmentAlpha()
{
    float alpha = u_diffuse.a * inColor.a;

#ifdef DIFFUSE_TEX
    vec4 colorTex = texture(u_texture, inTexCoord);

    if(u_useTextureAlpha == 0)
    {
        colorTex.a = 1.0;
    }

    alpha *= colorTex.a;
#endif // DIFFUSE_TEX

    return alpha;
}
