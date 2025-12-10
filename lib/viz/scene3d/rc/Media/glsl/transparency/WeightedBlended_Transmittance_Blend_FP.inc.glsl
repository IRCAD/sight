#ifdef HYBRID
uniform sampler2D u_frontDepthBuffer;
#endif

uniform sampler2D u_occlusionDepthBuffer;

uniform vec4 u_viewport;

out vec4 FragColor;

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

    FragColor.r = getFragmentAlpha();
}
