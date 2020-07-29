#version 330 core

in vec3 v_out_f3Position;
in vec3 v_out_f3Normal;

out vec4 f_out_f4FragColor;

void main()
{
    f_out_f4FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}