#version 150

uniform sampler2D u_videoTexture;
uniform vec4 u_textColor;

in vec2 oTexCoord;

out vec4 fragColor;

//-----------------------------------------------------------------------------

void main(void)
{
    vec4 color = texture(u_videoTexture, oTexCoord);

    fragColor = vec4(u_textColor.rgb, color.r * u_textColor.a);
}
