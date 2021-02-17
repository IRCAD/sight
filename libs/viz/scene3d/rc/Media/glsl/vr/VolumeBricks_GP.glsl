#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

uniform sampler3D u_bricksGrid;

uniform ivec3 u_imageResolution;
uniform ivec3 u_brickSize;

uniform vec3 u_boundingBoxMin;
uniform vec3 u_boundingBoxMax;

layout(location = 0) in ivec3 vs_gridPos[];

out vec3 oPos;

//-----------------------------------------------------------------------------

float getOpacity(in ivec3 voxel)
{
    return texelFetch(u_bricksGrid, voxel, 0).r;
}

//-----------------------------------------------------------------------------

bool isInsideBox(in ivec3 gridVoxel)
{
    // Brick positions in image space.
    vec3 brickSize = vec3(u_brickSize) / vec3(u_imageResolution);
    vec3 brickMin  = vec3(gridVoxel) * brickSize;
    vec3 brickMax  = brickMin + brickSize;

    // Check if the brick and the box intersect.
    return all(greaterThan(brickMax, u_boundingBoxMin)) && all(lessThan(brickMin, u_boundingBoxMax));
}

//-----------------------------------------------------------------------------

void main()
{
    ivec3 gridPos = vs_gridPos[0];

    // Stop now if the current position in the brick is empty or if it is out of the clipping box.
    if(getOpacity(gridPos) == 0 || !isInsideBox(gridPos))
    {
        return;
    }

    ivec3 faceNormals[6] = ivec3[6](
        ivec3(-1,  0,  0),
        ivec3( 0, -1,  0),
        ivec3( 0,  0, -1),
        ivec3( 1,  0,  0),
        ivec3( 0,  1,  0),
        ivec3( 0,  0,  1)
    );

    // Cube dimensions in image space.
    vec3 cubeDimensions = vec3(u_brickSize) / vec3(u_imageResolution);

    ivec3 gridPlusOnePos  = gridPos + ivec3(1);
    vec3 imagePosMin      = max(gridPos        * cubeDimensions, u_boundingBoxMin);
    vec3 imagePosMax      = min(gridPlusOnePos * cubeDimensions, u_boundingBoxMax);

    if(imagePosMin.x >= imagePosMax.x ||
       imagePosMin.y >= imagePosMax.y ||
       imagePosMin.z >= imagePosMax.z )
    {
        return;
    }

    const int nbVertexPerFace = 4;
    vec3 faceVertices[24] = vec3[24](
            // Negative X
            vec3(imagePosMin.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMax.z),

            // Negative Y
            vec3(imagePosMin.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMax.z),

            // Negative Z
            vec3(imagePosMin.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMin.z),

            // Positive X
            vec3(imagePosMax.x, imagePosMin.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMax.z),

            // Positive Y
            vec3(imagePosMin.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMin.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMax.z),

            // Positive Z
            vec3(imagePosMin.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMin.x, imagePosMax.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMin.y, imagePosMax.z),
            vec3(imagePosMax.x, imagePosMax.y, imagePosMax.z)
    );

    for(int i = 0; i < 6; ++i)
    {
        ivec3 neighbour = gridPos + faceNormals[i];

        // Discard face if its neighbour is not empty.
        // We only want to generate the outer hull.
        if(!isInsideBox(neighbour) || getOpacity(neighbour) == 0)
        {
            for(int j = 0; j < nbVertexPerFace; ++j)
            {
                oPos = faceVertices[i * nbVertexPerFace + j];
                EmitVertex();
            }
            EndPrimitive();
        }
    }
}
