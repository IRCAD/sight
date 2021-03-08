#version 330 core

in vec3 v_out_f3Position;
in vec3 v_out_f3Normal;
in vec3 v_out_f3Color;

// Used for flat lighting.
flat in vec3 v_out_f3FlatColor;

out vec4 f_out_f4FragColor;

uniform vec3 u_f3AmbientColCopy;
uniform int u_iLightingMode; //0=ambient, 1=flat, 2=gouraud, 4=phong.

// Include functions from another shader called 'lighting.inc.glsl'.
#pragma include lighting.inc.glsl
vec3 lighting(const in vec3 pos, const in vec3 n);

void main()
{
    if(u_iLightingMode == 0) //Ambient lighting.
    {
        f_out_f4FragColor = vec4(u_f3AmbientColCopy, 1.0);
    }
    else if(u_iLightingMode == 1) //Flat lighting.
    {
        f_out_f4FragColor = vec4(v_out_f3FlatColor, 1.0);
    }
    else if(u_iLightingMode == 2) //Gouraud lighting.
    {
        f_out_f4FragColor = vec4(v_out_f3Color, 1.0);
    }
    else //Phong lighting.
    {
        vec3 f3ResultCol = lighting(v_out_f3Position, normalize(v_out_f3Normal));
        f_out_f4FragColor = vec4(f3ResultCol, 1.0);
    }
}
