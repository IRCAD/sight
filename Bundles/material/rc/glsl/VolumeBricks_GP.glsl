#version 430

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

in VertexDataIn
{
    ivec3 vs_gridPos;
} vertexIn[];

uniform sampler3D u_bricksGrid;

uniform ivec3 u_imageResolution;
uniform ivec3 u_gridResolution;
uniform ivec3 u_brickSize;

uniform vec3 u_boundingBoxMin;
uniform vec3 u_boundingBoxMax;

out vec3 oPos;
out vec3 oNormal;

float getOpacity(in ivec3 voxel)
{
    return texelFetch(u_bricksGrid, voxel, 0).r;
}

bool isInsideBox(in ivec3 gridVoxel)
{
    // Brick positions in image space.
    vec3 v0  = vec3(gridVoxel) * (vec3(u_brickSize) / vec3(u_imageResolution));
    vec3 v1  = v0 + (vec3(u_brickSize) / vec3(u_imageResolution));

    return (all(greaterThan(v0, u_boundingBoxMin)) && all(lessThan(v0, u_boundingBoxMax))) ||
           (all(greaterThan(v1, u_boundingBoxMin)) && all(lessThan(v1, u_boundingBoxMax)));
}

void main()
{
    const ivec3 gridPos = vertexIn[0].vs_gridPos;

    if(getOpacity(gridPos) <= 0 || !isInsideBox(gridPos))
    {
        return;
    }

    const ivec3 faceNormals[6] = {
        ivec3(-1,  0,  0),
        ivec3( 0, -1,  0),
        ivec3( 0,  0, -1),
        ivec3( 1,  0,  0),
        ivec3( 0,  1,  0),
        ivec3( 0,  0,  1)
    };

    // Cube dimensions in image space.
    const vec3 cubeDimensions = vec3(u_brickSize) / vec3(u_imageResolution);

//    const vec3 imagePosMin = max(gridPos * cubeDimensions    , u_boundingBoxMin);
    const vec3 imagePos    = gridPos * cubeDimensions;
    const vec3 imagePosMin = max(imagePos,                  u_boundingBoxMin);
    const vec3 imagePosMax = min(imagePos + cubeDimensions, u_boundingBoxMax);

    if(imagePosMin.x >= imagePosMax.x ||
       imagePosMin.y >= imagePosMax.y ||
       imagePosMin.z >= imagePosMax.z )
    {
        return;
    }

    const vec3 faceVertices[6][4] = {
        {
            // Negative X
            vec3(imagePosMin.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMax.z)
        },
        {
            // Negative Y
            vec3(imagePosMin.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMax.z)
        },
        {
            // Negative Z
            vec3(imagePosMin.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMin.z)
        },
        {
            // Positive X
            vec3(imagePosMax.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMax.z)
        },
        {
            // Positive Y
            vec3(imagePosMin.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMax.z)
        },
        {
            // Positive Z
            vec3(imagePosMin.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMax.z)
        }
    };

    for(int i = 0; i < 6; ++i)
    {
        ivec3 normal = faceNormals[i];

        ivec3 neighbour = gridPos + normal;

        oNormal = vec3(normal);

        // Discard face if it's neighbour is not empty.
        // We only want to generate the outer hull.
        if(!isInsideBox(neighbour) || getOpacity(neighbour) == 0)
        {
            for(int j = 0; j < 4; ++j)
            {
                oPos = faceVertices[i][j];
                EmitVertex();
            }
            EndPrimitive();
        }

    }
}
