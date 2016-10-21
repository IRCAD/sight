#version 410

uniform sampler3D u_src;
uniform int u_sliceIndex;

uniform int u_passOrientation;
uniform int u_readOffset;

in vec2 uv;

out vec4 fragColour;

void main(void)
{
    ivec3 voxelCoords = ivec3(gl_FragCoord.xy, u_sliceIndex);
    vec4 outColour = texelFetch(u_src, voxelCoords, 0);

    ivec3 addCoords = voxelCoords;

    for(int i = 1; i < TEXREADS; ++i)
    {
        addCoords[u_passOrientation] -= u_readOffset;

        outColour += texelFetch(u_src, addCoords, 0);
    }

    fragColour = outColour;
}
