// requires OpenGL >= 3.2
#version 150

uniform mat4 u_worldViewProj;

in vec4 position;

out vec2 fractal_position;

void main()
{
    gl_Position = u_worldViewProj * position;
    fractal_position = vec2(-position.y, position.x);
    fractal_position -= vec2(0.50, 0);
    fractal_position *= 1.5;
}
