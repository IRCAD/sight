#version 150

in vec3 vertex;
in vec3 colour;

uniform mat4 u_worldView;

out vec4 g_f4PointCol;

void main()
{
    g_f4PointCol = vec4(colour, 1.);
    gl_Position = u_worldView * vec4(vertex, 1.);
}
