#version 330
uniform vec4 u_diffuse;
out vec4 FragColor;

void main()
{
    if(u_diffuse.a != 1.)
	{
		discard;
	}

	FragColor.r = gl_FragCoord.z;
}
