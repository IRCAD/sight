#version 430

in int position;

#ifndef FP_OUTPUT
uniform ivec3 u_gridResolution;

out ivec3 vs_gridPos;
#endif

void main()
{

#ifdef FP_OUTPUT
    vec2 quadPositions[] = {
        vec2(-1, -1),
        vec2(-1,  1),
        vec2( 1, -1),
        vec2( 1,  1)
    };

    gl_Position = vec4(quadPositions[gl_VertexID], 0, 1);
#else
    const int i = gl_VertexID % u_gridResolution.x;
    const int jk = gl_VertexID / u_gridResolution.x;
    const int j = jk % u_gridResolution.y;
    const int k = jk / u_gridResolution.y;

    vs_gridPos = ivec3(i, j, k);
    gl_Position = vec4(vec3(vs_gridPos), 1);
#endif
}
