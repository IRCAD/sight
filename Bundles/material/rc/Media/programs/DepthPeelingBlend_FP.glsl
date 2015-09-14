#version 330
in vec2 uv;
uniform sampler2D u_fragData0;
uniform sampler2D u_fragData1;
out vec4 FragColor;

vec2 unpackColor(float buffer);

/** Blend shader :
  * Blend front passes
  **/
void main()
{
    FragColor = vec4(unpackColor(texture(u_fragData0, uv).r), unpackColor(texture(u_fragData1, uv).r));
}
