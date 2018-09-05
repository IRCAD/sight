#version 330

uniform sampler3D u_mask;

uniform sampler2D u_entryPoints;

uniform mat4 u_invWorldViewProj;
#if IDVR == 1
uniform mat4 u_worldViewProj;
#endif // IDVR == 1

uniform float u_sampleDistance;

uniform vec4 u_viewport;

uniform float u_clippingNear;
uniform float u_clippingFar;

// MImP with countersink geometry
#if IDVR == 1
layout (location = 0) out vec4 mrt_IC_RayTracing;
layout (location = 1) out vec4 mrt_IC_JFA;
#else
#if IDVR == 2 || IDVR == 3
out vec4 mrt_IC_RayTracing;
#endif
#endif

//-----------------------------------------------------------------------------

vec3 getFragmentImageSpacePosition(in float depth, in mat4 invWorldViewProj)
{
    vec3 screenPos = vec3(gl_FragCoord.xy * u_viewport.zw, depth);
    screenPos -= 0.5;
    screenPos *= 2.0;

    vec4 clipPos;
    clipPos.w   = (2 * u_clippingNear * u_clippingFar) / (u_clippingNear + u_clippingFar + screenPos.z * (u_clippingNear - u_clippingFar));
    clipPos.xyz = screenPos * clipPos.w;

    vec4 imgPos = invWorldViewProj * clipPos;

    return imgPos.xyz / imgPos.w;
}

//-----------------------------------------------------------------------------

#if IDVR == 1
float voxelScreenDepth(in vec3 pos)
{
    vec4 projPos = u_worldViewProj * vec4(pos, 1);

    return projPos.z / projPos.w;
}
#endif

//-----------------------------------------------------------------------------

void composite(inout vec4 dest, in vec4 src)
{
    // Front-to-back blending
    dest.rgb = dest.rgb + (1 - dest.a) * src.a * src.rgb;
    dest.a   = dest.a   + (1 - dest.a) * src.a;
}

//-----------------------------------------------------------------------------

void launchRay(inout vec3 rayPos, in vec3 rayDir, in float rayLength, inout vec4 IC_RayTracing, inout vec4 IC_JFA)
{
#if IDVR == 1 // MImP
    IC_JFA = vec4(0.0);
    IC_RayTracing = vec4(0.0, 0.0, 0.0, 0.0);
#else
    IC_RayTracing = vec4(0.0, 0.0, 0.0, 0.0);
#endif

    int iterCount = 0;

    // For the segmentation we have a [0, 255] range
    // So we check for value superior to 128: 128 / 65536 = 0.001953125
    float edge = 0.5 + 0.001953125;

    float nbSamples = 0.0f;

    for(float t = 0; iterCount < 65000 && t < rayLength; iterCount += 1, t += u_sampleDistance)
    {
        float maskValue = texture(u_mask, rayPos).r;

        if(maskValue > edge)
        {
// Maximum Importance compositing
#if IDVR == 1
            IC_RayTracing = vec4(rayPos, 1.);

            vec2 ndcScreenPos = (gl_FragCoord.xy * u_viewport.zw - 0.5) * 2.;
            float rayScreenDepth = voxelScreenDepth(rayPos);

            IC_JFA = vec4(ndcScreenPos, rayScreenDepth, 1.);
#else
// Average Importance compositing & Visibility preserving Importance Compositing
// We count the number of samples until the first important object
// We will ponder those samples by the lowest important samples
            IC_RayTracing = vec4(nbSamples, 0., 0., 1.);
#endif
            break;
        }

        rayPos += rayDir;
        nbSamples += 1.0f;
    }
}

//-----------------------------------------------------------------------------

void main(void)
{
    vec2 rayEntryExit = texelFetch(u_entryPoints, ivec2(gl_FragCoord.xy), 0).rg;

    float entryDepth =  rayEntryExit.r;
    float exitDepth  = -rayEntryExit.g;

    if(exitDepth == -1)
    {
        discard;
    }

    vec3 rayEntry = getFragmentImageSpacePosition(entryDepth, u_invWorldViewProj);
    vec3 rayExit  = getFragmentImageSpacePosition(exitDepth, u_invWorldViewProj);

    vec3 rayDir   = normalize(rayExit - rayEntry) * u_sampleDistance;

    float rayLength = length(rayExit - rayEntry);

    vec3 rayPos = rayEntry;

#if IDVR == 2 || IDVR == 3
    vec4 mrt_IC_JFA = vec4(0.0, 0.0, 0.0, 1.0);
#endif

    launchRay(rayPos, rayDir, rayLength, mrt_IC_RayTracing, mrt_IC_JFA);
}
