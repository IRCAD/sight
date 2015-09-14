#version 330
out vec4 FragColor;
uniform vec4 u_diffuseColor;

void main()
{
    if(u_diffuseColor.a == 0.)
        discard;
    FragColor.r = gl_FragCoord.z;
}
