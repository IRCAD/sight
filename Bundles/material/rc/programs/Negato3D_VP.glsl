#version 150

uniform mat4 u_worldViewProj;
in vec4 position;

out vec2 uv;

uniform int u_orientation;

void main()
{
    gl_Position = u_worldViewProj * position;

    // sign return -1 for x<0, 0 for x=0 and 1 for x>1
    vec3 inPos = sign(position.xyz);

    if (u_orientation == 0) // Sagittal
    {
        uv = (vec2(inPos.z, inPos.y) + 1.0)/2.0;
    }
    else if (u_orientation == 1) // Frontal
    {
        uv = (vec2(inPos.x, inPos.z) + 1.0)/2.0;
    }
    else if (u_orientation == 2) // Axial
    {
        uv = (vec2(inPos.x, inPos.y) + 1.0)/2.0;
    }
}
