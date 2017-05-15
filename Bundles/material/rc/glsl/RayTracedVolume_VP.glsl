#version 330

uniform mat4 u_worldViewProj;

#ifdef AUTOSTEREO
uniform mat4 u_worldView;
uniform mat4 u_proj;
#endif // AUTOSTEREO

in vec4 position;

void main(void)
{
#ifdef AUTOSTEREO
    gl_Position = u_proj * u_worldView * position;
#else
    gl_Position = u_worldViewProj * position;
#endif
}
