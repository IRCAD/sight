#version 330

uniform sampler2D u_tfTexture;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float intensity)
{
    float scaledValue = intensity * 65535.f;

    // Computes 2D indices from the hounsfield value
    int j = int( scaledValue / 256 );
    int i = int( mod( int(scaledValue), 256 ) );

    return texelFetch(u_tfTexture, ivec2(i, j), 0);
}
