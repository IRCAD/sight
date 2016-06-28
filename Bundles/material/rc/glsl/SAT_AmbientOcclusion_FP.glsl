#version 150

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

    ivec3 shellMin = voxelCoords - ivec3(u_shellRadius);
    ivec3 shellMax = voxelCoords + ivec3(u_shellRadius);

    int radius = u_shellRadius;

    aoFactor = satLookup(shellMin, shellMax) / float(radius * radius);

    for(int i = 1; i < u_nbShells; ++ i)
    {
        ivec3 newShellMin = shellMin - ivec3(u_shellRadius);
        ivec3 newShellMax = shellMax + ivec3(u_shellRadius);

        radius += u_shellRadius;

        aoFactor += (satLookup(newShellMin, newShellMax) - satLookup(shellMin, shellMax)) / float(radius * radius);

        shellMin = newShellMin;
        shellMax = newShellMax;
    }
}
