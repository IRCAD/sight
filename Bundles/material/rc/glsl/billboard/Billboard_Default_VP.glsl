#version 150

in vec3 vertex;

uniform mat4 u_worldView;

void main()
{
    gl_Position = u_worldView * vec4(vertex, 1.);
}
