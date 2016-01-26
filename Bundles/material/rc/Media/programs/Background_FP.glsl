#version 150

in vec2 uv;

uniform vec4 topColour;
uniform vec4 bottomColour;

uniform float topScale;
uniform float bottomScale;

out vec4 fragColor;

void main()
{
    float high = uv.y;
    vec4 colour = vec4(0.f);

    if (high < topScale)
    {
        colour = topColour;
    } else if (high < bottomScale)
    {
        // Compute linear function for interpolation coefficient
        float a,b;
        a = - (1 / (topScale - bottomScale));
        b = 1 - a * bottomScale;

        float interpolation = a * high + b;

        colour = topColour + (bottomColour - topColour) * interpolation;
    } else
    {
        colour = bottomColour;
    }

    fragColor = colour;
}
