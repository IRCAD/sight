#version 150

out vec4 fragColor;

vec4 getMaterialColor();

void main(void)
{
    fragColor = getMaterialColor();
}
