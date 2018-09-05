#version 330

#ifdef AUTOSTEREO
uniform mat4 u_worldView;
uniform mat4 u_proj;
#else
uniform mat4 u_worldViewProj;
#endif // AUTOSTEREO

in vec3 position;

void main()
{
#ifdef AUTOSTEREO
    gl_Position = u_proj * u_worldView * vec4(position, 1.);
#else
    gl_Position = u_worldViewProj * vec4(position, 1.);
#endif // AUTOSTEREO
}
