#version 150

uniform sampler3D u_image;
uniform sampler2D u_tfTexture;

uniform float u_sliceDepth;

in vec2 uv;

out vec4 fragColor;

//-----------------------------------------------------------------------------

vec4 transferFunction(float intensity)
{
    float scaledValue = intensity * 65535.f;

    // Computes 2D indices from the hounsfield value
    int j = int( scaledValue / 256 );
    int i = int( mod( int(scaledValue), 256 ) );

    // Converts the indices into texture uv coordinates
    vec2 uvTF = vec2(i / 255.f, j / 255.f);

    return texture(u_tfTexture, uvTF);
}

//-----------------------------------------------------------------------------

void main(void)
{
    fragColor = transferFunction(texture(u_image, vec3(uv, u_sliceDepth)).r);
}
