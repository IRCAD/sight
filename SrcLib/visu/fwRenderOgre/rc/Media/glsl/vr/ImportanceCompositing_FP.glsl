#version 330

uniform sampler3D u_mask;

uniform sampler2D u_entryPoints;

uniform mat4 u_invWorldViewProj;
#if IDVR == 1
uniform mat4 u_worldViewProj;
#endif // IDVR == 1

uniform float u_sampleDistance;

layout (location = 0) out vec4 mrt_IC_RayTracing;
// MImP with countersink geometry
#if IDVR == 1
layout (location = 1) out vec4 mrt_IC_JFA;
#endif

//-----------------------------------------------------------------------------

vec3 fragCoordsToNDC(in vec3 _f3FragPos_Ss);
vec4 ndcToSpecificSpacePosition(in vec3 _f3FragPos_Ns, in mat4 _m4Inverse);

//-----------------------------------------------------------------------------

vec4 getFragmentImageSpacePosition(in float _depth, in mat4 _invWorldViewProj)
{
    vec3 screenPos = vec3(gl_FragCoord.xy, _depth);
    vec3 ndcPos = fragCoordsToNDC(screenPos);
    return ndcToSpecificSpacePosition(ndcPos, _invWorldViewProj);
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

void launchRay(inout vec3 rayPos, in vec3 rayDir, in float rayLength)
{
#if IDVR == 1 // MImP
    mrt_IC_JFA = vec4(0.0);
#endif
    mrt_IC_RayTracing = vec4(0.0);

    int iterCount = 0;

    // For the segmentation we have a [0, 255] range
    // So we check for value superior to 128: 128 / 65536 = 0.001953125
    float edge = 0.5 + 0.001953125;

    float nbSamples = 0.0f;

    for(float t = 0 ; iterCount < 65000 && t < rayLength; iterCount += 1, t += u_sampleDistance)
    {
        float maskValue = texture(u_mask, rayPos).r;

        if(maskValue > edge)
        {
// Maximum Importance compositing
#if IDVR == 1
            mrt_IC_RayTracing = vec4(rayPos, 1.);

            vec2 ndcScreenPos = fragCoordsToNDC(vec3(gl_FragCoord.xy, gl_FragDepth)).xy;
            float rayScreenDepth = voxelScreenDepth(rayPos);

            mrt_IC_JFA = vec4(ndcScreenPos, rayScreenDepth, 1.);
#else
// Average Importance compositing & Visibility preserving Importance Compositing
// We count the number of samples until the first important object
// We will ponder those samples by the lowest important samples
            mrt_IC_RayTracing = vec4(nbSamples, 0., 0., 1.);
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

    vec3 rayEntry = getFragmentImageSpacePosition(entryDepth, u_invWorldViewProj).xyz;
    vec3 rayExit  = getFragmentImageSpacePosition(exitDepth, u_invWorldViewProj).xyz;

    vec3 rayDir   = normalize(rayExit - rayEntry) * u_sampleDistance;

    float rayLength = length(rayExit - rayEntry);

    vec3 rayPos = rayEntry;

    launchRay(rayPos, rayDir, rayLength);
}
