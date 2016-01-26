#version 330

out vec4 FragColor;

vec4 packFloatToVec4(float value);

void main()
{
    FragColor = packFloatToVec4(1.0);
}
