#version 330

#ifndef NEAR_PLANE
uniform mat4 u_worldViewProj;
#endif // NEAR_PLANE

in vec3 position;

void main()
{
#if NEAR_PLANE == 1
    gl_Position = vec4(position, 1.);
#else
    gl_Position = u_worldViewProj * vec4(position, 1.);
#endif //  NEAR_PLANE
}
