#version 410

in vec3 vertex;
in vec3 normal;

layout(location = 1) out vec3 v_f3Normal_Ms;

void main()
{
    gl_Position = vec4(vertex, 1.);
    v_f3Normal_Ms = normal;
}
