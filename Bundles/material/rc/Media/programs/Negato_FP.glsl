#version 150

uniform sampler3D img;
uniform float slice;
uniform float minValue;
uniform float maxValue;
uniform int threshold = 0;
uniform int orientation = 0;
uniform float opacity = 1.f;

in vec2 uv;

out vec4 fragColor;

float windowLevel(vec4 color, float min, float max)
{
    float outputColor = color.r * 65535.f - 32768.f;

    if(threshold == 1)
    {
        outputColor = step(min, outputColor) * step(outputColor, max);
    } else
    {
        outputColor = clamp(outputColor, min, max);
        outputColor = (outputColor - min) / (max - min);
    }

    return outputColor;
}

void main()
{
    vec4 color;
    if (orientation == 0) // Sagittal
    {
        color = texture(img, vec3(slice, uv.y, uv.x));
    }
    else if (orientation == 1) // Frontal
    {
        color = texture(img, vec3(uv.x, slice, uv.y));
    }
    else if (orientation == 2) // Axial
    {
        color = texture(img, vec3(uv, slice));
    }

    float lum = windowLevel(color, minValue, maxValue);

    fragColor = vec4(lum, lum, lum, opacity);
}
