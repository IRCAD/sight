#version 330 core

in vec3 f3_in_position;
in vec3 f3_in_normal;
in vec3 f3_in_color;

// Used for flat lighting.
flat in vec3 f3_in_flatNormal;

out vec4 f4_out_fragColor;

uniform int u_iLightingMode; //0=ambient, 1=flat, 2=gouraud, 4=phong.

// Include functions from another shader called 'lighting.inc.glsl'.
#pragma include lighting.inc.glsl

void main()
{
    if(u_iLightingMode == 0) //Ambient lighting.
    {
        f4_out_fragColor = vec4(u_f3AmbientCol, 1.0);
    }
    else if(u_iLightingMode == 1) //Flat lighting.
    {
        vec3 result = lighting(f3_in_position, normalize(f3_in_flatNormal));
        f4_out_fragColor = vec4(result, 1.0);
    }
    else if(u_iLightingMode == 2) //Gouraud lighting.
    {
        f4_out_fragColor = vec4(f3_in_color, 1.0);
    }
    else //Phong lighting.
    {
        vec3 result = lighting(f3_in_position, normalize(f3_in_normal));
        f4_out_fragColor = vec4(result, 1.0);
    }
}