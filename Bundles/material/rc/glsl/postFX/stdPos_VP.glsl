#version 150

uniform mat4 MVP;
in vec4 vertex;
out vec4 pos;
out vec2 uv;


void main()
{
    gl_Position = MVP * vertex;
    vec2 inPos = sign(vertex.xy);
    uv = (vec2(inPos.x, -inPos.y) + 1.0)/2.0;

    pos = gl_Position;
}

