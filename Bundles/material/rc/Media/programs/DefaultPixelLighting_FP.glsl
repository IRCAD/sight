// requires OpenGL >= 3.2
#version 150

in vec3 oPosition_WS;
in vec3 oNormal_WS;

out vec4 fragColor;

vec4 lighting(vec3 _normal, vec3 _position);
vec4 fetch_texture();

void main(void)
{
    fragColor = lighting(normalize(oNormal_WS), oPosition_WS) * fetch_texture();
}
