// Uniforms
#ifdef HYBRID
layout(binding=2) uniform sampler2D u_frontDepthBuffer;
#endif

layout(binding=3) uniform sampler2D u_occlusionDepthBuffer;

uniform vec4 u_viewport;

#ifndef HAS_CLIP_UNIFORM
uniform float u_near;
uniform float u_far;
#endif

out vec4 FragColor;

float linearizeDepth(in float depth) {
    return (2.0*u_near) / (u_far+u_near - depth *(u_far-u_near));
}

void process()
{
    vec2 texCoord = gl_FragCoord.xy * u_viewport.zw;
#ifdef HYBRID
    float frontDepthBuffer = texture(u_frontDepthBuffer, texCoord).r;
#endif
    vec4 occlusionDepthBuffer = texture(u_occlusionDepthBuffer, texCoord);
    float currentDepth = gl_FragCoord.z;

#ifdef HYBRID
    if(frontDepthBuffer == 0. || currentDepth <= frontDepthBuffer || currentDepth > occlusionDepthBuffer.r)
#else
    if(currentDepth > occlusionDepthBuffer.r)
#endif
    {
        discard;
    }

    const vec4 colorOut = getFragmentColor();

    float linearDepth = linearizeDepth(gl_FragCoord.z);
    linearDepth = linearDepth*2.5;

    float weight = clamp(0.03 / (1e-5 + pow(linearDepth, 4.0)), 1e-2, 3e3);

    FragColor = vec4(colorOut.rgb * colorOut.a, colorOut.a) * weight;
}
