#version 410

uniform sampler2D u_texture;

in vec4 oColor;
in vec2 oTexCoord;

vec4 getFragmentColor()
{
    return oColor * texture(u_texture, oTexCoord);
}
