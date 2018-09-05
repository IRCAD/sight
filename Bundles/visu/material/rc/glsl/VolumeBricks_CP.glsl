#version 430

layout(local_size_x=1, local_size_y=1, local_size_z=1) in;
layout(binding = 0, r16) readonly uniform image3D u_image;
layout(binding = 1, r16) writeonly uniform image3D u_brickGrid;

uniform ivec3 u_voxelsPerCell;
//uniform ivec3 u_imageResolution;

vec4 sampleTransferFunction(float intensity);

void main()
{
    ivec3 imageResolution = imageSize(u_image);

    ivec3 gridVoxelPosition = ivec3(gl_WorkGroupID);
    ivec3 volumeBrickPosition = gridVoxelPosition * u_voxelsPerCell;

    ivec3 brickBeginPosition = volumeBrickPosition;
    ivec3 brickEndPosition   = min(volumeBrickPosition + u_voxelsPerCell, imageResolution);

    float brickMax = 0;
    for(int i = brickBeginPosition.x; i < brickEndPosition.x; ++ i)
    {
        for(int j = brickBeginPosition.y; j < brickEndPosition.y ++ j)
        {
            for(int k = brickBeginPosition.z; k < brickEndPosition.z; ++ k)
            {
                float intensity    = imageLoad(u_image, ivec3(i, j, k)).r;
                float voxelOpacity = sampleTransferFunction(intensity).a;
                brickMax = max(brickMax, voxelOpacity);
            }
        }
    }

    imageStore(u_brickGrid, gridVoxelPosition, ivec4(brickMax));
}
