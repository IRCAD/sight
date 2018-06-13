#version 330

#ifndef NEAR_PLANE
#ifdef AUTOSTEREO
uniform mat4 u_worldView;
uniform mat4 u_proj;
#else
uniform mat4 u_worldViewProj;
#endif // AUTOSTEREO
#endif // NEAR_PLANE

in vec3 position;

void main()
{
#if NEAR_PLANE == 1
    gl_Position = vec4(position, 1.);
#else
#ifdef AUTOSTEREO
    gl_Position = u_proj * u_worldView * vec4(position, 1.);
#else
    gl_Position = u_worldViewProj * vec4(position, 1.);
#endif // AUTOSTEREO
#endif //  NEAR_PLANE
}
