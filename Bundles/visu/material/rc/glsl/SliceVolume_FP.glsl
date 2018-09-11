#version 150

uniform sampler3D u_image;
uniform sampler2D u_tfTexture;

uniform vec3 u_lightDir;

uniform float u_sampleDistance;

#ifdef PREINTEGRATION
uniform vec3  u_viewDirection;

uniform int u_min;
uniform int u_max;
#endif // PREINTEGRATION

in vec3 vs_uvw;

#ifdef PREINTEGRATION
in vec3 vs_cameraDir;
#endif // PREINTEGRATION

out vec4 fragColor;

//-----------------------------------------------------------------------------
vec4 sampleTransferFunction(float intensity);

//-----------------------------------------------------------------------------

vec3 gradientNormal(vec3 uvw)
{
    ivec3 imgDimensions = textureSize(u_image, 0);
    vec3 h = 1. / vec3(imgDimensions);
    vec3 hx = vec3(h.x, 0, 0);
    vec3 hy = vec3(0, h.y, 0);
    vec3 hz = vec3(0, 0, h.z);

    return normalize( vec3(
                0.5 * (texture(u_image, uvw + hx).r - texture(u_image, uvw - hx).r),
                0.5 * (texture(u_image, uvw + hy).r - texture(u_image, uvw - hy).r),
                0.5 * (texture(u_image, uvw + hz).r - texture(u_image, uvw - hz).r)
    ));
}

//-----------------------------------------------------------------------------

void main(void)
{
#ifdef PREINTEGRATION
    vec3 camDir = normalize(vs_cameraDir);

    float perspectiveAngleCos = dot(u_viewDirection, camDir);
    float sliceDist = u_sampleDistance / perspectiveAngleCos;

    vec3 sbPos = vs_uvw + sliceDist * camDir;

    float sf = texture(u_image, vs_uvw).r;
    float sb = texture(u_image, sbPos ).r;

    sf = ((sf * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);
    sb = ((sb * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);

    vec4 colourTf = texture(u_tfTexture, vec2(sf, sb));
#else
    float intensity = texture(u_image, vs_uvw).r;

    vec4 colourTf = sampleTransferFunction(intensity);

    // Adjust opacity to sample distance.
    // This could be done when generating the TF texture to improve performance.
    colourTf.a = 1 - pow(1 - colourTf.a, u_sampleDistance * 200);

#endif // PREINTEGRATION

    vec3 N = gradientNormal(vs_uvw);

    vec3 outColour = colourTf.rgb * abs(dot(N, u_lightDir));

    fragColor = vec4(outColour.rgb, colourTf.a);
}
