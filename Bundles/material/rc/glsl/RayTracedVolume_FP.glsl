#version 410

uniform sampler3D u_image;
uniform sampler2D u_tfTexture;

uniform vec3 u_lightDir;
//uniform vec3 u_diffuse;

uniform float u_sampleDistance;

uniform vec3 u_boundingBoxMin;
uniform vec3 u_boundingBoxMax;

#ifdef PREINTEGRATION
uniform int u_min;
uniform int u_max;
#endif // PREINTEGRATION

in vec3 vs_uvw;

in vec4 vs_rayEntryPoint;
in vec3 vs_cameraDir;

out vec4 fragColor;

//-----------------------------------------------------------------------------

vec4 transferFunction(float intensity)
{
    float scaledValue = intensity * 65535.f;

    // Computes 2D indices from the hounsfield value
    int j = int( scaledValue / 256 );
    int i = int( mod( int(scaledValue), 256 ) );

    // Converts the indices into texture uv coordinates
    vec2 uvTF = vec2(i / 255.f, j / 255.f);

    return texture(u_tfTexture, uvTF);
}

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

vec3 getExitPoint(in vec3 rayPos, in vec3 rayDir)
{
    // compute Ray-Box intersection.
    vec3 tmin = (u_boundingBoxMin - rayPos) / rayDir;
    vec3 tmax = (u_boundingBoxMax - rayPos) / rayDir;

    vec3 real_min = min(tmin, tmax);
    vec3 real_max = max(tmin, tmax);

    float entry = min( min(real_max.x, real_max.y), real_max.z);
    float exit  = max( max(real_min.x, real_min.y), real_min.z);

    // If rayPos is inside the cube, exit = entry.
    exit = max(entry, exit);

    if(exit <= 0 || isnan(exit))
    {
        discard;
    }

    // Return the point where the ray exists the box.
    return vec3(vs_rayEntryPoint) + exit * rayDir;
}

//-----------------------------------------------------------------------------

void composite(inout vec4 result, in vec4 colour)
{
    result.rgb = result.rgb + (1 - result.a) * colour.a * colour.rgb;
    result.a   = result.a   + (1 - result.a) * colour.a;
}

//-----------------------------------------------------------------------------

void main(void)
{
    float t, tend;

    vec3 rayPos = vec3(vs_rayEntryPoint);
    vec3 rayDir = normalize(vs_cameraDir) * u_sampleDistance;

    vec3 rayEnd = getExitPoint(rayPos, rayDir);

    tend = length(rayEnd - rayPos);

    vec4 result = vec4(0);

    // Opacity correction factor =
    // Inverse of the sampling rate accounted by the TF.
    const float opacityCorrectionFactor = 200.;

    for(t = 0 ; t < tend; t += u_sampleDistance)
    {
#ifdef PREINTEGRATION
        float sf = texture(u_image, rayPos).r;
        float sb = texture(u_image, rayPos + rayDir).r;

        sf = ((sf * 65535.f) + float(u_min)) / float(u_max - u_min);
        sb = ((sb * 65535.f) + float(u_min)) / float(u_max - u_min);

        vec4 tfColour = texture(u_tfTexture, vec2(sf, sb));

#else
        float intensity = texture(u_image, rayPos).r;

        vec4  tfColour  = transferFunction(intensity);
#endif // PREINTEGRATION

        if(tfColour.a > 0)
        {
//            vec3 N = gradientNormal(rayPos);

//            tfColour.rgb = tfColour.rgb * abs(dot(N, u_lightDir));

            // Adjust opacity to sample distance.
            // This could be done when generating the TF texture to improve performance.
            tfColour.a   = 1 - pow(1 - tfColour.a, u_sampleDistance * opacityCorrectionFactor);

            composite(result, tfColour);

            if(result.a > 0.99)
            {
                break;
            }
        }

        rayPos += rayDir;
    }

    fragColor = result;
}
