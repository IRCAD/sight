#version 330
out vec4 FragColor;
uniform vec4 u_diffuse;

void main()
{
    if(u_diffuse.a == 0.)
        discard;
    FragColor.r = gl_FragCoord.z;
}
