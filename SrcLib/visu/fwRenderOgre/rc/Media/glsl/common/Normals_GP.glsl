#version 410

layout(triangles) in;

#    ifdef POINT
layout(line_strip, max_vertices = 6) out;

#    else // CELL
layout(line_strip, max_vertices = 2) out;
#    endif

uniform mat4 u_worldViewProj;
uniform float u_sceneSize;

#    ifdef POINT
layout(location = 1) in vec3 v_f3Normal_Ms[];
#    endif

#    ifndef DEPTH
uniform vec4 u_normalColor;

// Output semantics
layout(location = 2) out vec4 v_f4Color;

#    endif

void main()
{
    float scale = u_sceneSize * 0.02;
#    ifdef POINT
        for(int i = 0; i < 3; i++)
        {
            vec4 P = gl_in[i].gl_Position;
            vec3 N = v_f3Normal_Ms[i];

            // Vertex normal origin
            gl_Position = u_worldViewProj * P;

#        ifndef DEPTH
            v_f4Color = u_normalColor;
#        endif

            EmitVertex();

            // Vertex normal summit
            gl_Position = u_worldViewProj * (P + vec4(N, 0.) * scale);

#        ifndef DEPTH
            v_f4Color = u_normalColor;
#        endif

            EmitVertex();
            EndPrimitive();
        }

#    else // CELL
        vec4 P0 = gl_in[0].gl_Position;
        vec4 P1 = gl_in[1].gl_Position;
        vec4 P2 = gl_in[2].gl_Position;

        vec4 V0 = P0 - P1; // P1->P0
        vec4 V1 = P2 - P1; // P1->P2

        // Face normal, which is the cross product of V0 and V1
        vec3 N = cross(V1.xyz, V0.xyz);
        N = normalize(N);

        // Center of the triangle
        vec4 P = (P0 + P1 + P2) / 3.0;

        // Cell normal origin
        gl_Position = u_worldViewProj * P;

#        ifndef DEPTH
            v_f4Color = u_normalColor;
#        endif

        EmitVertex();

        // Cell normal summit
        gl_Position = u_worldViewProj * (P + vec4(N, 0.) * scale);

#        ifndef DEPTH
            v_f4Color = u_normalColor;
#        endif

        EmitVertex();
        EndPrimitive();
#    endif
}
