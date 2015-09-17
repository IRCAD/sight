#version 150

uniform mat4 u_worldView;
uniform mat4 u_proj;
uniform vec4 u_diffuse;

in vec4 vertex;
out vec4 oColor;

void main()
{
    vec4 position = u_worldView * vertex;
    position.w += 0.01;
    gl_Position = u_proj * position;

    // Default to black, only keep opacity setting
    oColor = vec4(0, 0, 0, u_diffuse.a);
}
