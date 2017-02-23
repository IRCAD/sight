#version 330

uniform sampler3D u_image;
uniform sampler3D u_mask;

uniform sampler2D u_tfTexture;

uniform sampler2D u_entryPoints;


uniform mat4 u_invWorldViewProj;

in vec2 uv;

uniform float u_sampleDistance;

uniform float u_viewportWidth;
uniform float u_viewportHeight;

uniform float u_clippingNear;
uniform float u_clippingFar;

uniform float u_renderTargetFlipping;

// MImP with countersink geometry
#if IDVR == 1
layout (location = 0) out vec4 mrt_IC_RayTracing;
layout (location = 1) out vec4 mrt_IC_JFA;
#else
#if IDVR == 2 || IDVR == 3
uniform float u_losImportant;
uniform float u_losNotImportant;

out vec4 mrt_IC_RayTracing;
#endif
#endif

//-----------------------------------------------------------------------------
vec4 sampleTransferFunction(float intensity);

//-----------------------------------------------------------------------------

vec3 getFragmentImageSpacePosition(in float depth, in mat4 invWorldViewProj)
{
    // TODO: Simplify this -> uniforms
    vec3 screenPos = vec3(gl_FragCoord.xy / vec2(u_viewportWidth, u_viewportHeight), depth);
    if(u_renderTargetFlipping < 0)
    {
        screenPos.y = 1.0 - screenPos.y;
    }
    screenPos -= 0.5;
    screenPos *= 2.0;

    vec4 clipPos;
    clipPos.w   = (2 * u_clippingNear * u_clippingFar) / (u_clippingNear + u_clippingFar + screenPos.z * (u_clippingNear - u_clippingFar));
    clipPos.xyz = screenPos * clipPos.w;

    vec4 imgPos = invWorldViewProj * clipPos;

    return imgPos.xyz / imgPos.w;
}

//-----------------------------------------------------------------------------

void composite(inout vec4 dest, in vec4 src)
{
    // Front-to-back blending
    dest.rgb = dest.rgb + (1 - dest.a) * src.a * src.rgb;
    dest.a   = dest.a   + (1 - dest.a) * src.a;
}

//-----------------------------------------------------------------------------

void launchRay(inout vec3 rayPos, in vec3 rayDir, in float rayLength, in float sampleDistance, inout vec4 IC_RayTracing, inout vec4 IC_JFA)
{
#if IDVR == 1
    IC_JFA = vec4(0.0);
    //IC_RayTracing = vec4(rayPos, 1.0);
    IC_RayTracing = vec4(0.0, 0.0, 0.0, 1.0);
#endif
#if IDVR == 2
    IC_RayTracing = vec4(0.0, 0.0, 0.0, 0.0);
#endif
#if IDVR == 3
    IC_RayTracing = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 alphaAccum = vec4(0.0);
#endif

    int iterCount = 0;

    // For the segmentation we have a [0, 255] range
    // So we check for value superior to 128: 128 / 65536 = 0.001953125
    float edge = 0.5 + 0.001953125;

    float nbSamples = 0.0f;

    for(float t = 0; iterCount < 65000 && t < rayLength; iterCount += 1, t += sampleDistance)
    {
        float maskValue = texture(u_mask, rayPos).r;

// Maximum Importance compositing
#if IDVR == 1
        if(maskValue > edge)
        {
            IC_RayTracing = vec4(rayPos, 1.);
            //IC_JFA = vec4(gl_FragCoord.x / u_viewportWidth, gl_FragCoord.y / u_viewportHeight, rayPos.z, 1.);
            IC_JFA = vec4(uv.x, uv.y, rayPos.z, 1.);
            break;
        }
#endif
#if IDVR == 2
// Average Importance compositing
// We count the number of samples until the first important object
// We will ponder those samples by the lowest important samples
        if(maskValue > edge)
        {
            IC_RayTracing = vec4(nbSamples, 0.0f, 0.0f, 1.0f);
            break;
        }
#endif
#if IDVR == 3
// Visibility preserving Importance Compositing

        float intensity = texture(u_image, rayPos).r;
        vec4  tfColour  = sampleTransferFunction(intensity);

        composite(alphaAccum, tfColour);

        if(maskValue > edge)
        {
            IC_RayTracing = vec4(nbSamples, alphaAccum.a, 0.0f, 1.0f);
            break;
        }
#endif

        rayPos += rayDir;
        nbSamples += 1.0f;
    }
}

//-----------------------------------------------------------------------------

void main(void)
{
    vec2 rayEntryExit = texture(u_entryPoints, uv).rg;

    float entryDepth =  rayEntryExit.r;
    float exitDepth  = -rayEntryExit.g;

    if(/*gl_FragCoord.z > entryDepth ||*/ exitDepth == -1)
    {
        discard;
    }

    vec3 rayEntry = getFragmentImageSpacePosition(entryDepth, u_invWorldViewProj);
    vec3 rayExit  = getFragmentImageSpacePosition(exitDepth, u_invWorldViewProj);

    vec3 rayDir   = normalize(rayExit - rayEntry) * u_sampleDistance;

    float rayLength = length(rayExit - rayEntry);

    vec3 rayPos = rayEntry;

#if IDVR == 1
    launchRay(rayPos, rayDir, rayLength, u_sampleDistance, mrt_IC_RayTracing, mrt_IC_JFA);
#else
#if IDVR == 2 || IDVR == 3
    vec4 mrt_IC_JFA = vec4(0.0, 0.0, 0.0, 1.0);
    launchRay(rayPos, rayDir, rayLength, u_sampleDistance, mrt_IC_RayTracing, mrt_IC_JFA);
#endif
#endif
}
