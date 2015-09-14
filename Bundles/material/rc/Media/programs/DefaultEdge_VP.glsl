#version 150

uniform mat4 u_worldView;
uniform mat4 u_proj;
in vec4 vertex;

void main()
{
    vec4 position = u_worldView * vertex;
    position.w += 0.01;
    gl_Position = u_proj * position;
}
