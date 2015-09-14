#version 150

#ifdef FLAT
flat in vec4 oColor;
#else
in vec4 oColor;
#endif

vec4 fetch_texture();

out vec4 fragColor;

void main(void)
{
    fragColor = oColor * fetch_texture();
}
