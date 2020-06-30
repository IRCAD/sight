#version 330 core

// Default attribute names (list can be found in qattribute.cpp)
in vec3 vertexPosition;
in vec3 vertexNormal;

// Out block to send, for each vertex, its normal to the geometry shader
out VS_OUT {
    vec3 f3_normal;
} vs_out;

uniform mat4 projectionMatrix;
uniform mat4 modelView;
uniform mat4 mvp;

void main()
{
    // compute vertex position
    gl_Position = mvp * vec4(vertexPosition, 1.0);

    // compute vertex' normal position
    mat3 normalMatrix = mat3(transpose(inverse(modelView)));
    vs_out.f3_normal = normalize(vec3(projectionMatrix * vec4(normalMatrix * vertexNormal, 0.0)));
}