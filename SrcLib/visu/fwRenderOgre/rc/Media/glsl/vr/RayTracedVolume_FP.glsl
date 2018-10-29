#version 330

#define MAX_ITERATIONS 8192

uniform sampler3D u_image;
uniform sampler1D u_tfTexture;
uniform vec2 u_tfWindow;

#if IDVR >= 1
uniform sampler2D u_IC;
#endif

#if IDVR == 1
uniform sampler2D u_JFA;
uniform vec2 u_CSGTFWindow;
uniform sampler1D u_CSGTFTexture;
#endif

#if IDVR == 2 || IDVR == 3
uniform sampler3D u_mask;
#endif

#ifdef AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
uniform sampler3D u_illuminationVolume;
uniform vec4 u_volIllumFactor;
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

uniform sampler2D u_entryPoints;

#ifdef AUTOSTEREO
uniform mat4 u_invWorldView;
uniform mat4 u_invProj;
#else // AUTOSTEREO
uniform mat4 u_invWorldViewProj;
uniform mat4 u_worldViewProj;
#endif // AUTOSTEREO

uniform vec3 u_cameraPos;
uniform float u_shininess;

#define MAX_LIGHTS 10

uniform float u_numLights;

uniform vec3 u_lightDir[MAX_LIGHTS];
uniform vec3 u_lightDiffuse[MAX_LIGHTS];
uniform vec3 u_lightSpecular[MAX_LIGHTS];

uniform float u_sampleDistance;

uniform vec4 u_viewport;

uniform float u_clippingNear;
uniform float u_clippingFar;

#ifdef PREINTEGRATION
uniform sampler2D u_tfTexture;
uniform int u_min;
uniform int u_max;
#else // PREINTEGRATION
uniform float u_opacityCorrectionFactor;
#endif // PREINTEGRATION

#if IDVR == 1
uniform float u_csgAngleCos;
// FIXME: find a new way to display the csg border.
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
#endif IDVR == 2

#if IDVR == 3
uniform float u_vpimcAlphaCorrection;
#endif IDVR == 3

#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6
uniform float u_colorModulationFactor;
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6

#ifdef CSG_OPACITY_DECREASE
uniform float u_opacityDecreaseFactor;
#endif // CSG_OPACITY_DECREASE

out vec4 fragColor;

//-----------------------------------------------------------------------------

vec4 sampleTransferFunction(float _intensity, in sampler1D _sampler, in vec2 _window);

//-----------------------------------------------------------------------------

vec3 lighting(vec3 _normal, vec3 _position, vec3 _diffuse)
{
    vec3 vecToCam = normalize(u_cameraPos - _position);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for(int i = 0; i < int(u_numLights); ++i)
    {
        // We use the Blinn-Phong lighting model.
        float fLitDiffuse = clamp(abs(dot( normalize(-u_lightDir[i]), _normal )), 0, 1);
        diffuse += fLitDiffuse * u_lightDiffuse[i] * _diffuse;

        vec3 H = normalize(u_lightDir[i] + vecToCam);
        float fLitSpecular = clamp(pow( abs(dot( _normal, H )), u_shininess), 0, 1);
        specular += fLitSpecular * u_lightSpecular[i];
    }

    return vec3(diffuse + specular);
}

//-----------------------------------------------------------------------------

vec3 gradientNormal(vec3 _uvw)
{
    ivec3 imgDimensions = textureSize(u_image, 0);
    vec3 h = 1. / vec3(imgDimensions);
    vec3 hx = vec3(h.x, 0, 0);
    vec3 hy = vec3(0, h.y, 0);
    vec3 hz = vec3(0, 0, h.z);

    return -normalize( vec3(
                (texture(u_image, _uvw + hx).r - texture(u_image, _uvw - hx).r),
                (texture(u_image, _uvw + hy).r - texture(u_image, _uvw - hy).r),
                (texture(u_image, _uvw + hz).r - texture(u_image, _uvw - hz).r)
    ));
}

//-----------------------------------------------------------------------------

/// Converts OpenGL fragment coordinates to normalized device coordinates (NDC).
vec3 fragCoordsToNDC(in vec3 _fragCoord)
{
    vec3 ndcCoords  = vec3(_fragCoord.xy * u_viewport.zw, _fragCoord.z);
    ndcCoords = (ndcCoords - 0.5) * 2.;
    return ndcCoords;
}

