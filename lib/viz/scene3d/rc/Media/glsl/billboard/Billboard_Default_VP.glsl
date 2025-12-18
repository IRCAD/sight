#version 150

in vec3 vertex;

#ifdef VERTEX_NORMAL
in vec3 normal;
#endif // VERTEX_normal

#ifdef VERTEX_COLOR
in vec3 colour;
#endif // VERTEX_COLOR

uniform mat4 u_worldView;

#ifdef VERTEX_NORMAL
uniform mat4 u_normalMatrix;
out vec3 v_f3Normal_Ws;
out vec3 v_f3Position_Ws;
#endif // VERTEX_NORMAL

#ifdef VERTEX_COLOR
out vec4 v_f4PointCol;
#endif // VERTEX_COLOR

void main()
{
#ifdef VERTEX_NORMAL
    v_f3Normal_Ws = normalize(u_normalMatrix * vec4(normal, 0.f)).xyz;
    v_f3Position_Ws = vertex;
#endif // VERTEX_NORMAL

#ifdef VERTEX_COLOR
    v_f4PointCol = vec4(colour, 1.);
#endif // VERTEX_COLOR
    gl_Position = u_worldView * vec4(vertex, 1.);
}
