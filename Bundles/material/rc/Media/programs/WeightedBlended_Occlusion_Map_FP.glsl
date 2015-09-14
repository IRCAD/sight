#version 330
uniform vec4 u_diffuseColor;
out vec4 FragColor;

void main()
{
    if(u_diffuseColor.a != 1.)
	{
		discard;
	}

	FragColor.r = gl_FragCoord.z;
}