//-----------------------------------------------------------------------------

float voxelScreenDepth(in vec3 _pos)
{
    vec4 projPos = u_worldViewProj * vec4(_pos, 1);

    return projPos.z / projPos.w;
}

//-----------------------------------------------------------------------------

/// Converts a position in OpenGL's normalized device coordinates (NDC) to object space.
vec3 ndcToVolumeSpacePosition(in vec3 _ndcPos, in mat4 _invWorldViewProj)
{
    vec4 clipPos;
    clipPos.w   = (2 * u_clippingNear * u_clippingFar)  / (u_clippingNear + u_clippingFar + _ndcPos.z * (u_clippingNear - u_clippingFar));
    clipPos.xyz = _ndcPos * clipPos.w;

    vec4 imgPos = _invWorldViewProj * clipPos;

    return imgPos.xyz / imgPos.w;
}

//-----------------------------------------------------------------------------

#ifdef PREINTEGRATION
vec4 samplePreIntegrationTable(vec3 _rayBack, vec3 _rayFront)
{
    float sf = texture(u_image, _rayBack).r;
    float sb = texture(u_image, _rayFront).r;

    sf = ((sf * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);
    sb = ((sb * 65535.f) - float(u_min) - 32767.f) / float(u_max - u_min);

    return texture(u_tfTexture, vec2(sf, sb));
}
#endif // PREINTEGRATION

//-----------------------------------------------------------------------------

void composite(inout vec4 _dest, in vec4 _src)
{
    // Front-to-back blending
    _dest.rgb = _dest.rgb + (1 - _dest.a) * _src.a * _src.rgb;
    _dest.a   = _dest.a   + (1 - _dest.a) * _src.a;
}

//-----------------------------------------------------------------------------

#ifdef CSG

// Returns true if the ray hits the cone, the origin is then moved to the intersection point.
bool rayConeIntersection(in vec3 _coneOrigin, in vec3 _coneDir, in float _coneAngleCos, inout vec3 _rayOrigin, in vec3 _rayDir)
{
    // Vector from the cone origin to the ray origin.
    vec3 origDir = _rayOrigin - _coneOrigin;
    float squaredAngleCos = _coneAngleCos * _coneAngleCos;

    float dirDot = dot(_rayDir, _coneDir);
    float origConeDirDot = dot(origDir, _coneDir);

    // Angle cosine between the cone direction and the origin to origin vector.
    float origAngleCos = dot(normalize(origDir), normalize(_coneDir));

    // Ensure that the ray origin is 'inside' the cone.
    if(origAngleCos < _coneAngleCos && origAngleCos > 0)
    {
        return false;
    }

    // We're looking for a point P belonging to the ray and the cone, P should therefore verify the following equations:
    //     · ((P - _coneOrigin) /  length(P - _coneOrigin)) * _coneDir = cos(coneAngle)
    //     · P = _rayOrigin + t * _rayDir
    // When simplifying this system we end up with a second degree polynomial a * t² + b * t + c with the following
    // coefficients :
    float a = dirDot * dirDot - squaredAngleCos;
    float b = 2 * (dirDot * origConeDirDot - dot(_rayDir, origDir) * squaredAngleCos);
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

        intersection = _rayOrigin + t * _rayDir;

        // Check if we hit the cone or its 'shadow'
        // i.e. check if the intersection is 'in front' or 'behind' the cone origin.
        vec3 cp = intersection - _coneOrigin;
        float h = dot(cp, _coneDir);

        if (h < 0.) // We hit the shadow ...
        {
            return false;
        }
    }
    else // No solution to the polynomial = no intersection
    {
        return false;
    }

    _rayOrigin = intersection;
    return true;
}

#ifdef CSG_MODULATION || CSG_OPACITY_DECREASE
// Computes the orthogonal distance from a point to a line.
float pointLineDistance(in vec3 _point, in vec3 _linePoint, in vec3 _lineUnitDir)
{
    vec3 line2Point = _point - _linePoint;
    return length(line2Point - dot(line2Point, _lineUnitDir) * _lineUnitDir);
}
#endif // CSG_MODULATION || CSG_OPACITY_DECREASE

#endif // CSG

//-----------------------------------------------------------------------------

#if CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6
vec3 rgb2hsv(vec3 RGB);
vec3 hsv2rgb(vec3 HSL);
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6

//-----------------------------------------------------------------------------

