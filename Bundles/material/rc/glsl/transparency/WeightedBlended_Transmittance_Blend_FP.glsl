#version 330

#ifdef HYBRID
uniform sampler2D u_frontDepthBuffer;
#endif

uniform sampler2D u_occlusionDepthBuffer;

uniform float u_vpWidth;
uniform float u_vpHeight;

uniform vec4 u_diffuse;

out vec4 FragColor;

// Extern functions
float getFragmentAlpha();

#ifdef HYBRID
float unpackFloatFromVec4(vec4 value);
#endif

void main()
{
    vec2 texCoord = gl_FragCoord.xy / vec2( u_vpWidth, u_vpHeight );

#ifdef HYBRID
    float frontDepthBuffer = unpackFloatFromVec4(texture(u_frontDepthBuffer, texCoord));
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
