#version 330

uniform mat4 u_worldViewProj;

in vec3 position;

void main()
{
#if NEAR_PLANE == 1
    gl_Position = vec4(position, 1.);
#else
    gl_Position = u_worldViewProj * vec4(position, 1.);
#endif //  NEAR_PLANE
}
