#version 150

layout(points) in;

layout(triangle_strip, max_vertices = 4) out;

uniform vec4 u_diffuse;

uniform float u_billboardSize;
uniform mat4 u_proj;
uniform vec3 u_cameraPos;
uniform vec4 u_viewport;

#ifndef DEPTH
out vec4 oColor;
out vec2 oTexCoord;
#endif

void main()
{
    // Compute the size and adjust the ratio to be 1:1
    vec2 size = vec2(1., u_viewport.x/u_viewport.y) * u_billboardSize;
#ifdef FIXED_SIZE
    size *= 0.01;
#endif

    vec4 P = gl_in[0].gl_Position;

    // Offset slightly the billboard to avoid z-fight when clicking points on meshes
    P.w += .01f;
    P = u_proj * P;

#ifdef FIXED_SIZE
    // Switch to screen-space coordinates, easier since we want a fixed size in pixels
    P /= P.w;
#endif

    vec2 va = P.xy + vec2(-1., -1.) * size;
    gl_Position = vec4(va, P.zw);

#ifndef DEPTH
    oColor = u_diffuse;
    oTexCoord = vec2(0,0);
#endif

    EmitVertex();

    vec2 vb = P.xy + vec2(-1., 1.) * size;
    gl_Position = vec4(vb, P.zw);

#ifndef DEPTH
    oColor = u_diffuse;
    oTexCoord = vec2(0,1);
#endif

    EmitVertex();

    vec2 vd = P.xy + vec2(1., -1.) * size;
    gl_Position = vec4(vd, P.zw);

#ifndef DEPTH
    oColor = u_diffuse;
    oTexCoord = vec2(1,0);
#endif

    EmitVertex();

    vec2 vc = P.xy + vec2(1., 1.) * size;
    gl_Position = vec4(vc, P.zw);

#ifndef DEPTH
    oColor = u_diffuse;
    oTexCoord = vec2(1,1);
#endif

    EmitVertex();

    EndPrimitive();
}
