#version 330

uniform float u_alpha;

out vec4 fragColor;

void main()
{
    fragColor.a = u_alpha;
}
