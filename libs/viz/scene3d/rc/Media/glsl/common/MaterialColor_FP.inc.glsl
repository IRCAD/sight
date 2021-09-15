uniform vec4 u_diffuse;

// Uniforms
#ifdef DIFFUSE_TEX
uniform int u_useTextureAlpha;
#endif // DIFFUSE_TEX

// Input semantics
layout(location = 0) in vec3 v_f3Normal_Ws;


#if defined(FLAT)
layout(location = 1) in vec3 v_f3Position_Vs;
#elif defined(PHONG)
layout(location = 1) in vec3 v_f3Position_Ws;
#endif

#ifdef FLAT
layout(location = 2) flat in vec4 v_f4Color;
#   else
layout(location = 2) in vec4 v_f4Color;
#   endif // FLAT

#ifdef DIFFUSE_TEX
layout(location = 3) in vec2 v_f2TexCoord;

uniform sampler2D u_texture;
#endif // DIFFUSE_TEX

// Compute illumination or forward illumination received from the vertex shader
vec4 getFragmentColor()
{
#if defined(FLAT)
    vec3 xTangent = dFdx( v_f3Position_Vs );
    vec3 yTangent = dFdy( v_f3Position_Vs );
    vec3 faceNormal_Vs = normalize( cross( xTangent, yTangent ) );
    vec4 colorOut = lighting(normalize(faceNormal_Vs), v_f3Position_Vs) * v_f4Color;
#elif defined(PHONG)
    vec4 colorOut = lighting(normalize(v_f3Normal_Ws), v_f3Position_Ws) * v_f4Color;
#else
    vec4 colorOut = v_f4Color;
#endif

#ifdef DIFFUSE_TEX
    vec4 colorTex = texture(u_texture, v_f2TexCoord);

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
    float alpha = u_diffuse.a * v_f4Color.a;

#ifdef DIFFUSE_TEX
    vec4 colorTex = texture(u_texture, v_f2TexCoord);

    if(u_useTextureAlpha == 0)
    {
        colorTex.a = 1.0;
    }

    alpha *= colorTex.a;
#endif // DIFFUSE_TEX

    return alpha;
}
