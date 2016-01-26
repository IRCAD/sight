#version 150

in vec3 vertex;
in vec3 normal;

out VertexDataOut
{
    vec3 normalVP;
} vertexOut;

void main()
{
    gl_Position = vec4(vertex, 1.);
    vertexOut.normalVP = normal;
}
