#version 330

#ifdef TF

uniform sampler1D u_tfTexture;
uniform vec2 u_tfWindow;

// Defined in TransferFunction.glsl
vec4 sampleTransferFunction_uint16(int _intensity, in sampler1D _sampler, in vec2 _window);
vec4 sampleTransferFunction_float(float _intensity, in sampler1D _sampler, in vec2 _window);

#   ifdef TF_INTEGER

uniform isampler2D u_videoTexture;
#   else // TF_INTEGER
uniform sampler2D u_videoTexture;
#   endif // TF_INTEGER

#else // TF

uniform sampler2D u_videoTexture;

#endif // TF

in vec2 oTexCoord;

vec4 getFragmentColor()
{
    vec4 color;

#ifdef TF

#   ifdef TF_INTEGER
    int value = texture(u_videoTexture, oTexCoord).r;
    color = sampleTransferFunction_uint16(value, u_tfTexture, u_tfWindow);
#   else // TF_INTEGER
    float value = texture(u_videoTexture, oTexCoord).r;
    color = sampleTransferFunction_float(value, u_tfTexture, u_tfWindow);
#   endif // TF_INTEGER
#else // TF
    color = texture(u_videoTexture, oTexCoord);
#endif // TF

    return color;
}

float getFragmentAlpha()
{
    vec4 color;

#ifdef TF

#   ifdef TF_INTEGER
    int value = texture(u_videoTexture, oTexCoord).r;
    color = sampleTransferFunction_uint16(value, u_tfTexture, u_tfWindow);
#   else // TF_INTEGER
    float value = texture(u_videoTexture, oTexCoord).r;
    color = sampleTransferFunction_float(value, u_tfTexture, u_tfWindow);
#   endif // TF_INTEGER
#else // TF
    color = texture(u_videoTexture, oTexCoord);
#endif // TF

    return color.a;
}
