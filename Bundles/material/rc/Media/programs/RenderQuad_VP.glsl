#version 330

in vec4 vertex;

uniform mat4 u_worldViewProj;

out vec2 uv;

void main()
{
    gl_Position = u_worldViewProj * vertex;
    vec2 inPos = sign(vertex.xy);
    uv = (vec2(inPos.x, -inPos.y) + 1.0)/2.0;
}
