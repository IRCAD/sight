#version 330

#define MAX_ITERATIONS 8192

uniform sampler3D u_image;
uniform sampler1D u_s1TFTexture;
uniform vec2 u_f2TFWindow;

uniform vec3 u_cameraPos;
uniform float u_shininess;

uniform float u_sampleDistance;

uniform sampler2D u_entryPoints;

#define MAX_LIGHTS 10
uniform float u_numLights;

uniform vec3 u_lightDir[MAX_LIGHTS];
uniform vec3 u_lightDiffuse[MAX_LIGHTS];
uniform vec3 u_lightSpecular[MAX_LIGHTS];

#if AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
uniform sampler3D u_illuminationVolume;
uniform vec4 u_volIllumFactor;
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
uniform sampler2D u_preintegratTFTexture;
uniform int u_min;
uniform int u_max;
#else // PREINTEGRATION
uniform float u_opacityCorrectionFactor;
#endif // PREINTEGRATION

#if IDVR >= 1
uniform sampler2D u_IC;
#endif

#if IDVR == 1
uniform sampler2D u_JFA;
#endif // IDVR == 1

#if IDVR == 2
uniform float u_aimcAlphaCorrection;
#endif // IDVR == 2

#if IDVR == 3
uniform float u_vpimcAlphaCorrection;
#endif // IDVR == 3

#ifdef CSG
uniform sampler2D u_csgInfos;
#endif // CSG

#ifdef CSG_TF
uniform vec2 u_CSGTFWindow;
uniform sampler1D u_CSGTFTexture;
#endif // CSG_TF

#ifdef CSG_DEPTH_LINES
// Number of image spacing units (millimeters usually) separating depth lines.
uniform int u_csgDepthLinesSpacing;
uniform float u_csgDepthLinesWidth;
uniform vec3 u_csgDepthLinesColor;
#endif // CSG_DEPTH_LINES

#ifdef CSG_BORDER
uniform float u_csgBorderWidth;
uniform vec3 u_csgBorderColor;
#endif // CSG_BORDER

#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6
uniform float u_csgColorModulationFactor;
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6

#ifdef CSG_OPACITY_DECREASE
uniform float u_csgOpacityDecreaseFactor;
#endif // CSG_OPACITY_DECREASE

out vec4 v_f4FragCol;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float _fIntensity, in sampler1D _s1Sampler, in vec2 _f2Window);
vec3 fragCoordsToNDC(in vec3 _f3FragPos_Ss);
vec3 ndcToFragCoord(in vec3 _f3FragPos_Ns);
vec4 ndcToSpecificSpacePosition(in vec3 _f3FragPos_Ns, in mat4 _m4Inverse);

//-----------------------------------------------------------------------------

vec3 lighting(vec3 _f3NormalDir_MsN, vec3 _f3Pos_Ms, vec3 _f3DiffuseCol)
{
    vec3 f3VecToCamDir_MsN = normalize(u_cameraPos - _f3Pos_Ms);

    vec3 f3DiffuseCol = vec3(0.);
    vec3 f3SpecularCol = vec3(0.);

    for(int i = 0; i < int(u_numLights); ++i)
    {
        // We use the Blinn-Phong lighting model.
        float fLitDiffuseCol_N = clamp(abs(dot( normalize(-u_lightDir[i]), _f3NormalDir_MsN )), 0, 1);
        f3DiffuseCol += fLitDiffuseCol_N * u_lightDiffuse[i] * _f3DiffuseCol;

        vec3 H = normalize(u_lightDir[i] + f3VecToCamDir_MsN);
        float fLitSpecular = clamp(pow( abs(dot( _f3NormalDir_MsN, H )), u_shininess), 0, 1);
        f3SpecularCol += fLitSpecular * u_lightSpecular[i];
    }

    return vec3(f3DiffuseCol + f3SpecularCol);
}

//-----------------------------------------------------------------------------

