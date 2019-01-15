#version 150

uniform sampler3D u_image;
uniform sampler1D u_s1TFTexture;
uniform vec2 u_f2TFWindow;

uniform float u_sliceDepth;
uniform float u_sampleDistance;

in vec2 uv;

out vec4 fragColor;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float _fIntensity, in sampler1D _s1Sampler, in vec2 _f2Window);

//-----------------------------------------------------------------------------

void main(void)
{
    vec4 sampledColor = sampleTransferFunction(texture(u_image, vec3(uv, u_sliceDepth)).r, u_s1TFTexture, u_f2TFWindow);
    sampledColor.a = -log(1-min(sampledColor.a, 0.999));
    fragColor = sampledColor;
}
