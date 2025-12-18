#version 150

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec4 u_diffuse;
uniform float u_billboardSize;
uniform mat4 u_proj;
uniform vec4 u_viewport;

#ifdef VERTEX_NORMAL
in vec3 v_f3Normal_Ws[];
in vec3 v_f3Position_Ws[];
uniform mat4 u_view;
uniform vec3 u_cameraPos;
#endif

#ifdef VERTEX_COLOR
in vec4 v_f4PointCol[];
#endif

#ifndef DEPTH
out vec4 oColor;
out vec2 oTexCoord;

#ifdef VERTEX_NORMAL
out vec3 oPosition_Ws;
out vec3 oNormal_Ws;
#endif

#endif

void main()
{
    //------------------------------------------------------------------------------
    // With normals provided, orient the billboard according to the normal direction
    //------------------------------------------------------------------------------
#ifdef VERTEX_NORMAL
    // Desired billboard side lengths (world-space), keep square using viewport ratio if needed
    vec2 size = vec2(1.0, u_viewport.x / u_viewport.y) * u_billboardSize;

    vec3 N = normalize(v_f3Normal_Ws[0]);
    vec3 P_ws = v_f3Position_Ws[0];

    // Choose a stable orientation around the normal using camera direction
    vec3 V = normalize(u_cameraPos - P_ws);
    vec3 R = normalize(cross(V, N));
    if(length(R) < 1e-4) {
        // Handle edge case when viewDir is parallel to normal
        vec3 arbitrary = abs(N.y) < 0.99 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
        R = normalize(cross(arbitrary, N));
    }
    vec3 U = normalize(cross(N, R));

#ifndef DEPTH
    oNormal_Ws = N;
#ifdef VERTEX_COLOR
    vec4 color = v_f4PointCol[0] * u_diffuse;
#else
    vec4 color = u_diffuse;
#endif
#endif

    // Quad corners in world space, oriented perpendicular to the normal
    vec3 A_ws = P_ws + (-R * size.x) + (-U * size.y);
    vec3 B_ws = P_ws + (-R * size.x) + ( U * size.y);
    vec3 D_ws = P_ws + ( R * size.x) + (-U * size.y);
    vec3 C_ws = P_ws + ( R * size.x) + ( U * size.y);

    // Emit A
    vec4 A_vs = u_view * vec4(A_ws, 1.0);
    gl_Position = u_proj * A_vs;
#ifndef DEPTH
    oColor = color;
    oTexCoord = vec2(0.0, 0.0);
    oPosition_Ws = A_ws;
#endif
    EmitVertex();

    // Emit B
    vec4 B_vs = u_view * vec4(B_ws, 1.0);
    gl_Position = u_proj * B_vs;
#ifndef DEPTH
    oColor = color;
    oTexCoord = vec2(0.0, 1.0);
    oPosition_Ws = B_ws;
#endif
    EmitVertex();

    // Emit D
    vec4 D_vs = u_view * vec4(D_ws, 1.0);
    gl_Position = u_proj * D_vs;
#ifndef DEPTH
    oColor = color;
    oTexCoord = vec2(1.0, 0.0);
    oPosition_Ws = D_ws;
#endif
    EmitVertex();

    // Emit C
    vec4 C_vs = u_view * vec4(C_ws, 1.0);
    gl_Position = u_proj * C_vs;
#ifndef DEPTH
    oColor = color;
    oTexCoord = vec2(1.0, 1.0);
    oPosition_Ws = C_ws;
#endif
    EmitVertex();

    EndPrimitive();

#else // VERTEX_NORMAL

    //------------------------------------------------------------------------------
    // Without normals, always face the camera
    //------------------------------------------------------------------------------

    // Compute the size and adjust the ratio to be 1:1
    vec2 size = vec2(1., u_viewport.x/u_viewport.y) * u_billboardSize;
#ifdef FIXED_SIZE
    size *= 0.01;
#endif // FIXED_SIZE

    vec4 P = gl_in[0].gl_Position;
#ifndef DEPTH
#ifdef VERTEX_COLOR
    vec4 color = v_f4PointCol[0] * u_diffuse;
#else
    vec4 color = u_diffuse;
#endif // VERTEX_COLOR
#endif // DEPTH

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
    oColor = color;
    oTexCoord = vec2(0,0);
#endif

    EmitVertex();

    vec2 vb = P.xy + vec2(-1., 1.) * size;
    gl_Position = vec4(vb, P.zw);

#ifndef DEPTH
    oColor =  color;
    oTexCoord = vec2(0,1);
#endif

    EmitVertex();

    vec2 vd = P.xy + vec2(1., -1.) * size;
    gl_Position = vec4(vd, P.zw);

#ifndef DEPTH
    oColor = color;
    oTexCoord = vec2(1,0);
#endif

    EmitVertex();

    vec2 vc = P.xy + vec2(1., 1.) * size;
    gl_Position = vec4(vc, P.zw);

#ifndef DEPTH
    oColor = color;
    oTexCoord = vec2(1,1);
#endif

    EmitVertex();

    EndPrimitive();
#endif // VERTEX_NORMAL
}
