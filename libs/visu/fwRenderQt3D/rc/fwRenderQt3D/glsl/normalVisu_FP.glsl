#version 330 core

out vec4 f4_out_fragColor;

const vec4 normalColor = vec4(1.0, 1.0, 0.0, 1.0);

void main()
{
    f4_out_fragColor = normalColor;
}