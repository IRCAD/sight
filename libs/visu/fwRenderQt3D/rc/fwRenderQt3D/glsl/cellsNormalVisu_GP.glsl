#version 330 core

// Specifies that inputs are passed to the geometry shader as triangle primitives.
layout (triangles) in;
// Specifies that the output sent to the fragment shader is a line_strip primitive containing maximum 6 points.
layout (line_strip, max_vertices = 2) out;

// Specifies normals size.
const float MAGNITUDE = 2.0;

void main()
{
    vec4 f4vertex0Pos = gl_in[0].gl_Position;
    vec4 f4vertex1Pos = gl_in[1].gl_Position;
    vec4 f4vertex2Pos = gl_in[2].gl_Position;

    vec4 f4v1Tov0Dir = f4vertex0Pos - f4vertex1Pos; // f4vertex1Pos->f4vertex0Pos
    vec4 f4v1Tov2Dir = f4vertex2Pos - f4vertex1Pos; // f4vertex1Pos->f4vertex2Pos

    // Face normal, which is the cross product of f4v1Tov0Dir and f4v1Tov2Dir.
    vec3 f3TriangleNormalN = cross(f4v1Tov2Dir.xyz, f4v1Tov0Dir.xyz);
    f3TriangleNormalN = normalize(f3TriangleNormalN);

    // Center of the triangle.
    vec4 f4TriangleCenter = (f4vertex0Pos + f4vertex1Pos + f4vertex2Pos) / 3.0;

    // Cell normal origin.
    gl_Position = f4TriangleCenter;

    EmitVertex();

    // Cell normal summit.
    gl_Position = (f4TriangleCenter + vec4(f3TriangleNormalN, 0.) * MAGNITUDE);

    EmitVertex();
    EndPrimitive();
}