#version 330
out vec4 FragColor;
uniform vec4 u_diffuse;

vec4 packFloatToVec4(float value);

void main()
{
    if(u_diffuse.a == 0.)
        discard;
    FragColor = packFloatToVec4(gl_FragCoord.z);
}
