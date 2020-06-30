#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 f3_in_position;
out vec3 f3_in_normal;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

void main()
{
    f3_in_normal = normalize(modelViewNormal * vertexNormal);
    f3_in_position = vec3(modelView * vec4(vertexPosition, 1.0));

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}