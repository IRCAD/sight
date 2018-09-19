#version 330

uniform ivec3 u_brickSize;

uniform int u_slice;

uniform sampler3D u_image;

out float o_brickMax;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float intensity);

//-----------------------------------------------------------------------------

void main()
{
    ivec3 gridPos = ivec3(floor(gl_FragCoord.xy), u_slice); // const

    ivec3 imageResolution = textureSize(u_image, 0); // const
    ivec3 imagePos        = gridPos * u_brickSize; // const

    ivec3 brickBeginPosition = max(imagePos, ivec3(0)); // const
    ivec3 brickEndPosition   = min(imagePos + u_brickSize, imageResolution); // const

    bool brickMax = false;
    for(int u = brickBeginPosition.x; u < brickEndPosition.x; ++ u)
    {
        for(int v = brickBeginPosition.y; v < brickEndPosition.y; ++ v)
        {
            for(int w = brickBeginPosition.z; w < brickEndPosition.z; ++ w)
            {
                float intensity    = texelFetch(u_image, ivec3(u, v, w), 0).r; // const

                // We only want to check if the voxel is not empty meaning its extinction coefficient is zero.
                // Therefore there is NO NEED TO CALCULATE THE EXTINCTION COEFFICIENT here since it is equal to zero
                // if and only if the opacity is equal to zero.
                float voxelOpacity = sampleTransferFunction(intensity).a; // const

                brickMax = brickMax || voxelOpacity != 0;
            }
        }
    }

    o_brickMax = float(brickMax);
}
