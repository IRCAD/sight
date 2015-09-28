#version 330
in vec2 uv;
uniform sampler2D u_backColorRGBuffer;
uniform sampler2D u_backColorBABuffer;
out vec4 FragColor;

vec2 unpackColor(float value);

/** Blend shader :
  * Blend front passes
  **/
void main()
{
    FragColor.ba = unpackColor(texture(u_backColorBABuffer, uv).r);
    if (FragColor.a == 0.) discard;
    FragColor.rg = unpackColor(texture(u_backColorRGBuffer, uv).r);
}
