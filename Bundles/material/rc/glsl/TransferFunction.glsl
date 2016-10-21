#version 330

uniform sampler2D u_tfTexture;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float intensity)
{
    float scaledValue = intensity * 65535.f;

    // Computes 2D indices from the hounsfield value
    vec2 uv;
    uv.y = floor( scaledValue / 256.f );
    uv.x = floor( scaledValue - uv.y * 256.f );

    return texelFetch(u_tfTexture, ivec2(uv), 0);
}
