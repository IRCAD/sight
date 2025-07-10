#version 410

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float u_size;
uniform mat4 u_proj;
uniform vec3 u_cameraPos;

layout(location = 0) out float v_gradient;

void main()
{
    // Compute the size and adjust the ratio to be 1:1
    float size = u_size;

    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;

    vec3 dir1 = normalize(p2.xyz - p1.xyz);
    vec3 dir2 = vec3(dir1.y, -dir1.x, dir1.z);

    vec3 pos;

    gl_Position = u_proj * vec4(p1.xyz - dir2 * size, 1.0);
    v_gradient = -1.;

    EmitVertex();

    gl_Position = u_proj * vec4(p2.xyz - dir2 * size, 1.0);
    v_gradient = -1.;

    EmitVertex();

    gl_Position = u_proj * vec4(p1.xyz + dir2 * size, 1.0);
    v_gradient = 1.;

    EmitVertex();

    gl_Position = u_proj * vec4(p2.xyz + dir2 * size, 1.0);
    v_gradient = 1.;

    EmitVertex();

    EndPrimitive();
}
