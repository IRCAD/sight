#version 150

uniform sampler2D u_defaultTex;
in vec2 oTexCoord;

vec4 fetch_texture()
{
    return texture(u_defaultTex, oTexCoord);
}
