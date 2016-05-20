#version 430

layout(local_size_x=1, local_size_y=1, local_size_z=1) in;
layout(binding = 0, r16) readonly uniform image3D u_image;
layout(binding = 1, r16) writeonly uniform image3D u_brickGrid;

uniform sampler2D u_tfTexture;

uniform ivec3 u_voxelsPerCell;
//uniform ivec3 u_imageResolution;

vec4 transferFunction(float intensity)
{
    float scaledValue = intensity * 65535.f;

    // Computes 2D indices from the hounsfield value
    int j = int( scaledValue / 256 );
    int i = int( mod( int(scaledValue), 256 ) );

    // Converts the indices into texture uv coordinates
    vec2 uvTF = vec2(i / 255.f, j / 255.f);

    return texture(u_tfTexture, uvTF);
}


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
                float voxelOpacity = transferFunction(intensity).a;
                brickMax = max(brickMax, voxelOpacity);
            }
        }
    }

    imageStore(u_brickGrid, gridVoxelPosition, ivec4(brickMax));
}
