#version 150

uniform sampler2D u_videoTexture;

in vec2 oTexCoord;

out vec4 fragColor;

void main(void)
{
    fragColor = texture(u_videoTexture, oTexCoord);
}
