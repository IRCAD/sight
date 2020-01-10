#version 330

uniform vec4 u_viewportSize_Ss;
uniform vec4 u_borderColor;
uniform float u_borderSize_Ss;

out vec4 f_fragColor;

void main()
{
    vec2 minBorderBounds_Ss = vec2(u_borderSize_Ss);
    vec2 maxBorderBounds_Ss = u_viewportSize_Ss.xy - minBorderBounds_Ss;

    bool isMinBound = any(lessThan(gl_FragCoord.xy, minBorderBounds_Ss));
    bool isMaxBound = any(greaterThan(gl_FragCoord.xy, maxBorderBounds_Ss));

    if(isMinBound || isMaxBound)
    {
        f_fragColor = u_borderColor;
    }
    else
    {
        discard;
    }
}