#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

// cspell:ignore vray

#include "VolumeRay.inc.glsl"
#include "VolumeSampling.inc.glsl"
#include "VolumeLighting.inc.glsl"

//-----------------------------------------------------------------------------

void blendComposite(inout vec4 _destColor, in vec4 _srcColor)
{
    // Front-to-back blending
    _destColor.rgb = _destColor.rgb + (1 - _destColor.a) * _srcColor.a * _srcColor.rgb;
    _destColor.a   = _destColor.a   + (1 - _destColor.a) * _srcColor.a;
}

//-----------------------------------------------------------------------------

void advanceUntilOpaque(inout VolumeRay _vray_Ms, in sampler3D _s3Image, in sampler3D _s1Mask)
{
    // Move the ray to the first non transparent voxel.
    for(; !vrayTerminated(_vray_Ms); vrayAdvance(_vray_Ms))
    {
        float sampleAlpha = sampleVolume(_s3Image, _s1Mask, _vray_Ms).a;
        if(sampleAlpha != 0)
        {
            break;
        }
    }
}

//-----------------------------------------------------------------------------

vec4 compositeAlongRay(inout VolumeRay _vray_Ms, in sampler3D _s3Image, in sampler3D _s1Mask)
{
    vec4 rayColor = vec4(0.);
    for(; !vrayTerminated(_vray_Ms); vrayAdvance(_vray_Ms))
    {
        vec4 sampleColor = sampleVolume(_s3Image, _s1Mask, _vray_Ms);

        if(sampleColor.a > 0.)
        {
            vec3 rayPos_Ms = _vray_Ms.position;

            sampleColor.rgb = computeLighting(sampleColor.rgb, rayPos_Ms, _s3Image);

            blendComposite(rayColor, sampleColor);

            if(rayColor.a > 0.99)
            {
                break;
            }
        }
    }

    return rayColor;
}
