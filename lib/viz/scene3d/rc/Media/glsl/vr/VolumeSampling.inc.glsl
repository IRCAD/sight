#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "TransferFunction.inc.glsl"

#ifdef PREINTEGRATION
uniform sampler2D u_s2PreintegratedTFTexture;
uniform int u_iMinImageValue;
uniform int u_iMaxImageValue;
#else // PREINTEGRATION
uniform sampler2D u_s2TFTexture;
uniform vec3 u_f3TFWindow;
uniform float u_fOpacityCorrectionFactor;
#endif // PREINTEGRATION


//-----------------------------------------------------------------------------

#ifdef PREINTEGRATION
vec4 samplePreIntegrationTable(in sampler3D _s3Image, in VolumeRay _ray_Ms)
{
    vec3 rayBack_Ms  = _ray_Ms.position;
    vec3 rayFront_Ms = rayBack_Ms + _ray_Ms.direction;

    float sf = texture(_s3Image, rayBack_Ms).r;
    float sb = texture(_s3Image, rayFront_Ms).r;

    sf = ((sf * u_window.y) - float(u_iMinImageValue) + u_window.x) / float(u_iMaxImageValue - u_iMinImageValue);
    sb = ((sb * u_window.y) - float(u_iMinImageValue) + u_window.x) / float(u_iMaxImageValue - u_iMinImageValue);

    return texture(u_s2PreintegratedTFTexture, vec2(sf, sb));
}
#endif // PREINTEGRATION

//-----------------------------------------------------------------------------

vec4 sampleVolume(in sampler3D _s3Image, in sampler3D u_s1Mask, in VolumeRay _ray_Ms)
{
    float mask = texture(u_s1Mask, _ray_Ms.position).r;
    if(mask < 0.5 )
        return vec4(0.);
#ifdef PREINTEGRATION
    vec4 sampleColor = samplePreIntegrationTable(_s3Image, _ray_Ms);
#else // PREINTEGRATION
    float voxelIntensity = texture(_s3Image, _ray_Ms.position).r;
    vec4 sampleColor = sampleTransferFunction(voxelIntensity, u_s2TFTexture, u_f3TFWindow);

    // Opacity correction.
    // TODO: compute this when generating the GPU transfer function to improve performance.
    sampleColor.a = 1 - pow(1 - sampleColor.a, _ray_Ms.stepLength * u_fOpacityCorrectionFactor);
#endif // PREINTEGRATION

    return sampleColor;
}
