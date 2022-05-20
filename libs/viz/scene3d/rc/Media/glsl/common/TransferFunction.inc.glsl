vec4 sampleTransferFunction(float _fIntensity, in sampler1D _s1Sampler, in vec3 _f3Window)
{
    float intIntensity = (_fIntensity * 65535.f - 32768.f);
    float scaledValue = ((intIntensity - _f3Window.x) / (_f3Window.y - _f3Window.x));

    // Since the texture is likely to be bigger than the actual data, we rescale and clamp manually
    // See sight::viz::scene3d::TransferFunction::updateTexture()
    return texture(_s1Sampler, min(scaledValue * _f3Window.z, _f3Window.z));
}

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction_uint16(int _fIntensity, in sampler1D _s1Sampler, in vec3 _f3Window)
{
    float intIntensity = _fIntensity ;
    float scaledValue = ((intIntensity - _f3Window.x) / (_f3Window.y - _f3Window.x));

    // Since the texture is likely to be bigger than the actual data, we rescale and clamp manually
    // See sight::viz::scene3d::TransferFunction::updateTexture()
    return texture(_s1Sampler, min(scaledValue * _f3Window.z, _f3Window.z));
}

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction_float(float _fIntensity, in sampler1D _s1Sampler, in vec3 _f3Window)
{
    float intIntensity = _fIntensity ;
    float scaledValue = ((intIntensity - _f3Window.x) / (_f3Window.y - _f3Window.x));

    // Since the texture is likely to be bigger than the actual data, we rescale and clamp manually
    // See sight::viz::scene3d::TransferFunction::updateTexture()
    return texture(_s1Sampler, min(scaledValue * _f3Window.z, _f3Window.z));
}
