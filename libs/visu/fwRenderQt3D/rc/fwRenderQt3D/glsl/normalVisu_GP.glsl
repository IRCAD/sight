#version 330 core

// Specifies that inputs are passed to the geometry shader as triangle primitives
layout (triangles) in;
// Specifies that the output sent to the fragment shader is a line_strip primitive containing maximum 6 points
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 f3_normal;
} gs_in[];

// Specifies normals size
const float MAGNITUDE = 2.0;

// Generate normal for <index>-th vertex
void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].f3_normal, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}