#version 150

uniform sampler2D u_srcTextures0;
uniform sampler2D u_srcTextures1;

uniform int u_invertLines;

uniform int u_imageShift;

in vec2 uv;

out vec4 fragColor;

/*----------------------------------------------------------------------------*/

void main()
{
    // Line number.
    int lineNum = int(gl_FragCoord.y);

    int isOdd = lineNum & 0x1;

    ivec2 imgDimensions = textureSize(u_srcTextures0, 0);
    float shift = u_imageShift / float(imgDimensions.x);

    vec2 shiftVec = vec2(shift, 0);

    vec3 outColor;
    if(bool(isOdd ^ u_invertLines))
    {
        outColor = texture(u_srcTextures0, uv + shiftVec).rgb;
    }
    else
    {
        outColor = texture(u_srcTextures1, uv).rgb;
    }

    fragColor = vec4(outColor, 1.0);
}