vec4 launchRay(in vec3 _rayPos, in vec3 _rayDir, in float _rayLength, in float _sampleDistance, in sampler1D _tfTexture, in vec2 _tfWindow)
{
    vec4 result = vec4(0);

#if IDVR == 2 || IDVR == 3
    // For the segmentation we have a [0, 255] range
    // So we check for value superior to 128: 128 / 65536 = 0.001953125
    float edge = 0.5 + 0.001953125;

// With Visibility preserving importance compositing
// We count the number of samples until we reach the important feature
    float nbSamples = 0.0;
#endif // IDVR == 2 || IDVR == 3

    int iterCount = 0;
    float t = 0.f;
    // Move the ray to the first non transparent voxel.
    for(; iterCount < MAX_ITERATIONS && t < _rayLength; iterCount += 1, t += _sampleDistance)
    {
#ifdef PREINTEGRATION
        float tfAlpha = samplePreIntegrationTable(_rayPos, _rayPos + _rayDir).a;
#else // PREINTEGRATION
        float intensity = texture(u_image, _rayPos).r;
        float tfAlpha = sampleTransferFunction(intensity, _tfTexture, _tfWindow).a;
#endif // PREINTEGRATION

        if(tfAlpha != 0)
        {
            break;
        }

        _rayPos += _rayDir;
    }

    float rayScreenDepth = voxelScreenDepth(_rayPos);
    gl_FragDepth = rayScreenDepth * 0.5f + 0.5f; // Convert to NDC assuming no clipping planes are set.

    for(; iterCount < MAX_ITERATIONS && t < _rayLength; iterCount += 1, t += _sampleDistance)
    {
#ifdef PREINTEGRATION
        vec4 tfColour = samplePreIntegrationTable(_rayPos, _rayPos + _rayDir);
#else // PREINTEGRATION
        float intensity = texture(u_image, _rayPos).r;
        vec4 tfColour = sampleTransferFunction(intensity, _tfTexture, _tfWindow);
#endif // PREINTEGRATION

        if(tfColour.a > 0)
        {
            vec3 N = gradientNormal(_rayPos);

            tfColour.rgb = lighting(N, _rayPos, tfColour.rgb);

#ifndef PREINTEGRATION
            // Adjust opacity to sample distance.
            // This could be done when generating the TF texture to improve performance.
            tfColour.a = 1 - pow(1 - tfColour.a, u_sampleDistance * u_opacityCorrectionFactor);
#endif // PREINTEGRATION

#ifdef AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS
            vec4 volIllum = texture(u_illuminationVolume, _rayPos);
#endif // AMBIENT_OCCLUSION || COLOR_BLEEDING || SHADOWS

#ifdef AMBIENT_OCCLUSION || SHADOWS
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

        _rayPos += _rayDir;
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

    if(exitDepth == -1 || exitDepth < entryDepth)
    {
        discard;
    }

#ifdef AUTOSTEREO
    mat4x4 invWorldViewProj = u_invWorldView * u_invProj;
#else // AUTOSTEREO
    mat4x4 invWorldViewProj = u_invWorldViewProj;
#endif // AUTOSTEREO

    // Entry points in window space.
    vec3 entryFrag = fragCoordsToNDC(vec3(gl_FragCoord.xy, entryDepth));
    vec3 exitFrag = fragCoordsToNDC(vec3(gl_FragCoord.xy, exitDepth));

    // Entry points in volume texture space.
    vec3 rayEntry = ndcToVolumeSpacePosition(entryFrag, invWorldViewProj);
    vec3 rayExit  = ndcToVolumeSpacePosition(exitFrag, invWorldViewProj);

    vec3 rayDir   = normalize(rayExit - rayEntry);

#if IDVR == 1

#ifdef CSG
#ifdef CSG_DEPTH_LINES
    float rayDepth = 0.f;
#endif //CSG_DEPTH_LINES

    vec4 jfaDistance = vec4(0.f);
    bool isCsg = false; // true if this ray hits the csg.

#ifdef CSG_MODULATION || CSG_OPACITY_DECREASE
    float coneDistance = 0.f;
#endif // CSG_MODULATION || CSG_OPACITY_DECREASE

#endif // CSG

    vec3 rayCSGEntry = rayEntry;
    vec4 importance = texelFetch(u_IC, ivec2(gl_FragCoord.xy), 0);

    // If we have an importance factor, we move the ray accordingly
    if(importance.a > 0.)
    {
        rayEntry = importance.rgb;
#ifdef CSG
        isCsg = true;
#endif // CSG
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

        bool hit = rayConeIntersection(scaledClosestPt, coneDir, u_csgAngleCos, scaledEntry, scaledDir);

        if(hit)
        {
#ifdef CSG_MODULATION || CSG_OPACITY_DECREASE
            // Ray entry to central cone line distance.
            coneDistance = pointLineDistance(scaledEntry, scaledClosestPt, coneDir);
#endif // CSG_MODULATION || CSG_OPACITY_DECREASE

            // Back to volume texture space.
            rayEntry = scaledEntry / normSpacing;

            vec3 volDimensions = vec3(textureSize(u_image, 0));
            vec3 volSize = u_imageSpacing * volDimensions;

#ifdef CSG_DEPTH_LINES
            // Compute the ray depth in the image spacing's unit (typically millimeters for medical images).
            vec3 cVector = normalize(u_cameraPos - closestPt) * volSize;
            vec3 cOrig2RayEntry = rayEntry * volSize - closestPt * volSize;
            rayDepth = dot(cVector, cOrig2RayEntry) / length(cVector);
#endif //CSG_DEPTH_LINES

            isCsg = true;
        }

#ifdef CSG_DISABLE_CONTEXT
        else
        {
            discard;
        }
#endif // CSG_DISABLE_CONTEXT
    }
#endif // CSG
#endif // IDVR == 1

    rayDir *= u_sampleDistance;

    float rayLength = length(rayExit - rayEntry);

    vec3 rayPos = rayEntry;

#ifndef CSG
    vec4 result = launchRay(rayPos, rayDir, rayLength, u_sampleDistance, u_tfTexture, u_tfWindow);
#else // CSG

    vec4 result;

    if(isCsg)
    {
        float entryIntensity = texture(u_image, rayEntry).r;
        float entryOpacity = sampleTransferFunction(entryIntensity, u_tfTexture, u_tfWindow).a;
#ifdef CSG_DEPTH_LINES
        float rayDepthIntegralPart;
        float rayDepthFractPart = modf(rayDepth, rayDepthIntegralPart);

        int distToDepthLine = int(mod(rayDepthIntegralPart, u_depthLinesSpacing));

        if(entryOpacity > 0.f && rayDepth > 0.0f && int(rayDepthIntegralPart) != 0 && distToDepthLine == 0 && (rayDepthFractPart < u_depthLinesWidth))
        {
            result = vec4(u_csgBorderColor, 1.);
        }
        else
#endif // CSG_DEPTH_LINES
        if(entryOpacity > 0.f)
        {
            vec4 color = launchRay(rayPos, rayDir, rayLength, u_sampleDistance, u_tfTexture, u_tfWindow);
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
#endif // CSG_MODULATION == 4 || CSG_MODULATION == 5 || CSG_MODULATION == 6

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
            float alphaModDecr = max((coneDistance / (1.f - u_opacityDecreaseFactor)), 0.f);
            color.a -= alphaModDecr;
#endif // CSG_OPACITY_DECREASE

            result = color;
        }
        else
        {
            result = launchRay(rayPos, rayDir, rayLength, u_sampleDistance, u_tfTexture, u_tfWindow);
        }

#ifdef CSG_TF
        // If the second TF usage is enable, we fill the CSG with it.
        float rayCSGLength = length(rayEntry - rayCSGEntry);
        vec3 rayCSGPos = rayCSGEntry;
        vec4 colorCSG = launchRay(rayCSGPos, rayDir, rayCSGLength, u_sampleDistance, u_CSGTFTexture, u_CSGTFWindow);
#else // CSG_TF
        // Else, we use an empty color.
        vec4 colorCSG = vec4(0.0, 0.0, 0.0, 0.0);
#endif // CSG_TF

#ifdef CSG_DISABLE_CONTEXT
        if(entryOpacity > 0.f)
        {
            composite(colorCSG, result);
        }
#else
        composite(colorCSG, result);
#endif // CSG_DISABLE_CONTEXT
        result = colorCSG;
    }
    else
    {
        result = launchRay(rayPos, rayDir, rayLength, u_sampleDistance, u_tfTexture, u_tfWindow);
    }
#endif // CSG

    fragColor = result;
}
