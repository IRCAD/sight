#version 330

#define MAX_ITERATIONS 8192

uniform sampler3D u_s3Image;
uniform sampler1D u_s1TFTexture;
uniform vec2 u_f2TFWindow;

uniform float u_fSampleDis;

uniform sampler2D u_s2EntryPoints;

#if AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
uniform sampler3D u_s3IlluminationVolume;
uniform vec4 u_f4VolIllumFactor;
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

#ifdef AUTOSTEREO
uniform mat4 u_invWorldView;
uniform mat4 u_invProj;
uniform mat4 u_worldView;
uniform mat4 u_proj;
#else // AUTOSTEREO
uniform mat4 u_invWorldViewProj;
uniform mat4 u_worldViewProj;
#endif // AUTOSTEREO

#ifdef PREINTEGRATION
uniform sampler2D u_s2PreintegratedTFTexture;
uniform int u_iMinImageValue;
uniform int u_iMaxImageValue;
#else // PREINTEGRATION
uniform float u_fOpacityCorrectionFactor;
#endif // PREINTEGRATION

out vec4 v_f4FragCol;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float _fIntensity, in sampler1D _s1Sampler, in vec2 _f2Window);
vec3 fragCoordsToNDC(in vec3 _f3FragPos_Ss);
vec3 ndcToFragCoord(in vec3 _f3FragPos_Ns);
vec4 ndcToSpecificSpacePosition(in vec3 _f3FragPos_Ns, in mat4 _m4Inverse);
vec3 lightingBlinnPhong(vec3 _f3NormalDir_N, vec3 _f3Pos, vec3 _f3DiffuseCol);
vec3 gradientNormal(vec3 _f3Pos_Ms, sampler3D _s3Image);

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

#ifdef PREINTEGRATION
vec4 samplePreIntegrationTable(vec3 _f3RayBack_Ms, vec3 _f3RayFront_Ms)
{
    float sf = texture(u_s3Image, _f3RayBack_Ms).r;
    float sb = texture(u_s3Image, _f3RayFront_Ms).r;

    sf = ((sf * 65535.f) - float(u_iMinImageValue) - 32767.f) / float(u_iMaxImageValue - u_iMinImageValue);
    sb = ((sb * 65535.f) - float(u_iMinImageValue) - 32767.f) / float(u_iMaxImageValue - u_iMinImageValue);

    return texture(u_s2PreintegratedTFTexture, vec2(sf, sb));
}
#endif // PREINTEGRATION

//-----------------------------------------------------------------------------

void composite(inout vec4 _f4DestCol, in vec4 _f4SrcCol)
{
    // Front-to-back blending
    _f4DestCol.rgb = _f4DestCol.rgb + (1 - _f4DestCol.a) * _f4SrcCol.a * _f4SrcCol.rgb;
    _f4DestCol.a   = _f4DestCol.a   + (1 - _f4DestCol.a) * _f4SrcCol.a;
}

//-----------------------------------------------------------------------------

struct RayInfos
{
    float m_fRayDepth_Ss;
    int m_iIterCount;
    float m_fT;
    vec3 m_f3RayPos_Ms;
};

RayInfos firstOpaqueRayInfos(in vec3 _f3RayPos_Ms, in vec3 _f3RayDir_Ms, in float _fRayLen, in float _fSampleDis, in sampler1D _s1TFTexture, in vec2 _f2TFWindow)
{
    int iIterCount = 0;
    float t = 0.;
    // Move the ray to the first non transparent voxel.
    for(; iIterCount < MAX_ITERATIONS && t < _fRayLen; iIterCount += 1, t += _fSampleDis)
    {
#ifdef PREINTEGRATION
        float fTFAlpha = samplePreIntegrationTable(_f3RayPos_Ms, _f3RayPos_Ms + _f3RayDir_Ms).a;
#else // PREINTEGRATION
        float fIntensity = texture(u_s3Image, _f3RayPos_Ms).r;
        float fTFAlpha = sampleTransferFunction(fIntensity, _s1TFTexture, _f2TFWindow).a;
#endif // PREINTEGRATION

        bool bRayPosInBounds = all(lessThanEqual(_f3RayPos_Ms, vec3(1.))) &&
                               all(greaterThanEqual(_f3RayPos_Ms, vec3(0.)));

        if(fTFAlpha != 0 && bRayPosInBounds)
        {
            break;
        }

        _f3RayPos_Ms += _f3RayDir_Ms;
    }

    float rayScreenDepth = modelSpaceToNDC(_f3RayPos_Ms) * 0.5f + 0.5f; // Convert to NDC assuming no clipping planes are set.
    RayInfos result;
    result.m_fRayDepth_Ss = rayScreenDepth;
    result.m_iIterCount = iIterCount;
    result.m_fT = t;
    result.m_f3RayPos_Ms = _f3RayPos_Ms;
    return result;
}

