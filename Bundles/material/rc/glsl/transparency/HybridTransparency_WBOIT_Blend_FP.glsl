#version 330
in vec2 uv;
uniform sampler2D u_fragData0;
uniform sampler2D u_fragData1;
uniform vec4 u_diffuse;
out vec4 FragColor;

/** Final shader :
  * Blend scene with blended object
  **/
void main()
{
	vec4 sumColor = texture(u_fragData0, uv);
	float transmittance = texture(u_fragData1, uv).r;
    vec3 averageColor = sumColor.rgb / max(sumColor.a, 0.00001);

    FragColor.rgb  = averageColor * (1. - transmittance);
    FragColor.a  = 1. - transmittance;
}
