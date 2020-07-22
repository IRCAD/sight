#version 330 core

in vec3 f3_in_position;
in vec3 f3_in_normal;

out vec4 f4_out_fragColor;

void main()
{
    f4_out_fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}