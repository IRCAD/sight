#version 150

in vec2 uv;

uniform sampler2D background;

out vec4 fragColor;

void main()
{
    fragColor = texture(background, uv);
}