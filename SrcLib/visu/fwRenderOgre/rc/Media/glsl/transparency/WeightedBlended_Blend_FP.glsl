#version 330
in vec2 uv;
uniform sampler2D u_sumColorBuffer;
uniform sampler2D u_transmittanceBuffer;
out vec4 FragColor;

/** Final shader :
  * Blend scene with blended object
  **/
void main()
{
    vec4 sumColor = texture(u_sumColorBuffer, uv);
    float transmittance = texture(u_transmittanceBuffer, uv).r;
    vec3 averageColor = sumColor.rgb / max(sumColor.a, 0.00001);

    FragColor.rgb  = averageColor * (1. - transmittance);
    FragColor.a  = 1.-transmittance;
}
