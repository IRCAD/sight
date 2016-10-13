#version 150

uniform sampler3D u_image;

uniform float u_sliceDepth;

in vec2 uv;

out vec4 fragColor;

//-----------------------------------------------------------------------------
vec4 sampleTransferFunction(float intensity);

//-----------------------------------------------------------------------------

void main(void)
{
    fragColor = sampleTransferFunction(texture(u_image, vec3(uv, u_sliceDepth)).r);
}
