#version 150

uniform sampler3D u_texture;
uniform float u_slice;
uniform float u_minValue;
uniform float u_maxValue;
uniform int u_threshold = 0;
uniform int u_orientation = 0;
uniform vec4 u_diffuse;

in vec2 uv;

float windowLevel(vec4 color, float min, float max)
{
    float outputColor = color.r * 65535.f - 32768.f;

//    if(outputColor < min || outputColor > max)
//    {
//        discard;
//    }

    if(u_threshold == 1)
    {
        outputColor = step(min, outputColor) * step(outputColor, max);
    }
    else
    {
        outputColor = clamp(outputColor, min, max);
        outputColor = (outputColor - min) / (max - min);
    }

    return outputColor;
}

vec4 negato()
{
    vec4 color;
    if (u_orientation == 0) // Sagittal
    {
        color = texture(u_texture, vec3(u_slice, uv.y, uv.x));
    }
    else if (u_orientation == 1) // Frontal
    {
        color = texture(u_texture, vec3(uv.x, u_slice, uv.y));
    }
    else if (u_orientation == 2) // Axial
    {
        color = texture(u_texture, vec3(uv, u_slice));
    }

    float lum = windowLevel(color, u_minValue, u_maxValue);

    return vec4(lum, lum, lum, u_diffuse.a);
}
