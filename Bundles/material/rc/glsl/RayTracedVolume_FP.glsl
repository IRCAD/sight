#version 330

uniform sampler3D u_image;
uniform sampler2D u_tfTexture;

#if AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
uniform sampler3D u_illuminationVolume;
uniform vec4 u_volIllumFactor;
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

uniform sampler2D u_entryPoints;

uniform mat4 u_invWorldViewProj;

#ifdef AUTOSTEREO
uniform mat4 u_invWorldView;
uniform mat4 u_invProj;
#endif // AUTOSTEREO

uniform vec3 u_cameraPos;
uniform float u_shininess;

#define MAX_LIGHTS 10

uniform int u_numLights;

uniform vec3 u_lightDir[MAX_LIGHTS];
uniform vec3 u_lightDiffuse[MAX_LIGHTS];
uniform vec3 u_lightSpecular[MAX_LIGHTS];

uniform float u_sampleDistance;

uniform float u_viewportWidth;
uniform float u_viewportHeight;

uniform float u_clippingNear;
uniform float u_clippingFar;

#ifdef PREINTEGRATION
uniform int u_min;
uniform int u_max;
#endif // PREINTEGRATION

out vec4 fragColor;

//-----------------------------------------------------------------------------
vec4 sampleTransferFunction(float intensity);

//-----------------------------------------------------------------------------

vec3 lighting(vec3 _normal, vec3 _position, vec3 _diffuse)
{
    vec3 vecToCam = normalize(u_cameraPos - _position);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for(int i = 0; i < u_numLights; ++i)
    {
        float fLitDiffuse = clamp(dot( normalize(-u_lightDir[i]), _normal ), 0, 1);
        diffuse += fLitDiffuse * u_lightDiffuse[i] * _diffuse;

        vec3 r = reflect(u_lightDir[i], _normal);
        float fLitSpecular = pow( clamp(dot( r, vecToCam ), 0, 1), u_shininess);
        specular += fLitSpecular * u_lightSpecular[i];
    }

    return vec3(diffuse + specular);
}

//-----------------------------------------------------------------------------

vec3 gradientNormal(vec3 uvw)
{
    ivec3 imgDimensions = textureSize(u_image, 0);
    vec3 h = 1. / vec3(imgDimensions);
    vec3 hx = vec3(h.x, 0, 0);
    vec3 hy = vec3(0, h.y, 0);
    vec3 hz = vec3(0, 0, h.z);

    return -normalize( vec3(
                (texture(u_image, uvw + hx).r - texture(u_image, uvw - hx).r),
                (texture(u_image, uvw + hy).r - texture(u_image, uvw - hy).r),
                (texture(u_image, uvw + hz).r - texture(u_image, uvw - hz).r)
    ));
}

//-----------------------------------------------------------------------------

vec3 getFragmentImageSpacePosition(in float depth, in mat4 invWorldViewProj)
{
    // TODO: Simplify this -> uniforms
    vec3 screenPos = vec3(gl_FragCoord.xy / vec2(u_viewportWidth, u_viewportHeight), depth);
    screenPos -= 0.5;
    screenPos *= 2.0;

    vec4 clipPos;
    clipPos.w   = (2 * u_clippingNear * u_clippingFar) / (u_clippingNear + u_clippingFar + screenPos.z * (u_clippingNear - u_clippingFar));
    clipPos.xyz = screenPos * clipPos.w;

    vec4 imgPos = invWorldViewProj * clipPos;

    return imgPos.xyz / imgPos.w;
}

//-----------------------------------------------------------------------------

void composite(inout vec4 dest, in vec4 src)
{
    // Front-to-back blending
    dest.rgb = dest.rgb + (1 - dest.a) * src.a * src.rgb;
    dest.a   = dest.a   + (1 - dest.a) * src.a;
}

//-----------------------------------------------------------------------------

vec4 launchRay(inout vec3 rayPos, in vec3 rayDir, in float rayLength, in float sampleDistance)
{
    // Opacity correction factor =
    // Inverse of the sampling rate accounted by the TF.
    const float opacityCorrectionFactor = 200.;

    vec4 result = vec4(0);

    int iterCount = 0;
    for(float t = 0; iterCount < 65000 && t < rayLength; iterCount += 1, t += sampleDistance)
    {
#ifdef PREINTEGRATION
        float sf = texture(u_image, rayPos).r;
        float sb = texture(u_image, rayPos + rayDir).r;

        sf = ((sf * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);
        sb = ((sb * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);

        vec4 tfColour = texture(u_tfTexture, vec2(sf, sb));

#else
        float intensity = texture(u_image, rayPos).r;

        vec4  tfColour  = sampleTransferFunction(intensity);
#endif // PREINTEGRATION

        if(tfColour.a > 0)
        {
            vec3 N = gradientNormal(rayPos);

            tfColour.rgb = lighting(N, rayPos, tfColour.rgb);

#ifndef PREINTEGRATION
            // Adjust opacity to sample distance.
            // This could be done when generating the TF texture to improve performance.
            tfColour.a   = 1 - pow(1 - tfColour.a, u_sampleDistance * opacityCorrectionFactor);
#endif // PREINTEGRATION

#if AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
            vec4 volIllum = texture(u_illuminationVolume, rayPos);
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

#if AMBIENT_OCCLUSION || SHADOWS
            // Apply ambient occlusion + shadows
            tfColour.rgb *= pow(exp(-volIllum.a), u_volIllumFactor.a);
#endif // AMBIENT_OCCLUSION || SHADOWS

#ifdef COLOR_BLEEDING
            // Apply color bleeding
            tfColour.rgb *= pow(1+volIllum.rgb, u_volIllumFactor.rgb);
#endif // COLOR_BLEEDING

            composite(result, tfColour);

            if(result.a > 0.99)
            {
                break;
            }
        }

        rayPos += rayDir;
    }

    return result;
}

//-----------------------------------------------------------------------------

void main(void)
{
    vec2 rayEntryExit = texelFetch(u_entryPoints, ivec2(gl_FragCoord.xy), 0).rg;

    float entryDepth =  rayEntryExit.r;
    float exitDepth  = -rayEntryExit.g;

    if(/*gl_FragCoord.z > entryDepth ||*/ exitDepth == -1)
    {
        discard;
    }

    gl_FragDepth = entryDepth;

#ifdef AUTOSTEREO
    mat4x4 invWorldViewProj = u_invWorldView * u_invProj;
#else
    mat4x4 invWorldViewProj = u_invWorldViewProj;
#endif

    vec3 rayEntry = getFragmentImageSpacePosition(entryDepth, invWorldViewProj);
    vec3 rayExit  = getFragmentImageSpacePosition(exitDepth, invWorldViewProj);

    vec3 rayDir   = normalize(rayExit - rayEntry) * u_sampleDistance;

    float rayLength = length(rayExit - rayEntry);

    vec3 rayPos = rayEntry;
    vec4 result = launchRay(rayPos, rayDir, rayLength, u_sampleDistance);

    fragColor = result;

}
