#version 330

uniform ivec3 u_gridResolution;
uniform ivec3 u_brickSize;

uniform int u_slice;

uniform sampler3D u_image;
uniform sampler2D u_tfTexture;

out float o_brickMax;

vec4 transferFunction(float intensity)
{
    float scaledValue = intensity * 65535.f;

    // Computes 2D indices from the hounsfield value
    int j = int( scaledValue / 256 ); // const
    int i = int( mod( int(scaledValue), 256 ) ); // const

    // Converts the indices into texture uv coordinates
    vec2 uvTF = vec2(i / 255.f, j / 255.f);

    return texture(u_tfTexture, uvTF);
}

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
                float voxelOpacity = transferFunction(intensity).a; // const

                brickMax = brickMax || voxelOpacity != 0;
            }
        }
    }

    o_brickMax = float(brickMax);
}
