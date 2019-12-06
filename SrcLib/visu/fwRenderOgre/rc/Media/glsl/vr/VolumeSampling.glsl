#version 330

#extension GL_GOOGLE_include_directive : enable // Needed for glslangValidator to handle include directives.

#include "VolumeRay.glsl.struct"

#ifdef PREINTEGRATION
uniform sampler2D u_s2PreintegratedTFTexture;
uniform int u_iMinImageValue;
uniform int u_iMaxImageValue;
#else // PREINTEGRATION
uniform sampler1D u_s1TFTexture;
uniform vec2 u_f2TFWindow;
uniform float u_fOpacityCorrectionFactor;
#endif // PREINTEGRATION

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float _fIntensity, in sampler1D _s1Sampler, in vec2 _f2Window);

//-----------------------------------------------------------------------------

#ifdef PREINTEGRATION
vec4 samplePreIntegrationTable(in sampler3D _volume, in VolumeRay _vray_Ms)
{
    vec3 rayBack_Ms  = _vray_Ms.position;
    vec3 rayFront_Ms = rayBack_Ms + _vray_Ms.direction;

    float sf = texture(_volume, rayBack_Ms).r;
    float sb = texture(_volume, rayFront_Ms).r;

    sf = ((sf * 65535.f) - float(u_iMinImageValue) - 32767.f) / float(u_iMaxImageValue - u_iMinImageValue);
    sb = ((sb * 65535.f) - float(u_iMinImageValue) - 32767.f) / float(u_iMaxImageValue - u_iMinImageValue);

    return texture(u_s2PreintegratedTFTexture, vec2(sf, sb));
}
#endif // PREINTEGRATION

//-----------------------------------------------------------------------------

vec4 sampleVolume(in sampler3D _volume, in VolumeRay _vray_Ms)
{
#ifdef PREINTEGRATION
    vec4 sampleColor = samplePreIntegrationTable(_volume, _vray_Ms);
#else // PREINTEGRATION
    float voxelIntensity = texture(_volume, _vray_Ms.position).r;
    vec4 sampleColor = sampleTransferFunction(voxelIntensity, u_s1TFTexture, u_f2TFWindow);

    // Opacity correction.
    // TODO: compute this when generating the GPU transfer function to improve performance.
    sampleColor.a = 1 - pow(1 - sampleColor.a, _vray_Ms.stepLength * u_fOpacityCorrectionFactor);
#endif // PREINTEGRATION

    return sampleColor;
}