#version 400

uniform sampler3D u_sat;

uniform int u_sliceIndex;

uniform int u_nbShells;
uniform int u_shellRadius;

out vec4 aoFactor;

//-----------------------------------------------------------------------------

vec4 satLookup(in ivec3 min, in ivec3 max)
{
    return texelFetch(u_sat, max, 0)
            - texelFetch(u_sat, ivec3( max.x, max.y, min.z ), 0)
            - texelFetch(u_sat, ivec3( max.x, min.y, max.z ), 0)
            - texelFetch(u_sat, ivec3( min.x, max.y, max.z ), 0)
            + texelFetch(u_sat, ivec3( min.x, min.y, max.z ), 0)
            + texelFetch(u_sat, ivec3( min.x, max.y, min.z ), 0)
            + texelFetch(u_sat, ivec3( max.x, min.y, min.z ), 0)
            - texelFetch(u_sat, min, 0);
}

//-----------------------------------------------------------------------------

void main(void)
{
    ivec3 voxelCoords = ivec3(gl_FragCoord.xy, u_sliceIndex);
    ivec3 satSize = textureSize(u_sat, 0) - ivec3(1);

    ivec3 shellMin = max(voxelCoords - ivec3(u_shellRadius), ivec3(0));
    ivec3 shellMax = min(voxelCoords + ivec3(u_shellRadius), satSize);

    int radius = u_shellRadius;

    aoFactor = satLookup(shellMin, shellMax) / float(radius * radius);

    for(int i = 0; i < u_nbShells; ++i)
    {
        ivec3 newShellMin = max(shellMin - ivec3(u_shellRadius), ivec3(0));
        ivec3 newShellMax = min(shellMax + ivec3(u_shellRadius), satSize);

        radius += u_shellRadius;

        aoFactor += (satLookup(newShellMin, newShellMax) - satLookup(shellMin, shellMax)) / float(radius * radius);

        shellMin = newShellMin;
        shellMax = newShellMax;
    }
}
