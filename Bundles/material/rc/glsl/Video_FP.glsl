#version 330

#ifdef TF

// Defined in TransferFunction.glsl
vec4 sampleTransferFunction_uint16(int intensity);

uniform isampler2D u_videoTexture;

#else

uniform sampler2D u_videoTexture;

#endif

in vec2 oTexCoord;

out vec4 fragColor;

//-----------------------------------------------------------------------------

void main(void)
{
    vec4 color;

#ifdef TF
    int value = texture(u_videoTexture, oTexCoord).r;
    color = sampleTransferFunction_uint16(value);
#else
    color = texture(u_videoTexture, oTexCoord);
#endif

    fragColor = color;
}
