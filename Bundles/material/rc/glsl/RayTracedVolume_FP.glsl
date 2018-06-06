#version 330

uniform sampler3D u_image;

#if IDVR >= 1
uniform sampler2D u_IC;
#endif
#if IDVR == 1
uniform sampler2D u_JFA;
#endif
#if IDVR == 2 || IDVR == 3
uniform sampler3D u_mask;
#endif

#if AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
uniform sampler3D u_illuminationVolume;
uniform vec4 u_volIllumFactor;
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

uniform sampler2D u_entryPoints;

#ifdef AUTOSTEREO
uniform mat4 u_invWorldView;
uniform mat4 u_invProj;
#else
uniform mat4 u_invWorldViewProj;
#endif // AUTOSTEREO

uniform vec3 u_cameraPos;
uniform float u_shininess;

#define MAX_LIGHTS 10

uniform float u_numLights;

uniform vec3 u_lightDir[MAX_LIGHTS];
uniform vec3 u_lightDiffuse[MAX_LIGHTS];
uniform vec3 u_lightSpecular[MAX_LIGHTS];

uniform float u_sampleDistance;

uniform float u_viewportWidth;
uniform float u_viewportHeight;

uniform float u_clippingNear;
uniform float u_clippingFar;

#ifdef PREINTEGRATION
uniform sampler2D u_tfTexture;
uniform int u_min;
uniform int u_max;
#else
uniform float u_opacityCorrectionFactor;
#endif // PREINTEGRATION

#if IDVR == 1
uniform float u_csgAngleCos;
uniform float u_csgBorderThickness;
uniform vec3 u_csgBorderColor;
uniform vec3 u_imageSpacing;
#ifdef CSG_DEPTH_LINES
// Number of image spacing units (millimeters usually) separating depth lines.
uniform int u_depthLinesSpacing;
uniform float u_depthLinesWidth;
#endif // CSG_DEPTH_LINES
#endif // IDVR == 1
#if IDVR == 2
uniform float u_aimcAlphaCorrection;
#endif
#if IDVR == 3
uniform float u_vpimcAlphaCorrection;
#endif

#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6
uniform float u_colorModulationFactor;
#endif

#ifdef CSG_OPACITY_DECREASE
uniform float u_opacityFactor;
#endif // CSG_OPACITY_DECREASE

out vec4 fragColor;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float intensity);

//-----------------------------------------------------------------------------

vec3 lighting(vec3 _normal, vec3 _position, vec3 _diffuse)
{
    vec3 vecToCam = normalize(u_cameraPos - _position);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for(int i = 0; i < int(u_numLights); ++i)
    {
        float fLitDiffuse = clamp(dot( normalize(-u_lightDir[i]), _normal ), 0, 1);
        diffuse += fLitDiffuse * u_lightDiffuse[i] * _diffuse;

        vec3 r = reflect(u_lightDir[i], _normal);
        float fLitSpecular = pow( clamp(dot( r, vecToCam ), 0, 1), u_shininess);
        specular += fLitSpecular * u_lightSpecular[i];
    }

    return vec3(diffuse + specular);
}

//-----------------------------------------------------------------------------

vec3 gradientNormal(vec3 uvw)
{
    ivec3 imgDimensions = textureSize(u_image, 0);
    vec3 h = 1. / vec3(imgDimensions);
    vec3 hx = vec3(h.x, 0, 0);
    vec3 hy = vec3(0, h.y, 0);
    vec3 hz = vec3(0, 0, h.z);

    return -normalize( vec3(
                (texture(u_image, uvw + hx).r - texture(u_image, uvw - hx).r),
                (texture(u_image, uvw + hy).r - texture(u_image, uvw - hy).r),
                (texture(u_image, uvw + hz).r - texture(u_image, uvw - hz).r)
    ));
}

//-----------------------------------------------------------------------------

/// Converts OpenGL fragment coordinates to normalized device coordinates (NDC).
vec3 fragCoordsToNDC(in vec3 fragCoord)
{
    vec3 ndcCoords  =vec3(fragCoord.xy / vec2(u_viewportWidth, u_viewportHeight), fragCoord.z);
    ndcCoords = (ndcCoords - 0.5) * 2.;
    return ndcCoords;
}

//-----------------------------------------------------------------------------

