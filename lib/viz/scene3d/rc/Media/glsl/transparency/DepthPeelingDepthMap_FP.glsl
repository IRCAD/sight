#version 330

out vec4 FragColor;
uniform vec4 u_diffuse;

// Writing depth directly into a R32F render target

void main()
{
    if(u_diffuse.a == 0.)
        discard;
    FragColor = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
