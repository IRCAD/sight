#version 410

uniform mat4 u_worldView;
uniform mat4 u_proj;
uniform vec4 u_diffuse;

in vec4 vertex;

layout(location = 2) out vec4 outColor;

void main()
{
    vec4 position = u_worldView * vertex;
    position.w += 0.01;
    gl_Position = u_proj * position;

    // Default to black, only keep opacity setting
    outColor = vec4(0, 0, 0, u_diffuse.a);
}
