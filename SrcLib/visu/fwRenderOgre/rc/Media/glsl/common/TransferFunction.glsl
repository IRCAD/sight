#version 330

uniform sampler1D u_tfTexture;
uniform vec2 u_tfWindow;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float intensity)
{
    float intIntensity = intensity * 65535.f - 32768.f;
    float scaledValue = ((intIntensity - u_tfWindow.x) / (u_tfWindow.y - u_tfWindow.x));

    return texture(u_tfTexture, scaledValue);
}

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction_uint16(int intensity)
{
    float intIntensity = intensity ;
    float scaledValue = ((intIntensity - u_tfWindow.x) / (u_tfWindow.y - u_tfWindow.x));

    return texture(u_tfTexture, scaledValue);
}

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction_float(float intensity)
{
    float intIntensity = intensity ;
    float scaledValue = ((intIntensity - u_tfWindow.x) / (u_tfWindow.y - u_tfWindow.x));

    return texture(u_tfTexture, scaledValue);
}
