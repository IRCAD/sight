#version 330 core

uniform mat4 viewMatrix;

uniform vec3 u_f3LightPosition;
uniform vec3 u_f3LightIntensity;

uniform vec3 u_f3AmbientCol;
uniform vec3 u_f3DiffuseCol;
uniform vec3 u_f3SpecularCol;
uniform float u_fShininess;

in vec3 f3_in_position;
in vec3 f3_in_normal;

out vec4 f4_out_fragColor;

// Include functions from another shader called 'ligting.glsl'
#pragma include lighting.glsl

void main()
{
    vec3 result = lighting(f3_in_position, normalize(f3_in_normal));
    f4_out_fragColor = vec4(result, 1.0);
}