//-----------------------------------------------------------------------------

vec4 launchRay(in vec3 _f3RayPos_Ms, in vec3 _f3RayDir_Ms, in float _fRayLen, in float _fSampleDis, in sampler1D _s1TFTexture, in vec2 _f2TFWindow)
{
    // Move the ray to the first non transparent voxel.
    RayInfos rayInfos = firstOpaqueRayInfos(_f3RayPos_Ms, _f3RayDir_Ms, _fRayLen, _fSampleDis, _s1TFTexture, _f2TFWindow);
    gl_FragDepth = rayInfos.m_fRayDepth_Ss;
    int iIterCount = rayInfos.m_iIterCount;
    float t = rayInfos.m_fT;
    _f3RayPos_Ms = rayInfos.m_f3RayPos_Ms;
    vec4 f4ResultCol = vec4(0.);
    for(; iIterCount < MAX_ITERATIONS && t < _fRayLen; iIterCount += 1, t += _fSampleDis)
    {
#ifdef PREINTEGRATION
        vec4 f4TFCol = samplePreIntegrationTable(_f3RayPos_Ms, _f3RayPos_Ms + _f3RayDir_Ms);
#else // PREINTEGRATION
        float fIntensity = texture(u_s3Image, _f3RayPos_Ms).r;
        vec4 f4TFCol = sampleTransferFunction(fIntensity, _s1TFTexture, _f2TFWindow);
#endif // PREINTEGRATION

        if(f4TFCol.a > 0.)
        {
            vec3 f3NormalDir_MsN = gradientNormal(_f3RayPos_Ms, u_s3Image);

            f4TFCol.rgb = lightingBlinnPhong(f3NormalDir_MsN, _f3RayPos_Ms, f4TFCol.rgb);

#ifndef PREINTEGRATION
            // Adjust opacity to sample distance.
            // This could be done when generating the TF texture to improve performance.
            f4TFCol.a = 1 - pow(1 - f4TFCol.a, u_fSampleDis * u_fOpacityCorrectionFactor);
#endif // PREINTEGRATION

#if AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
            vec4 f4VolIllum = texture(u_s3IlluminationVolume, _f3RayPos_Ms);
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

#if AMBIENT_OCCLUSION || SHADOWS
            // Apply ambient occlusion + shadows
            f4TFCol.rgb *= pow(exp(-f4VolIllum.a), u_f4VolIllumFactor.a);
#endif // AMBIENT_OCCLUSION || SHADOWS

#ifdef COLOR_BLEEDING
            // Apply color bleeding
            f4TFCol.rgb *= pow(1+f4VolIllum.rgb, u_f4VolIllumFactor.rgb);
#endif // COLOR_BLEEDING

            composite(f4ResultCol, f4TFCol);

            if(f4ResultCol.a > 0.99)
            {
                break;
            }
        }

        _f3RayPos_Ms += _f3RayDir_Ms;
    }

    return f4ResultCol;
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

    vec3 f3RayDir_Ms = f3RayDir_MsN * u_fSampleDis;

    float fRayLen = length(f3RayExitPos_Ms - f3RayEntryPos_Ms);
    vec4 f4ResultCol = launchRay(f3RayEntryPos_Ms, f3RayDir_Ms, fRayLen, u_fSampleDis, u_s1TFTexture, u_f2TFWindow);

    v_f4FragCol = f4ResultCol;
}
