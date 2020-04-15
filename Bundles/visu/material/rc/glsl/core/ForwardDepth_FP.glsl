#version 330

uniform sampler2D u_s2Depth;
in vec2 uv;

layout (location = 1) out float f_fFragDepth;

void main()
{
    f_fFragDepth = texture(u_s2Depth, uv).r;
}
