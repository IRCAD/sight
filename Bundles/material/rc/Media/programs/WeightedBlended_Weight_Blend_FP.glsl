#version 330

#ifdef HYBRID
uniform sampler2D u_frontDepthBuffer;
#endif

uniform sampler2D u_occlusionDepthBuffer;
uniform float u_vpWidth;
uniform float u_vpHeight;
uniform vec4 u_diffuse;

vec4 getMaterialColor();

uniform float u_near;
uniform float u_far;
out vec4 FragColor;

float linearizeDepth(in float depth) {
    return (2.0*u_near) / (u_far+u_near - depth *(u_far-u_near));
}

void main()
{
    vec2 texCoord = gl_FragCoord.xy / vec2( u_vpWidth, u_vpHeight );
#ifdef HYBRID
    vec4 frontDepthBuffer = texture(u_frontDepthBuffer, texCoord);
#endif
    vec4 occlusionDepthBuffer = texture(u_occlusionDepthBuffer, texCoord);
    float currentDepth = gl_FragCoord.z;

#ifdef HYBRID
    if(frontDepthBuffer.r == 0. || currentDepth <= frontDepthBuffer.r || currentDepth > occlusionDepthBuffer.r)
#else
    if(currentDepth > occlusionDepthBuffer.r)
#endif
    {
        discard;
    }

    vec4 colorOut = getMaterialColor();

    float linearDepth = linearizeDepth(gl_FragCoord.z);
    linearDepth = linearDepth*2.5;

    float weight = clamp(0.03 / (1e-5 + pow(linearDepth, 4.0)), 1e-2, 3e3);

    FragColor = vec4(colorOut.rgb * colorOut.a, colorOut.a) * weight;
}
