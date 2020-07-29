#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 f3_in_position;
out vec3 f3_in_normal;
out vec3 f3_in_color;

// Used for flat lighting.
flat out vec3 f3_in_flatNormal;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

uniform int u_iLightingMode; //0=ambient, 1=flat, 2=gouraud, 4=phong.

// Include functions from another shader called 'lighting.inc.glsl'.
#pragma include lighting.inc.glsl
vec3 lighting(const in vec3 pos, const in vec3 n);

void main()
{
    f3_in_normal = normalize(modelViewNormal * vertexNormal);
    f3_in_position = vec3(modelView * vec4(vertexPosition, 1.0));
    f3_in_color = vec3(0);
    f3_in_flatNormal = vec3(0);

    if(u_iLightingMode == 1) //If flat lighting, compute normal in flat output.
    {
        f3_in_flatNormal = normalize(modelViewNormal * vertexNormal);
    }

    if(u_iLightingMode == 2) //If gouraud lighting, compute color in vertex shader.
    {
        f3_in_color = lighting(f3_in_position, normalize(f3_in_normal));
    }

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
