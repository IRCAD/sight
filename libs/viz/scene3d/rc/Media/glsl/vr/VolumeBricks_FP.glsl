#version 420

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "TransferFunction.inc.glsl"

uniform vec3 u_f3TFWindow;

uniform ivec3 u_brickSize;

uniform int u_slice;

layout(binding=0) uniform sampler3D u_image;
layout(binding=1) uniform sampler3D u_s1Mask;
layout(binding=2) uniform sampler2D u_s2TFTexture;

out float o_brickMax;

//-----------------------------------------------------------------------------

void main()
{
    const ivec3 gridPos = ivec3(floor(gl_FragCoord.xy), u_slice);

    const ivec3 imageResolution = textureSize(u_image, 0);
    const ivec3 imagePos        = gridPos * u_brickSize;

    const ivec3 brickBeginPosition = max(imagePos, ivec3(0));
    const ivec3 brickEndPosition   = min(imagePos + u_brickSize, imageResolution);

    bool brickMax = false;
    for(int u = brickBeginPosition.x; u < brickEndPosition.x; ++ u)
    {
        for(int v = brickBeginPosition.y; v < brickEndPosition.y; ++ v)
        {
            for(int w = brickBeginPosition.z; w < brickEndPosition.z; ++ w)
            {
                const float mask    = texelFetch(u_s1Mask, ivec3(u, v, w), 0).r;
                if(mask < 0.5)
                    continue;
                const float intensity    = texelFetch(u_image, ivec3(u, v, w), 0).r;

                // We only want to check if the voxel is not empty meaning its extinction coefficient is zero.
                // Therefore there is NO NEED TO CALCULATE THE EXTINCTION COEFFICIENT here since it is equal to zero
                // if and only if the opacity is equal to zero.
                const float voxelOpacity = sampleTransferFunction(intensity, u_s2TFTexture, u_f3TFWindow).a;

                brickMax = brickMax || voxelOpacity != 0;
            }
        }
    }

    o_brickMax = float(brickMax);
}
