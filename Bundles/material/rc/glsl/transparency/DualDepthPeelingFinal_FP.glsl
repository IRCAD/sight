#version 330
in vec2 uv;

uniform sampler2D u_backColorBuffer;
uniform sampler2D u_frontColorRGBuffer;
uniform sampler2D u_frontColorBABuffer;
out vec4 FragColor;

vec2 unpackColor(float value);

/** Final shader :
  * Blend scene with blended object
  **/
void main()
{
    vec4 backColor = texture(u_backColorBuffer, uv);

    vec4 frontColor;
    frontColor.rg = unpackColor(texture(u_frontColorRGBuffer, uv).r);
    frontColor.ba = unpackColor(texture(u_frontColorBABuffer, uv).r);
    float alphaMultiplier = 1.0 - frontColor.a;

    // front + back
    FragColor = frontColor + backColor * alphaMultiplier;
}
