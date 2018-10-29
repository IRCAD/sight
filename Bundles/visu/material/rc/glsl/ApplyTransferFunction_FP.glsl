#version 150

uniform sampler3D u_image;
uniform sampler1D u_tfTexture;
uniform vec2 u_tfWindow;

uniform float u_sliceDepth;
uniform float u_sampleDistance;

in vec2 uv;

out vec4 fragColor;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float _intensity, in sampler1D _sampler, in vec2 _window);

//-----------------------------------------------------------------------------

void main(void)
{
    vec4 sampledColor = sampleTransferFunction(texture(u_image, vec3(uv, u_sliceDepth)).r, u_tfTexture, u_tfWindow);
    sampledColor.a = -log(1-min(sampledColor.a, 0.999));
    fragColor = sampledColor;
}
