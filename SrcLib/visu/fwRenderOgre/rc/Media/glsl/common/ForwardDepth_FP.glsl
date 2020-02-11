#version 330

uniform sampler2D u_s2Depth;
in vec2 uv;

layout (location = 1) out float f_fFragCol;

void main()
{
    f_fFragCol = texture(u_s2Depth, uv).r;
}
