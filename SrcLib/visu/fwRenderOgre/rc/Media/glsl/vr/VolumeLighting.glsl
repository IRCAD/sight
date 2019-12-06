#version 330

#if ADVANCED_LIGHTING || COLOR_BLEEDING
uniform sampler3D u_s3IlluminationVolume;
uniform vec4 u_f4VolIllumFactor;
#endif // ADVANCED_LIGHTING || COLOR_BLEEDING

//-----------------------------------------------------------------------------

vec3 lightingBlinnPhong(vec3 _f3NormalDir_N, vec3 _f3Pos, vec3 _f3DiffuseCol);
vec3 gradientNormal(vec3 _f3Pos_Ms, sampler3D _s3Image);

//-----------------------------------------------------------------------------

vec3 computeLighting(in vec3 _sampleColor, in vec3 _samplePos_Ms, in sampler3D _volume)
{
    vec3 sampleNormal_MsN = gradientNormal(_samplePos_Ms, _volume);

    vec3 shadedSampleColor = lightingBlinnPhong(sampleNormal_MsN, _samplePos_Ms, _sampleColor);

#if ADVANCED_LIGHTING || COLOR_BLEEDING
    vec4 volumeIllumination = texture(u_s3IlluminationVolume, _samplePos_Ms);
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

#if ADVANCED_LIGHTING
    // Apply ambient occlusion + shadows
    shadedSampleColor *= pow(exp(-volumeIllumination.a), u_f4VolIllumFactor.a);
#endif // AMBIENT_OCCLUSION || SHADOWS

#ifdef COLOR_BLEEDING
    // Apply color bleeding
    shadedSampleColor *= pow(1 + volumeIllumination.rgb, u_f4VolIllumFactor.rgb);
#endif // COLOR_BLEEDING

    return shadedSampleColor;
}
