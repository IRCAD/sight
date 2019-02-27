#version 330

vec4 sampleTransferFunction(float _fIntensity, in sampler1D _s1Sampler, in vec2 _f2Window)
{
    float intIntensity = _fIntensity * 65535.f - 32768.f;
    float scaledValue = ((intIntensity - _f2Window.x) / (_f2Window.y - _f2Window.x));

    return texture(_s1Sampler, scaledValue);
}

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction_uint16(int _fIntensity, in sampler1D _s1Sampler, in vec2 _f2Window)
{
    float intIntensity = _fIntensity ;
    float scaledValue = ((intIntensity - _f2Window.x) / (_f2Window.y - _f2Window.x));

    return texture(_s1Sampler, scaledValue);
}

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction_float(float _fIntensity, in sampler1D _s1Sampler, in vec2 _f2Window)
{
    float intIntensity = _fIntensity ;
    float scaledValue = ((intIntensity - _f2Window.x) / (_f2Window.y - _f2Window.x));

    return texture(_s1Sampler, scaledValue);
}
