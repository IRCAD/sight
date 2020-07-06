#version 330 core

// Specifies that inputs are passed to the geometry shader as triangle primitives.
layout (triangles) in;
// Specifies that the output sent to the fragment shader is a line_strip primitive containing maximum 6 points.
layout (line_strip, max_vertices = 2) out;

// Specifies normals size.
const float MAGNITUDE = 2.0;

void main()
{
    vec4 P0 = gl_in[0].gl_Position;
    vec4 P1 = gl_in[1].gl_Position;
    vec4 P2 = gl_in[2].gl_Position;

    vec4 V0 = P0 - P1; // P1->P0
    vec4 V1 = P2 - P1; // P1->P2

    // Face normal, which is the cross product of V0 and V1.
    vec3 N = cross(V1.xyz, V0.xyz);
    N = normalize(N);

    // Center of the triangle.
    vec4 P = (P0 + P1 + P2) / 3.0;

    // Cell normal origin.
    gl_Position = P;

    EmitVertex();

    // Cell normal summit.
    gl_Position = (P + vec4(N, 0.) * MAGNITUDE);

    EmitVertex();
    EndPrimitive();
}