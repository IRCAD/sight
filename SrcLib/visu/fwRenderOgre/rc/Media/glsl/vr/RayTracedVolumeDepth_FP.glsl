#version 330

#define MAX_ITERATIONS 8192

uniform sampler3D u_s3Image;
uniform sampler1D u_s1TFTexture;
uniform vec2 u_f2TFWindow;

uniform float u_fSampleDis_Ms;

uniform sampler2D u_s2EntryPoints;

uniform vec3 u_f3VolumeClippingBoxMinPos_Ms;
uniform vec3 u_f3VolumeClippingBoxMaxPos_Ms;

#ifdef AUTOSTEREO
uniform mat4 u_invWorldView;
uniform mat4 u_invProj;
uniform mat4 u_worldView;
uniform mat4 u_proj;
#else // AUTOSTEREO
uniform mat4 u_invWorldViewProj;
uniform mat4 u_worldViewProj;
#endif // AUTOSTEREO

out vec4 f_f4FragCol;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float _fIntensity, in sampler1D _s1Sampler, in vec2 _f2Window);
vec3 fragCoordsToNDC(in vec3 _f3FragPos_Ss);
vec4 ndcToSpecificSpacePosition(in vec3 _f3FragPos_Ns, in mat4 _m4Inverse);
vec4 packFloatToVec4(float value);
float rayAxisAlignedBoxIntersection(in vec3 _f3RayPos, in vec3 _f3RayDir,
                                    in vec3 _f3AxisAlignedBoxMin, in vec3 _f3AxisAlignedBoxMax);

//-----------------------------------------------------------------------------

float modelSpaceToNDC(in vec3 _f3Pos_Ms)
{
#ifdef AUTOSTEREO
    vec4 f3pos_Cs = u_proj * u_worldView * vec4(_f3Pos_Ms, 1);
#else // AUTOSTEREO
    vec4 f3pos_Cs = u_worldViewProj * vec4(_f3Pos_Ms, 1);
#endif // AUTOSTEREO

    return f3pos_Cs.z / f3pos_Cs.w;
}

//-----------------------------------------------------------------------------

float firstOpaqueRayDepth(in vec3 _f3RayPos_Ms, in vec3 _f3RayDir_Ms, in float _fRayLen, in float _fSampleDis, in sampler1D _s1TFTexture, in vec2 _f2TFWindow)
{
    int iIterCount = 0;
    float t = 0.;
    // Move the ray to the first non transparent voxel.
    for(; iIterCount < MAX_ITERATIONS && t < _fRayLen; iIterCount += 1, t += _fSampleDis)
    {
        float fIntensity = texture(u_s3Image, _f3RayPos_Ms).r;
        float fTFAlpha = sampleTransferFunction(fIntensity, _s1TFTexture, _f2TFWindow).a;

        if(fTFAlpha != 0)
        {
            break;
        }

        _f3RayPos_Ms += _f3RayDir_Ms;
    }

    return modelSpaceToNDC(_f3RayPos_Ms) * 0.5f + 0.5f; // Convert to NDC assuming no clipping planes are set.
}

//-----------------------------------------------------------------------------

void main(void)
{
    vec2 f2RayEntryExitDis_Ss = texelFetch(u_s2EntryPoints, ivec2(gl_FragCoord.xy), 0).rg;

    float fRayEntryDis_Ss =  f2RayEntryExitDis_Ss.r;
    float fRayExitDis_Ss  = -f2RayEntryExitDis_Ss.g;

    if(fRayExitDis_Ss == -1 || fRayExitDis_Ss < fRayEntryDis_Ss)
    {
        discard;
    }

#ifdef AUTOSTEREO
    mat4x4 m4Cs_Ms = u_invWorldView * u_invProj;
#else // AUTOSTEREO
    mat4x4 m4Cs_Ms = u_invWorldViewProj;
#endif // AUTOSTEREO

    // Entry points in window space.
    vec3 f3RayEntryPos_Ns = fragCoordsToNDC(vec3(gl_FragCoord.xy, fRayEntryDis_Ss));
    vec3 f3RayExitPos_Ns = fragCoordsToNDC(vec3(gl_FragCoord.xy, fRayExitDis_Ss));

    // Entry points in volume texture space.
    vec3 f3RayEntryPos_Ms = ndcToSpecificSpacePosition(f3RayEntryPos_Ns, m4Cs_Ms).xyz;
    vec3 f3RayExitPos_Ms  = ndcToSpecificSpacePosition(f3RayExitPos_Ns, m4Cs_Ms).xyz;
    vec3 f3RayDir_MsN     = normalize(f3RayExitPos_Ms - f3RayEntryPos_Ms);

    vec3 f3RayDir_Ms = f3RayDir_MsN * u_fSampleDis_Ms;

    // We sometimes have rays starting at the near plane when they really shouldn't.
    // This is due to intersecting edges in the proxy geometry.
    // Although this is not technically wrong, it's suboptimal and causes artifacts due to the fact that we clamp
    // texture coordinates outside of the volume.
    // We fix this here by skipping the empty space between the ray's starting point and the volume.
    if(fRayEntryDis_Ss == 0)
    {
        float t = rayAxisAlignedBoxIntersection(f3RayEntryPos_Ms, f3RayDir_Ms,
                                                u_f3VolumeClippingBoxMinPos_Ms, u_f3VolumeClippingBoxMaxPos_Ms);

        f3RayEntryPos_Ms += f3RayDir_Ms * max(0, t);
    }

    float fRayLen = length(f3RayExitPos_Ms - f3RayEntryPos_Ms);
    float fRayDepth_Ss = firstOpaqueRayDepth(f3RayEntryPos_Ms, f3RayDir_Ms, fRayLen, u_fSampleDis_Ms, u_s1TFTexture, u_f2TFWindow);

    // If the depth of the first opaque voxel is farther from the exit point, it means there is nothing to display
    if(fRayDepth_Ss >= fRayExitDis_Ss)
    {
        discard;
    }

    f_f4FragCol = packFloatToVec4(fRayDepth_Ss);
}