/// Converts a position in OpenGL's normalized device coordinates (NDC) to object space.
vec3 ndcToVolumeSpacePosition(in vec3 ndcPos, in mat4 invWorldViewProj)
{
    vec4 clipPos;
    clipPos.w   = (2 * u_clippingNear * u_clippingFar)  / (u_clippingNear + u_clippingFar + ndcPos.z * (u_clippingNear - u_clippingFar));
    clipPos.xyz = ndcPos * clipPos.w;

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

#if IDVR == 1 && CSG

// Returns true if the ray hits the cone, the origin is then moved to the intersection point.
bool rayConeIntersection(in vec3 coneOrigin, in vec3 coneDir, in float coneAngleCos, inout vec3 rayOrigin, in vec3 rayDir)
{
    // Vector from the cone origin to the ray origin.
    vec3 origDir = rayOrigin - coneOrigin;
    float squaredAngleCos = coneAngleCos * coneAngleCos;

    float dirDot = dot(rayDir, coneDir);
    float origConeDirDot = dot(origDir, coneDir);

    // We're looking for a point P belonging to the ray and the cone, P should therefore verify the following equations:
    //     · ((P - coneOrigin) /  length(P - coneOrigin)) * coneDir = cos(coneAngle)
    //     · P = rayOrigin + t * rayDir
    // When simplifying this system we end up with a second degree polynomial a * t² + b * t + c with the following
    // coefficients :
    float a = dirDot * dirDot - squaredAngleCos;
    float b = 2 * (dirDot * origConeDirDot - dot(rayDir, origDir) * squaredAngleCos);
    float c = origConeDirDot * origConeDirDot - dot(origDir, origDir) * squaredAngleCos;

    // Solving the polynomial is trivial.
    float delta = b * b - 4. * a * c;

    vec3 intersection;
    if(delta > 0)
    {
        float sqrtDelta = sqrt(delta);
        float t1 = (-b - sqrtDelta) / (2. * a);
        float t2 = (-b + sqrtDelta) / (2. * a);

        // We're looking for the point closest to the ray origin.
        float t = min(t1, t2);

        if (t < 0.)
        {
            return false;
        }

        intersection = rayOrigin + t * rayDir;

        // Check if we hit the cone or its 'shadow'
        // i.e. check if the intersection is 'in front' or 'behind' the cone origin.
        vec3 cp = intersection - coneOrigin;
        float h = dot(cp, coneDir);

        if (h < 0.) // We hit the shadow ...
        {
            return false;
        }
    }
    else // No solution to the polynomial = no intersection
    {
        return false;
    }

    rayOrigin = intersection;
    return true;
}

#ifdef CSG_MODULATION

// Computes the orthogonal distance from a point to a line.
float pointLineDistance(in vec3 point, in vec3 linePoint, in vec3 lineUnitDir)
{
    vec3 line2Point = point - linePoint;
    return length(line2Point - dot(line2Point, lineUnitDir) * lineUnitDir);
}

#endif // CSG_MODULATION

#endif

//-----------------------------------------------------------------------------

#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6 || CSG_MODULATION == 7
vec3 rgb2hsv(vec3 RGB);
vec3 hsv2rgb(vec3 HSL);
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6 || CSG_MODULATION == 7

//-----------------------------------------------------------------------------

vec4 launchRay(in vec3 rayPos, in vec3 rayDir, in float rayLength, in float sampleDistance)
{
    vec4 result = vec4(0);

#if IDVR == 2 || IDVR == 3
    // For the segmentation we have a [0, 255] range
    // So we check for value superior to 128: 128 / 65536 = 0.001953125
    float edge = 0.5 + 0.001953125;

// With Visibility preserving importance compositing
// We count the number of samples until we reach the important feature
    float nbSamples = 0.0;
#endif

    int iterCount = 0;
    for(float t = 0; iterCount < 65000 && t < rayLength; iterCount += 1, t += sampleDistance)
    {
#ifdef PREINTEGRATION
        float sf = texture(u_image, rayPos).r;
        float sb = texture(u_image, rayPos + rayDir).r;

        sf = ((sf * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);
        sb = ((sb * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);

        vec4 tfColour = texture(u_tfTexture, vec2(sf, sb));

#else
        float intensity = texture(u_image, rayPos).r;

        vec4  tfColour = sampleTransferFunction(intensity);
#endif // PREINTEGRATION

        if(tfColour.a > 0)
        {
            vec3 N = gradientNormal(rayPos);

            tfColour.rgb = lighting(N, rayPos, tfColour.rgb);

#ifndef PREINTEGRATION
            // Adjust opacity to sample distance.
            // This could be done when generating the TF texture to improve performance.
            tfColour.a = 1 - pow(1 - tfColour.a, u_sampleDistance * u_opacityCorrectionFactor);
#endif // PREINTEGRATION

#if AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
            vec4 volIllum = texture(u_illuminationVolume, rayPos);
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

#if AMBIENT_OCCLUSION || SHADOWS
            // Apply ambient occlusion + shadows
            tfColour.rgb *= pow(exp(-volIllum.a), u_volIllumFactor.a);
#endif // AMBIENT_OCCLUSION || SHADOWS

#ifdef COLOR_BLEEDING
            // Apply color bleeding
            tfColour.rgb *= pow(1+volIllum.rgb, u_volIllumFactor.rgb);
#endif // COLOR_BLEEDING

// Average Importance Compositing
#if IDVR == 2
            vec4 aimc = texelFetch(u_IC, ivec2(gl_FragCoord.xy), 0);
            // We ensure that we have a number of samples > 0, to be in the region of interest
            if(aimc.r > 0 && nbSamples <= aimc.r)
            {
                tfColour.a = tfColour.a * u_aimcAlphaCorrection;
            }
#endif // IDVR == 2

#if IDVR == 3
            vec4 aimc = texelFetch(u_IC, ivec2(gl_FragCoord.xy), 0);
            // We ensure that we have a number of samples > 0, to be in the region of interest
            if(aimc.r > 0 && int(nbSamples) == int(aimc.r))
            {
                result.rgb = result.rgb * u_vpimcAlphaCorrection;
                result.a = u_vpimcAlphaCorrection;
            }
#endif // IDVR == 3

            composite(result, tfColour);

            if(result.a > 0.99
#if IDVR == 3
// In the case of Visibility preserving importance compositing
// We need to ensure that we reach a certain amount of samples
// Before cutting off the compositing and breaking the ray casting
// Otherwise we will miss the important feature
            && int(nbSamples) > int(aimc.r)
#endif // IDVR == 3
            )
            {
                break;
            }
        }

        rayPos += rayDir;
#if IDVR == 2 || IDVR == 3
// With Visibility preserving importance compositing
// We count the number of samples until we reach the important feature
        nbSamples += 1.0f;
#endif // IDVR == 2 || IDVR == 3
    }

    return result;
}

//-----------------------------------------------------------------------------

void main(void)
{
    vec2 rayEntryExit = texelFetch(u_entryPoints, ivec2(gl_FragCoord.xy), 0).rg;

    float entryDepth =  rayEntryExit.r;
    float exitDepth  = -rayEntryExit.g;

    if(exitDepth == -1)
    {
        discard;
    }

    gl_FragDepth = entryDepth;

#ifdef AUTOSTEREO
    mat4x4 invWorldViewProj = u_invWorldView * u_invProj;
#else
    mat4x4 invWorldViewProj = u_invWorldViewProj;
#endif

    // Entry points in window space.
    vec3 entryFrag = fragCoordsToNDC(vec3(gl_FragCoord.xy, entryDepth));
    vec3 exitFrag = fragCoordsToNDC(vec3(gl_FragCoord.xy, exitDepth));

    // Entry points in volume texture space.
    vec3 rayEntry = ndcToVolumeSpacePosition(entryFrag, invWorldViewProj);
    vec3 rayExit  = ndcToVolumeSpacePosition(exitFrag, invWorldViewProj);

    vec3 rayDir   = normalize(rayExit - rayEntry);

#if IDVR == 1

#ifdef CSG
    float rayDepth = 0.f;

    vec4 jfaDistance = vec4(0.f);
    bool isCsg = false; // true if this ray hits the csg.

#if CSG_MODULATION || CSG_OPACITY_DECREASE
    float coneDistance = 0.f;
#endif // CSG_MODULATION || CSG_OPACITY_DECREASE

#endif // CSG

    vec4 importance = texelFetch(u_IC, ivec2(gl_FragCoord.xy), 0);

    // If we have an importance factor, we move the ray accordingly
    if(importance.a > 0.)
    {
        rayEntry = importance.rgb;
    }
#ifdef CSG
    // Otherwise, we use the distance to the closest important point
    // to dig into the volume
    else
    {
        jfaDistance = texelFetch(u_JFA, ivec2(gl_FragCoord.xy), 0);

        vec3 normSpacing = normalize(u_imageSpacing);

        // Closest mask point to the entry point.
        vec3 closestPt = ndcToVolumeSpacePosition(jfaDistance.xyz, invWorldViewProj);
        vec3 scaledClosestPt = closestPt * normSpacing;

        // Scale points and vectors to take voxel anisotropy into account when computing the countersink geometry.
        vec3 coneDir = normalize(u_cameraPos * normSpacing - scaledClosestPt);
        vec3 scaledEntry = rayEntry * normSpacing;
        vec3 scaledDir   = normalize(rayDir * normSpacing);

        vec3 oldEntry = rayEntry;
        bool hit = rayConeIntersection(scaledClosestPt, coneDir, u_csgAngleCos, scaledEntry, scaledDir);

        if(hit)
        {
#if CSG_MODULATION || CSG_OPACITY_DECREASE
            // Ray entry to central cone line distance.
            coneDistance = pointLineDistance(scaledEntry, scaledClosestPt, coneDir);
#endif // CSG_MODULATION || CSG_OPACITY_DECREASE

            // Back to volume texture space.
            rayEntry = scaledEntry / normSpacing;

            vec3 volDimensions = vec3(textureSize(u_image, 0));
            vec3 volSize = u_imageSpacing * volDimensions;

            // Compute the ray depth in the image spacing's unit (typically millimeters for medical images).
            vec3 cVector = normalize(u_cameraPos - closestPt) * volSize;
            vec3 cOrig2RayEntry = rayEntry * volSize - closestPt * volSize;
            rayDepth = dot(cVector, cOrig2RayEntry) / length(cVector);

            // If the new entry point hits a transparent zone then we discard it.
            float entryIntensity = texture(u_image, rayEntry).r;
            float entryOpacity = sampleTransferFunction(entryIntensity).a;

            isCsg = entryOpacity > 0.f;

#if CSG_DISABLE_CONTEXT == 1
            if(!isCsg)
            {
                discard;
            }
#endif // CSG_DISABLE_CONTEXT == 1
        }

#if CSG_DISABLE_CONTEXT == 1
        else
        {
            discard;
        }
#endif // CSG_DISABLE_CONTEXT == 1
    }
#endif // CSG
#endif // IDVR == 1

    rayDir *= u_sampleDistance;

    float rayLength = length(rayExit - rayEntry);

    vec3 rayPos = rayEntry;

#ifndef CSG
    vec4 result = launchRay(rayPos, rayDir, rayLength, u_sampleDistance);
#else
    vec4 result;

    if(rayDepth > 0 && isCsg)
    {

#ifdef CSG_DEPTH_LINES
        float rayDepthIntegralPart;
        float rayDepthFractPart = modf(rayDepth, rayDepthIntegralPart);
#endif // CSG_DEPTH_LINES
#if CSG_BORDER == 1
        if(rayDepth > u_csgBorderThickness
#ifdef CSG_DEPTH_LINES
           && mod(int(rayDepthIntegralPart), u_depthLinesSpacing) == 0
                && (rayDepthFractPart < u_depthLinesWidth)
#endif // CSG_DEPTH_LINES == 1
        )
        {
            result = vec4(u_csgBorderColor, 1.);
        }
        else
        {
#endif // CSG_BORDER == 1
            vec4 color = launchRay(rayPos, rayDir, rayLength, u_sampleDistance);

// Average grayscale
#if CSG_GRAYSCALE == 1
            // The average method simply averages the values
            float grayScale = (color.r + color.g + color.b) / 3.;
            color.rgb = vec3(grayScale);
#endif // CSG_GRAYSCALE == 1
// Lightness grayscale
#if CSG_GRAYSCALE == 2
            // The lightness method averages the most prominent and least prominent colors
            float grayScale =
                (max(color.r, max(color.g, color.b)) +
                 min(color.r, min(color.g, color.b))) / 2.;

            color.rgb = vec3(grayScale);
#endif // CSG_GRAYSCALE == 2
// Luminosity grayscale
#if CSG_GRAYSCALE == 3
            // The luminosity method is a more sophisticated version of the average method.
            // It also averages the values, but it forms a weighted average to account for human perception.
            // We’re more sensitive to green than other colors, so green is weighted most heavily.
            float grayScale = 0.21 * color.r + 0.72 * color.g + 0.07 * color.b;
            color.rgb = vec3(grayScale);
#endif // CSG_GRAYSCALE == 3

// CSG luminance and saturation modulations
// Brightness increase (CSG_MODULATION == 4)
// Saturation increase (CSG_MODULATION == 5)
// Saturation and brightness increase (CSG_MODULATION == 6)
#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6
            float modIncr = max(1.f - (coneDistance / abs(u_colorModulationFactor)), 0.f);
#endif CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6 ||  CSG_OPACITY_DECREASE

#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6
            vec3 hsv = rgb2hsv(color.rgb);

#if CSG_MODULATION == 5 || CSG_MODULATION == 6
            hsv.g += modIncr * sign(u_colorModulationFactor);
#endif // CSG_MODULATION == 5 || CSG_MODULATION == 6

#if CSG_MODULATION == 4 || CSG_MODULATION == 6
            hsv.b += modIncr * sign(u_colorModulationFactor);
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 6

            color.rgb = hsv2rgb(hsv);
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6

#ifdef CSG_OPACITY_DECREASE
            float alphaModDecr = max(1.f - (coneDistance / u_opacityFactor), 0.f);
            color.a -= alphaModDecr;
#endif // CSG_OPACITY_DECREASE

            result = color;
#if CSG_BORDER == 1
        }
#endif // CSG_BORDER == 1
    }
    else
    {
        result = launchRay(rayPos, rayDir, rayLength, u_sampleDistance);
    }
#endif // CSG

    fragColor = result;

}
