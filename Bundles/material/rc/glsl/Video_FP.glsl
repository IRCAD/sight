#version 330

#ifdef TF

// Defined in TransferFunction.glsl
vec4 sampleTransferFunction_uint16(int intensity);
vec4 sampleTransferFunction_float(float intensity);

#   ifdef TF_INTEGER

uniform isampler2D u_videoTexture;
#   else // TF_INTEGER
uniform sampler2D u_videoTexture;
#   endif // TF_INTEGER

#else // TF

uniform sampler2D u_videoTexture;

#endif // TF

in vec2 oTexCoord;

out vec4 fragColor;

//-----------------------------------------------------------------------------

void main(void)
{
    vec4 color;

#ifdef TF

#   ifdef TF_INTEGER
    int value = texture(u_videoTexture, oTexCoord).r;
    color = sampleTransferFunction_uint16(value);
#   else // TF_INTEGER
    float value = texture(u_videoTexture, oTexCoord).r;
    color = sampleTransferFunction_float(value);
#   endif // TF_INTEGER
#else // TF
    color = texture(u_videoTexture, oTexCoord);
#endif // TF

    fragColor = color;
}
