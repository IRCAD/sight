#version 150

uniform sampler2D u_srcTextures0;
uniform sampler2D u_srcTextures1;

uniform int u_invertLines;

in vec2 uv;

out vec4 fragColor;

/*----------------------------------------------------------------------------*/

void main()
{
    // Line number.
    int lineNum = int(gl_FragCoord.y);

    int isOdd = lineNum & 0x1;

    vec3 outColor;
    if(bool(isOdd ^ u_invertLines))
    {
        outColor = texture(u_srcTextures0, uv).rgb;
    }
    else
    {
        outColor = texture(u_srcTextures1, uv).rgb;
    }

    fragColor = vec4(outColor, 1.0);
}
