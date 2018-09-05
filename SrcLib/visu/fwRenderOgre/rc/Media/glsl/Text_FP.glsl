#version 150

uniform sampler2D u_fontMap;
uniform vec4 u_textColor;

in vec2 oTexCoord;

out vec4 fragColor;

//-----------------------------------------------------------------------------

void main(void)
{
    vec4 color = texture(u_fontMap, oTexCoord);

    fragColor = color * u_textColor;
}
