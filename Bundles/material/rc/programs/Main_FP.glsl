#version 150

out vec4 fragColor;

vec4 getFragmentColor();

void main(void)
{
    fragColor = getFragmentColor();
}
