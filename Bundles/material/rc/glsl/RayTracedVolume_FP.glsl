#version 410
#define VIEWPOINTS 8

uniform sampler3D u_image;
uniform sampler2D u_tfTexture;

#ifdef MODE3D
uniform sampler2D u_entryPoints0;
uniform sampler2D u_entryPoints1;
uniform sampler2D u_entryPoints2;
uniform sampler2D u_entryPoints3;
uniform sampler2D u_entryPoints4;
uniform sampler2D u_entryPoints5;
uniform sampler2D u_entryPoints6;
uniform sampler2D u_entryPoints7;

uniform mat4 u_invWorldViewProjs[VIEWPOINTS];

uniform float u_lobeOffset;

in vec2 uv;
#else
uniform sampler2D u_entryPoints;

uniform mat4 u_invWorldViewProj;

#endif


uniform vec3 u_lightDirs[1];
//uniform vec3 u_diffuse;

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

vec4 transferFunction(float intensity)
{
    float scaledValue = intensity * 65535.f;

    // Computes 2D indices from the hounsfield value
    int j = int( scaledValue / 256 );
    int i = int( mod( int(scaledValue), 256 ) );

    // Converts the indices into texture uv coordinates
    vec2 uvTF = vec2(i / 255.f, j / 255.f);

    return texture(u_tfTexture, uvTF);
}

//-----------------------------------------------------------------------------

vec3 gradientNormal(vec3 uvw)
{
    ivec3 imgDimensions = textureSize(u_image, 0);
    vec3 h = 1. / vec3(imgDimensions);
    vec3 hx = vec3(h.x, 0, 0);
    vec3 hy = vec3(0, h.y, 0);
    vec3 hz = vec3(0, 0, h.z);

    return normalize( vec3(
                0.5 * (texture(u_image, uvw + hx).r - texture(u_image, uvw - hx).r),
                0.5 * (texture(u_image, uvw + hy).r - texture(u_image, uvw - hy).r),
                0.5 * (texture(u_image, uvw + hz).r - texture(u_image, uvw - hz).r)
    ));
}

//-----------------------------------------------------------------------------

#ifdef MODE3D
void getRayEntryExitPoints(out vec2 rayEntryPoints[3], out mat4 viewpoints[3])
{
    vec2 entryPoints[VIEWPOINTS];

    entryPoints[0] = texture(u_entryPoints0, uv).rg;
    entryPoints[1] = texture(u_entryPoints1, uv).rg;
    entryPoints[2] = texture(u_entryPoints2, uv).rg;
    entryPoints[3] = texture(u_entryPoints3, uv).rg;
    entryPoints[4] = texture(u_entryPoints4, uv).rg;
    entryPoints[5] = texture(u_entryPoints5, uv).rg;
    entryPoints[6] = texture(u_entryPoints6, uv).rg;
    entryPoints[7] = texture(u_entryPoints7, uv).rg;

    float xy = (gl_FragCoord.x * 3.0) + gl_FragCoord.y + u_lobeOffset + 0.5;

    int vp = (VIEWPOINTS - 1) - int(mod(xy, float(VIEWPOINTS)));

    for(int i = 0; i < 3; ++i )
    {
        int index = (vp + i) % VIEWPOINTS;

        viewpoints[2 - i]     = u_invWorldViewProjs[index];
        rayEntryPoints[2 - i] = entryPoints[index];
    }
}
#endif

//-----------------------------------------------------------------------------

vec3 getFragmentImageSpacePosition(in float depth, in mat4 invWorldViewProj)
{
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

void composite(inout vec4 result, in vec4 colour)
{
    result.rgb = result.rgb + (1 - result.a) * colour.a * colour.rgb;
    result.a   = result.a   + (1 - result.a) * colour.a;
}

//-----------------------------------------------------------------------------

void main(void)
{
#ifndef MODE3D
    vec2 rayEntryExit = texelFetch(u_entryPoints, ivec2(gl_FragCoord.xy), 0).rg;

    float entryDepth =  rayEntryExit.r;
    float exitDepth  = -rayEntryExit.g;

    if(/*gl_FragCoord.z > entryDepth ||*/ exitDepth == -1)
    {
        discard;
    }

    gl_FragDepth = entryDepth;

    vec3 rayEntry = getFragmentImageSpacePosition(entryDepth, u_invWorldViewProj);
    vec3 rayExit  = getFragmentImageSpacePosition(exitDepth, u_invWorldViewProj);

    vec3 rayDir   = normalize(rayExit - rayEntry) * u_sampleDistance;

    float rayLength = length(rayExit - rayEntry);

#else
    vec2 rayEntryPoints[3];
    mat4 viewports[3];

    getRayEntryExitPoints(rayEntryPoints, viewports);

    if(rayEntryPoints[0].g == 1 && rayEntryPoints[1].g == 1 && rayEntryPoints[2].g == 1 )
    {
        discard;
    }
//    fragColor.a = 0;

    for(int i = 0; i < 3; ++ i)
    {
        float entryDepth =  rayEntryPoints[i].r;
        float exitDepth  = -rayEntryPoints[i].g;

        if(/*gl_FragCoord.z > entryDepth ||*/ exitDepth == -1)
        {
            fragColor[i] = 0;
            discard;
        }

        mat4 invWorldViewProj = viewports[i];

        vec3 rayEntry = getFragmentImageSpacePosition(entryDepth, invWorldViewProj);
        vec3 rayExit  = getFragmentImageSpacePosition(exitDepth, invWorldViewProj);

        vec3 rayDir   = normalize(rayExit - rayEntry) * u_sampleDistance;

        float rayLength = length(rayExit - rayEntry);

#endif

        vec4 result = vec4(0);

        // Opacity correction factor =
        // Inverse of the sampling rate accounted by the TF.
        const float opacityCorrectionFactor = 200.;

        vec3 rayPos = rayEntry;

        int iterCount = 0;
        for(float t = 0; iterCount < 2000 && t < rayLength; iterCount += 1, t += u_sampleDistance)
        {
    #ifdef PREINTEGRATION
            float sf = texture(u_image, rayPos).r;
            float sb = texture(u_image, rayPos + rayDir).r;

            sf = ((sf * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);
            sb = ((sb * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);

            vec4 tfColour = texture(u_tfTexture, vec2(sf, sb));

    #else
            float intensity = texture(u_image, rayPos).r;

            vec4  tfColour  = transferFunction(intensity);
    #endif // PREINTEGRATION

            if(tfColour.a > 0)
            {
                vec3 N = gradientNormal(rayPos);

                tfColour.rgb = tfColour.rgb * abs(dot(N, u_lightDirs[0]))/* * 0.7 + tfColour.rgb * abs(dot(N, u_lightDirs[1])) * 1.0*/;

    #ifndef PREINTEGRATION
                // Adjust opacity to sample distance.
                // This could be done when generating the TF texture to improve performance.
                tfColour.a   = 1 - pow(1 - tfColour.a, u_sampleDistance * opacityCorrectionFactor);
    #endif

                composite(result, tfColour);

                if(result.a > 0.99)
                {
                    break;
                }
            }

            rayPos += rayDir;
        }
#ifdef MODE3D
        fragColor[i] = result[i];
        fragColor.a += result.a;
    }

    fragColor.a /= 3;

#else
    fragColor = result;
#endif

}
