#version 330

vec4 sampleTransferFunction(float _intensity, in sampler1D _sampler, in vec2 _window)
{
    float intIntensity = _intensity * 65535.f - 32768.f;
    float scaledValue = ((intIntensity - _window.x) / (_window.y - _window.x));

    return texture(_sampler, scaledValue);
}

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction_uint16(int _intensity, in sampler1D _sampler, in vec2 _window)
{
    float intIntensity = _intensity ;
    float scaledValue = ((intIntensity - _window.x) / (_window.y - _window.x));

    return texture(_sampler, scaledValue);
}

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction_float(float _intensity, in sampler1D _sampler, in vec2 _window)
{
    float intIntensity = _intensity ;
    float scaledValue = ((intIntensity - _window.x) / (_window.y - _window.x));

    return texture(_sampler, scaledValue);
}
