#version 150

uniform sampler3D u_image;

uniform float u_sliceDepth;
uniform float u_sampleDistance;

in vec2 uv;

out vec4 fragColor;

//-----------------------------------------------------------------------------
vec4 sampleTransferFunction(float intensity);

//-----------------------------------------------------------------------------

void main(void)
{
    vec4 sampledColor = sampleTransferFunction(texture(u_image, vec3(uv, u_sliceDepth)).r);
    sampledColor.a = 1 - exp(-sampledColor.a * u_sampleDistance);
    sampledColor.a = -log(1-min(sampledColor.a, 0.999));
    fragColor = sampledColor;
}
