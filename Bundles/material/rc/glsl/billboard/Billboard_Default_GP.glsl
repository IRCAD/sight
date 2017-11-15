#version 150

layout(points) in;

layout(triangle_strip, max_vertices = 4) out;

uniform vec4 u_diffuse;

uniform float u_sceneSize;
uniform mat4 u_proj;

#ifndef DEPTH
out vec4 oColor;
out vec2 oTexCoord;
#endif

void main()
{
    float size = u_sceneSize * .01;
    vec4 P = gl_in[0].gl_Position;
    P.z += 0.01;

    vec2 va = P.xy + vec2(-0.5, -0.5) * size;
    gl_Position = u_proj * vec4(va, P.zw);

#ifndef DEPTH
    oColor = u_diffuse;
    oTexCoord = vec2(0,0);
#endif

    EmitVertex();

    vec2 vb = P.xy + vec2(-0.5, 0.5) * size;
    gl_Position = u_proj * vec4(vb, P.zw);

#ifndef DEPTH
    oColor = u_diffuse;
    oTexCoord = vec2(0,1);
#endif

    EmitVertex();

    vec2 vd = P.xy + vec2(0.5, -0.5) * size;
    gl_Position = u_proj * vec4(vd, P.zw);

#ifndef DEPTH
    oColor = u_diffuse;
    oTexCoord = vec2(1,0);
#endif

    EmitVertex();

    vec2 vc = P.xy + vec2(0.5, 0.5) * size;
    gl_Position = u_proj * vec4(vc, P.zw);

#ifndef DEPTH
    oColor = u_diffuse;
    oTexCoord = vec2(1,1);
#endif

    EmitVertex();

    EndPrimitive();
}
