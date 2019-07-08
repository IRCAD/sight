#version 150

in vec3 vertex;

#ifdef PER_POINT_COLOR
in vec3 colour;
#endif // PER_POINT_COLOR


uniform mat4 u_worldView;

#ifdef PER_POINT_COLOR
out vec4 v_f4PointCol;
#endif // PER_POINT_COLOR

void main()
{
#ifdef PER_POINT_COLOR
    v_f4PointCol = vec4(colour, 1.);
#endif // PER_POINT_COLOR
    gl_Position = u_worldView * vec4(vertex, 1.);
}
