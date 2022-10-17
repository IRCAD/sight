#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "Lighting.inc.glsl"
#include "VolumeNormals.inc.glsl"

#if defined(AMBIENT_OCCLUSION) || defined(COLOR_BLEEDING)
uniform sampler3D u_s3IlluminationVolume;
uniform vec4 u_f4VolIllumFactor;
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING

//-----------------------------------------------------------------------------

vec3 computeLighting(in vec3 _f3SampleColor, in vec3 _f3SamplePos_Ms, in sampler3D _s3Image)
{
    vec3 sampleNormal_MsN = gradientNormal(_f3SamplePos_Ms, _s3Image);

    vec3 shadedSampleColor = (length(sampleNormal_MsN) > 0)
                             ? lightingBlinnPhong(sampleNormal_MsN, _f3SamplePos_Ms, _f3SampleColor)
                             : _f3SampleColor;

#if defined(AMBIENT_OCCLUSION) || defined(COLOR_BLEEDING)
    vec4 volumeIllumination = texture(u_s3IlluminationVolume, _f3SamplePos_Ms);
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

#if defined(AMBIENT_OCCLUSION)
    // Apply ambient occlusion + shadows
    shadedSampleColor *= pow(exp(-volumeIllumination.a), u_f4VolIllumFactor.a);
#endif // AMBIENT_OCCLUSION || SHADOWS

#if defined(COLOR_BLEEDING)
    // Apply color bleeding
    shadedSampleColor *= pow(1 + volumeIllumination.rgb, u_f4VolIllumFactor.rgb);
#endif // COLOR_BLEEDING

    return shadedSampleColor;
}
