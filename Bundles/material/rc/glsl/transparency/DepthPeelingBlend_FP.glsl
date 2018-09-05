#version 330
in vec2 uv;
uniform sampler2D u_fragData0;

out vec4 FragColor;

/** Blend shader :
  * Blend front passes
  **/
void main()
{
    FragColor = texture(u_fragData0, uv);
}
