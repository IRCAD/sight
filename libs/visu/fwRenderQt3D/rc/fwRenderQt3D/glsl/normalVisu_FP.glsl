#version 330 core

out vec4 f_out_f4FragColor;

const vec4 f4NormalCol = vec4(1.0, 1.0, 0.0, 1.0);

void main()
{
    f_out_f4FragColor = f4NormalCol;
}