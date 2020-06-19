#version 330

#extension GL_GOOGLE_include_directive : enable // Needed for glslangValidator to handle include directives.

#include "VolumeRay.glsl.struct"

uniform sampler3D u_s3Image;

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

vec3 fragCoordsToNDC(in vec3 _f3FragPos_Ss);
vec3 ndcToFragCoord(in vec3 _f3FragPos_Ns);
vec4 ndcToSpecificSpacePosition(in vec3 _f3FragPos_Ns, in mat4 _m4Inverse);
float rayAxisAlignedBoxIntersection(in vec3 _f3RayPos, in vec3 _f3RayDir, in vec3 _f3AxisAlignedBoxMin, in vec3 _f3AxisAlignedBoxMax);
void advanceUntilOpaque(inout VolumeRay _vray_Ms, in sampler3D _s3Image);
vec4 compositeAlongRay(inout VolumeRay _vray_Ms, in sampler3D _s3Image);


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
    VolumeRay vray_Ms = VolumeRay(f3RayEntryPos_Ms, f3RayDir_Ms, fRayLen, u_fSampleDis_Ms, 0);

    // Compute the ray's screen space depth at the first non transparent voxel.
    advanceUntilOpaque(vray_Ms, u_s3Image);
    gl_FragDepth = modelSpaceToNDC(vray_Ms.position) * 0.5f + 0.5f;

    f_f4FragCol = compositeAlongRay(vray_Ms, u_s3Image);
}