vec3 gradientNormal(vec3 _f3UVW)
{
    ivec3 imgDimensions = textureSize(u_image, 0);
    vec3 h = 1. / vec3(imgDimensions);
    vec3 hx = vec3(h.x, 0, 0);
    vec3 hy = vec3(0, h.y, 0);
    vec3 hz = vec3(0, 0, h.z);

    return -normalize( vec3(
                (texture(u_image, _f3UVW + hx).r - texture(u_image, _f3UVW - hx).r),
                (texture(u_image, _f3UVW + hy).r - texture(u_image, _f3UVW - hy).r),
                (texture(u_image, _f3UVW + hz).r - texture(u_image, _f3UVW - hz).r)
    ));
}

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
    float sf = texture(u_image, _f3RayBack_Ms).r;
    float sb = texture(u_image, _f3RayFront_Ms).r;

    sf = ((sf * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);
    sb = ((sb * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);

    return texture(u_preintegratTFTexture, vec2(sf, sb));
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

#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6
vec3 rgb2hsv(vec3 RGB);
vec3 hsv2rgb(vec3 HSL);
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6

//-----------------------------------------------------------------------------

struct RayInfos
{
    float m_fRayDepth_Ss;
    int m_iIterCount;
    float m_fT;
    vec3 m_f3RayPos_Ms;
};

RayInfos firstOpaqueRayInfos(in vec3 _f3RayPos_Ms, in vec3 _f3RayDir_MsN, in float _fRayLen, in float _fSampleDis, in sampler1D _s1TFTexture, in vec2 _f2TFWindow)
{
    int iIterCount = 0;
    float t = 0.;
    // Move the ray to the first non transparent voxel.
    for(; iIterCount < MAX_ITERATIONS && t < _fRayLen; iIterCount += 1, t += _fSampleDis)
    {
#ifdef PREINTEGRATION
        float fTFAlpha = samplePreIntegrationTable(_f3RayPos_Ms, _f3RayPos_Ms + _f3RayDir_MsN).a;
#else // PREINTEGRATION
        float fIntensity = texture(u_image, _f3RayPos_Ms).r;
        float fTFAlpha = sampleTransferFunction(fIntensity, _s1TFTexture, _f2TFWindow).a;
#endif // PREINTEGRATION

        if(fTFAlpha != 0)
        {
            break;
        }

        _f3RayPos_Ms += _f3RayDir_MsN;
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

vec4 launchRay(in vec3 _f3RayPos_Ms, in vec3 _f3RayDir_MsN, in float _fRayLen, in float _fSampleDis, in sampler1D _s1TFTexture, in vec2 _f2TFWindow)
{
    // Move the ray to the first non transparent voxel.
    RayInfos rayInfos = firstOpaqueRayInfos(_f3RayPos_Ms, _f3RayDir_MsN, _fRayLen, _fSampleDis, _s1TFTexture, _f2TFWindow);
    gl_FragDepth = rayInfos.m_fRayDepth_Ss;
    int iIterCount = rayInfos.m_iIterCount;
    float t = rayInfos.m_fT;
    _f3RayPos_Ms = rayInfos.m_f3RayPos_Ms;

#if IDVR == 2 || IDVR == 3
    // With Visibility preserving importance compositing
    // We count the number of samples until we reach the important feature
    float fNbSamples = 0.;
#endif // IDVR == 2 || IDVR == 3
    vec4 f4ResultCol = vec4(0.);
    for(; iIterCount < MAX_ITERATIONS && t < _fRayLen; iIterCount += 1, t += _fSampleDis)
    {
#ifdef PREINTEGRATION
        vec4 f4TFCol = samplePreIntegrationTable(_f3RayPos_Ms, _f3RayPos_Ms + _f3RayDir_MsN);
#else // PREINTEGRATION
        float fIntensity = texture(u_image, _f3RayPos_Ms).r;
        vec4 f4TFCol = sampleTransferFunction(fIntensity, _s1TFTexture, _f2TFWindow);
#endif // PREINTEGRATION

        if(f4TFCol.a > 0.)
        {
            vec3 f3NormalDir_MsN = gradientNormal(_f3RayPos_Ms);

            f4TFCol.rgb = lighting(f3NormalDir_MsN, _f3RayPos_Ms, f4TFCol.rgb);

#ifndef PREINTEGRATION
            // Adjust opacity to sample distance.
            // This could be done when generating the TF texture to improve performance.
            f4TFCol.a = 1 - pow(1 - f4TFCol.a, u_sampleDistance * u_opacityCorrectionFactor);
#endif // PREINTEGRATION

#if AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
            vec4 f4VolIllum = texture(u_illuminationVolume, _f3RayPos_Ms);
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

#if AMBIENT_OCCLUSION || SHADOWS
            // Apply ambient occlusion + shadows
            f4TFCol.rgb *= pow(exp(-f4VolIllum.a), u_volIllumFactor.a);
#endif // AMBIENT_OCCLUSION || SHADOWS

#ifdef COLOR_BLEEDING
            // Apply color bleeding
            f4TFCol.rgb *= pow(1+f4VolIllum.rgb, u_volIllumFactor.rgb);
#endif // COLOR_BLEEDING

// Average Importance Compositing
#if IDVR == 2
            vec4 f4Aimc = texelFetch(u_IC, ivec2(gl_FragCoord.xy), 0);
            // We ensure that we have a number of samples > 0, to be in the region of interest
            if(f4Aimc.r > 0. && fNbSamples <= f4Aimc.r)
            {
                f4TFCol.a = f4TFCol.a * u_aimcAlphaCorrection;
            }
#endif // IDVR == 2

#if IDVR == 3
            vec4 f4Aimc = texelFetch(u_IC, ivec2(gl_FragCoord.xy), 0);
            // We ensure that we have a number of samples > 0, to be in the region of interest
            if(f4Aimc.r > 0. && int(fNbSamples) == int(f4Aimc.r))
            {
                f4ResultCol.rgb = f4ResultCol.rgb * u_vpimcAlphaCorrection;
                f4ResultCol.a = u_vpimcAlphaCorrection;
            }
#endif // IDVR == 3

            composite(f4ResultCol, f4TFCol);

            if(f4ResultCol.a > 0.99
#if IDVR == 3
// In the case of Visibility preserving importance compositing
// We need to ensure that we reach a certain amount of samples
// Before cutting off the compositing and breaking the ray casting
// Otherwise we will miss the important feature
            && int(fNbSamples) > int(f4Aimc.r)
#endif // IDVR == 3
            )
            {
                break;
            }
        }

        _f3RayPos_Ms += _f3RayDir_MsN;
#if IDVR == 2 || IDVR == 3
        fNbSamples += 1.0;
#endif // IDVR == 2 || IDVR == 3
    }

    return f4ResultCol;
}

//-----------------------------------------------------------------------------

void main(void)
{
    vec2 f2RayEntryExitDis_Ss = texelFetch(u_entryPoints, ivec2(gl_FragCoord.xy), 0).rg;

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
    vec3 f3RayDir_MsN   = normalize(f3RayExitPos_Ms - f3RayEntryPos_Ms);
#if IDVR == 1
    vec3 f3RayCSGEntryPos_Ms = f3RayEntryPos_Ms;
    vec4 f4ImportancePos_Ms = texelFetch(u_IC, ivec2(gl_FragCoord.xy), 0);
#ifdef CSG
    vec4 f4CSGInfos = texelFetch(u_csgInfos, ivec2(gl_FragCoord.xy), 0);
    bool isCsg = f4CSGInfos.w > 0.; // true if this ray hits the csg.
#endif // CSG
    // If we have an importance factor, we move the ray accordingly
    if(f4ImportancePos_Ms.a > 0.)
    {
        f3RayEntryPos_Ms = f4ImportancePos_Ms.xyz;
#ifdef CSG
        isCsg = true;
#endif // CSG
    }
#ifdef CSG
    // Otherwise, we check if the frag is in the CSG to dig into the volume
    else if(isCsg)
    {
        f3RayEntryPos_Ms = f4CSGInfos.xyz;
    }
#ifdef CSG_DISABLE_CONTEXT
    else
    {
        discard;
    }
#endif // CSG_DISABLE_CONTEXT
#endif // CSG
#endif // IDVR == 1

    f3RayDir_MsN *= u_sampleDistance;
    float fRayLen = length(f3RayExitPos_Ms - f3RayEntryPos_Ms);
#ifndef CSG
    vec4 f4ResultCol = launchRay(f3RayEntryPos_Ms, f3RayDir_MsN, fRayLen, u_sampleDistance, u_s1TFTexture, u_f2TFWindow);
#else // CSG
    vec4 f4ResultCol = vec4(0.);
    // If the ray hit the CSG, some modulations and effects must be applied.
    if(isCsg)
    {
        // We launch a ray only if the exit position is in front of the ray entry position and if the context must be disabled,
        // check the entry opacity. These tests avoid computing and launch a useless ray
        // (by default the color is fully transparent, so, it doesn't affect the following composite).
        if(distance(u_cameraPos, f3RayExitPos_Ms) >= distance(u_cameraPos, f3RayEntryPos_Ms))
        {
#ifdef PREINTEGRATION
            float fEntryOpacity = samplePreIntegrationTable(f3RayEntryPos_Ms, f3RayEntryPos_Ms + f3RayDir_MsN).a;
#else // PREINTEGRATION
            float fEntryIntensity = texture(u_image, f3RayEntryPos_Ms).r;
            float fEntryOpacity = sampleTransferFunction(fEntryIntensity, u_s1TFTexture, u_f2TFWindow).a;
#endif // PREINTEGRATION
            // If the ray is not in an importance zone, modulations and effects are applied.
            // Else, the ray is just launch normaly.
            if(f4ImportancePos_Ms.a <=  0. && fEntryOpacity > 0.)
            {
                vec4 f4Col;
#if CSG_DEPTH_LINES == 1 || CSG_BORDER == 1 || CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6 || CSG_OPACITY_DECREASE
                float fCSGDepthLen_Ts = texelFetch(u_csgInfos, ivec2(gl_FragCoord.xy), 0).w;
                float fCSGDepthMaxLen_Ts = 0.;

                // Find the maximum depth on the cone
                {
                    vec2 f2TargetPos_Ss = gl_FragCoord.xy;
                    vec2 f2ClostestPos_Ss = ndcToFragCoord(texelFetch(u_JFA, ivec2(gl_FragCoord.xy), 0).xyz).xy;
                    vec2 f2ClostestTof2TargetPos_SsN = normalize(f2TargetPos_Ss - f2ClostestPos_Ss);

                    vec4 f4CSGInfosTemp = texelFetch(u_csgInfos, ivec2(f2TargetPos_Ss), 0);
#ifdef PREINTEGRATION
                    while(f4CSGInfosTemp.w > 0. && samplePreIntegrationTable(f4CSGInfosTemp.xyz, f4CSGInfosTemp.xyz + f3RayDir_MsN).a > 0.)
#else // PREINTEGRATION
                    while(f4CSGInfosTemp.w > 0. && sampleTransferFunction(texture(u_image, f4CSGInfosTemp.xyz).r, u_s1TFTexture, u_f2TFWindow).a > 0.)
#endif // PREINTEGRATION
                    {
                        fCSGDepthMaxLen_Ts = f4CSGInfosTemp.w;
                        f2TargetPos_Ss += f2ClostestTof2TargetPos_SsN;
                        f4CSGInfosTemp = texelFetch(u_csgInfos, ivec2(f2TargetPos_Ss), 0);
                    }
                }
#endif // CSG_DEPTH_LINES == 1 || CSG_BORDER == 1 || CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6 || CSG_OPACITY_DECREASE

#if CSG_DEPTH_LINES == 1 || CSG_BORDER == 1
                float fDepthLineLen_Ts = fCSGDepthMaxLen_Ts - fCSGDepthLen_Ts;
                float fCSGDepthIntegralPart_Ts;
                float fCSGDepthFractPart_Ts = modf(fDepthLineLen_Ts, fCSGDepthIntegralPart_Ts);
#endif // CSG_DEPTH_LINES == 1 || CSG_BORDER == 1
#ifdef CSG_LIGHTING
                // Compute the CSG's normal
                vec4 f4ClosetMaskPos_Ns = texelFetch(u_JFA, ivec2(gl_FragCoord.xy), 0);
                vec3 f3ClosetMaskPos_Ms = ndcToSpecificSpacePosition(f4ClosetMaskPos_Ns.xyz, m4Cs_Ms).xyz;

                vec3 f3ClosetPointToCam_Ms = u_cameraPos-f3ClosetMaskPos_Ms;
                vec3 f3ClosetPointToEntry_Ms = f3RayEntryPos_Ms-f3ClosetMaskPos_Ms;

                vec3 f3NormDir_Ms = cross(f3ClosetPointToCam_Ms, f3ClosetPointToEntry_Ms);
                vec3 f3ConeNormalDir_MsN = normalize(cross(f3ClosetPointToEntry_Ms, f3NormDir_Ms));
#else
                // Else, use the gradient
                vec3 f3ConeNormalDir_MsN = gradientNormal(f3RayEntryPos_Ms);
#endif // CSG_LIGHTING
#ifdef CSG_BORDER
                // First CSG border
                if(fDepthLineLen_Ts > 0. && fDepthLineLen_Ts < u_csgBorderWidth)
                {
                    gl_FragDepth = firstOpaqueRayInfos(f3RayEntryPos_Ms, f3RayDir_MsN, fRayLen, u_sampleDistance, u_s1TFTexture, u_f2TFWindow).m_fRayDepth_Ss;
                    f4Col = vec4(lighting(f3ConeNormalDir_MsN, f3RayEntryPos_Ms, u_csgBorderColor), 1.);
                }
                else
#endif // CSG_BORDER
#ifdef CSG_DEPTH_LINES
                // Depth lines inside the SCG.
                if(fDepthLineLen_Ts > 0. && int(mod(fCSGDepthIntegralPart_Ts, u_csgDepthLinesSpacing)) == 0. && (fCSGDepthFractPart_Ts < u_csgDepthLinesWidth))
                {
                    gl_FragDepth = firstOpaqueRayInfos(f3RayEntryPos_Ms, f3RayDir_MsN, fRayLen, u_sampleDistance, u_s1TFTexture, u_f2TFWindow).m_fRayDepth_Ss;
                    f4Col = vec4(lighting(f3ConeNormalDir_MsN, f3RayEntryPos_Ms, u_csgDepthLinesColor), 1.);
                }
                else
#endif // CSG_DEPTH_LINES
                {
#ifdef CSG_LIGHTING
                    // Use the CSG's normal to light the first voxex in the countersink
#ifdef PREINTEGRATION
                    vec4 f4TFCol = samplePreIntegrationTable(f3RayEntryPos_Ms, f3RayEntryPos_Ms + f3RayDir_MsN);
#else // PREINTEGRATION
                    float fIntensity = texture(u_image, f3RayEntryPos_Ms).r;
                    vec4 f4TFCol = sampleTransferFunction(fIntensity, u_s1TFTexture, u_f2TFWindow);
#endif // PREINTEGRATION
                    f4Col = vec4(lighting(f3ConeNormalDir_MsN, f3RayEntryPos_Ms, f4TFCol.rgb), f4TFCol.a);

                    fRayLen = length(f3RayExitPos_Ms - (f3RayEntryPos_Ms+f3RayDir_MsN));
                    composite(f4Col, launchRay(f3RayEntryPos_Ms+f3RayDir_MsN, f3RayDir_MsN, fRayLen, u_sampleDistance, u_s1TFTexture, u_f2TFWindow));
                    gl_FragDepth = firstOpaqueRayInfos(f3RayEntryPos_Ms, f3RayDir_MsN, fRayLen, u_sampleDistance, u_s1TFTexture, u_f2TFWindow).m_fRayDepth_Ss;
#else
                    // Else, juste laucn the ray
                    f4Col = launchRay(f3RayEntryPos_Ms, f3RayDir_MsN, fRayLen, u_sampleDistance, u_s1TFTexture, u_f2TFWindow);
#endif // CSG_LIGHTING
                }

// CSG grayscale
#if CSG_GRAYSCALE == 1
                // The luminosity method is a more sophisticated version of the average method.
                // It also averages the values, but it forms a weighted average to account for human perception.
                // We’re more sensitive to green than other colors, so green is weighted most heavily.
                float fGrayScale = 0.21 * f4Col.r + 0.72 * f4Col.g + 0.7 * f4Col.b;
                f4Col.rgb = vec3(fGrayScale);
#endif // CSG_GRAYSCALE == 1

// CSG luminance and saturation modulations
// Brightness increase (CSG_MODULATION == 4)
// Saturation increase (CSG_MODULATION == 5)
// Saturation and brightness increase (CSG_MODULATION == 6)
#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6 || CSG_OPACITY_DECREASE
                float fModulationRatio = fCSGDepthLen_Ts/fCSGDepthMaxLen_Ts;
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6 || CSG_OPACITY_DECREASE

#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6
                vec3 hsv = rgb2hsv(f4Col.rgb);
#if CSG_MODULATION == 5 || CSG_MODULATION == 6
                hsv.g += (1.0-fModulationRatio)*u_csgColorModulationFactor;
#endif // CSG_MODULATION == 5 || CSG_MODULATION == 6

#if CSG_MODULATION == 4 || CSG_MODULATION == 6
                hsv.b += (1.0-fModulationRatio)*u_csgColorModulationFactor;
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 6

                f4Col.rgb = hsv2rgb(hsv);
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6

#ifdef CSG_OPACITY_DECREASE
                f4Col.a -= fModulationRatio*u_csgOpacityDecreaseFactor;
#endif // CSG_OPACITY_DECREASE

                f4ResultCol = f4Col;
            }
            else
            {
#ifdef CSG_DISABLE_CONTEXT
                if(fEntryOpacity > 0.)
                {
                    f4ResultCol = launchRay(f3RayEntryPos_Ms, f3RayDir_MsN, fRayLen, u_sampleDistance, u_s1TFTexture, u_f2TFWindow);
                }
#else
                f4ResultCol = launchRay(f3RayEntryPos_Ms, f3RayDir_MsN, fRayLen, u_sampleDistance, u_s1TFTexture, u_f2TFWindow);
#endif // CSG_DISABLE_CONTEXT
            }
        }
#ifdef CSG_TF
        // If the second TF usage is enable, we fill the CSG with it.
        if(distance(u_cameraPos, f3RayExitPos_Ms) >= distance(u_cameraPos, f3RayCSGEntryPos_Ms))
        {
            float fRayCSGLen;
            // Launch the ray until it hit the nearest exit point (the exit point in the volume or the mesh if they are mixed rendering)
            if(distance(u_cameraPos, f3RayExitPos_Ms) < distance(u_cameraPos, f3RayEntryPos_Ms))
            {
                fRayCSGLen = length(f3RayExitPos_Ms - f3RayCSGEntryPos_Ms);
            }
            else
            {
                fRayCSGLen = length(f3RayEntryPos_Ms - f3RayCSGEntryPos_Ms);
            }
            vec4 colorCSG = launchRay(f3RayCSGEntryPos_Ms, f3RayDir_MsN, fRayCSGLen, u_sampleDistance, u_CSGTFTexture, u_CSGTFWindow);
            composite(colorCSG, f4ResultCol);
            f4ResultCol = colorCSG;
        }
#endif // CSG_TF
    }
    else
    {
        f4ResultCol = launchRay(f3RayEntryPos_Ms, f3RayDir_MsN, fRayLen, u_sampleDistance, u_s1TFTexture, u_f2TFWindow);
    }
#endif // CSG

    v_f4FragCol = f4ResultCol;
}
