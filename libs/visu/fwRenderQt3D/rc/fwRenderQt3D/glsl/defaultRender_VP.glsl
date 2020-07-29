#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 v_out_f3Position;
out vec3 v_out_f3Normal;
out vec3 v_out_f3Color;

// Used for flat lighting.
flat out vec3 v_out_f3FlatColor;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

uniform int u_iLightingMode; //0=ambient, 1=flat, 2=gouraud, 4=phong.

// Include functions from another shader called 'lighting.inc.glsl'.
#pragma include lighting.inc.glsl
vec3 lighting(const in vec3 pos, const in vec3 n);

void main()
{

    v_out_f3Normal = normalize(modelViewNormal * vertexNormal);
    v_out_f3Position = vec3(modelView * vec4(vertexPosition, 1.0));
    v_out_f3Color = vec3(0);
    v_out_f3FlatColor = vec3(0);

    if(u_iLightingMode == 1) //If flat lighting, compute normal in flat output.
    {
        v_out_f3FlatColor = lighting(v_out_f3Position, normalize(modelViewNormal * vertexNormal));
    }

    if(u_iLightingMode == 2) //If gouraud lighting, compute color in vertex shader.
    {
        v_out_f3Color = lighting(v_out_f3Position, normalize(v_out_f3Normal));
    }

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
