// Require GLSL 150
#version 150

uniform mat4 u_worldViewProj;

in vec4 position;

out vec3 normal;

void main()
{
    gl_Position = u_worldViewProj * position;

    normal = normalize(gl_NormalMatrix * gl_Normal);
}
