#version 330

uniform mat4 u_worldViewProj;

in vec3 position;

out vec3 localPosition;

void main()
{
	localPosition = position;
    gl_Position = u_worldViewProj * vec4(position, 1.);
}
