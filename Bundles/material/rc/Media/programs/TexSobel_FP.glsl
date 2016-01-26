#version 330

#define M_PI 3.1415926535897932384626433832795
#define M_TRUE 1

in vec2 uv;

ivec2 offs[9] = ivec2[](
    ivec2(-1, -1), //Top Left     0
    ivec2( 0, -1), //Top          1
    ivec2( 1, -1), //Top Right    2
    ivec2(-1,  0), //Left         3
    ivec2( 0,  0), //Center       4
    ivec2( 0,  1), //Right        5
    ivec2(-1,  1), //Bottom Left  6
    ivec2( 0,  1),  //Bottom       7
    ivec2( 1,  1) //Bottom Right 8
);

float filterSobelX[9] = float[](
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
);

float filterSobelY[9] = float[](
    -1, -2, -1,
    0, 0, 0,
    1, 2, 1
);

float filterMiniPrewittX[9] = float[](
    0, 1, 0,
    0, -1, 0,
    0, 0, 0
);

float filterMiniPrewittY[9] = float[](
    0, 0, 0,
    1, -1, 0,
    0, 0, 0
);

float filterPrewittX[9] = float[](
    -1, 0, 1,
    -1, 0, 1,
    -1, 0, 1
);

float filterPrewittY[9] = float[](
    -1, -1, -1,
    0, 0, 0,
    1, 1, 1
);

float filterLaplacian[9] = float[](
    1, 1, 1,
    1, -8, 1,
    1, 1, 1
);

float filterMeanAngle[9] = float[](
    1, 2, 1,
    2, 0, 2,
    1, 2, 1
);

// Input maps defined in [0;1]
uniform sampler2D depthMap;
uniform sampler2D normalMap;

uniform float near_clip;
uniform float far_clip;

// Variables setted by user to choose normal map filter type and parameters
/**
 * Filter type allowing to choose different filters (0,1 are defaults, >2 are prototypes)
 * 0 : Sobel with interpolation depending on depth
 * 1 : Mini Prewitt
 * 2 : Sobel
 * 3 : Prewitt
 * 4 : Mean Angle
 * 5 : Sobel by comparing angles (NOT WORKING)
 */
uniform float filterType = 0;
// Minimum edge normal accepted value [0;1]
uniform float normalStep = 0.3;
// Step returns computed value instead of 1
uniform int linearNormalStep = 1;
// Minimum edge depth accepted value [0;1]
uniform float depthStep = 0.05;
// Step returns computed value instead of 1
uniform int linearDepthStep = 0;
// If we should divide the convolution product by it's coefficient's sum
uniform int normalizeGradient = 1;
// These uniform are only use when using filterType 0
uniform float lowPass = .3;  // Low depth value when starting to interpolate
uniform float highCut = .8;  // High depth value when stopping to interpolate

layout(location = 0) out vec4 edgeMap;

//---------------------------------------------------------------------------------------------------------------------

float computeFilter(sampler2D map, float[9] filter, bool normalized)
{
    // Because filters change intervals, we must compute sum of its coefficients
    float normalize = 0;
    vec3 g = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        // Getting map values in [-1;1]
        g += (textureOffset(map, uv, offs[i]) * 2.0 - 1.0).xyz * filter[i];
        normalize += abs(filter[i]);
    }
    return normalized ? length(g) / normalize : length(g);
}

//---------------------------------------------------------------------------------------------------------------------

vec3 computeFilterInterpolateWithDistance(sampler2D map, float[9] filter)
{
    vec3 g = vec3(0.0);

    float lowPercent = .3;
    float highPercent = .8;

    float depthRange = far_clip - near_clip;
    float lowPass = near_clip + depthRange * lowPercent;
    float highCut = near_clip + depthRange * highPercent;

    float centerDepth = texture(depthMap, uv).x;

    vec3 centerPixel = normalize(texture(map, uv).xyz * 2.0 - 1.0);
    vec3 currentPixel, interpolatedPixel;

    // Compute linear function for interpolation coefficient
    float a,b;
    a = - (1 / (highPercent - lowPercent));
    b = 1 - a * lowPercent;
    float interpolation;

    // TODO : Find best interpolation function (linear or not)

    for(int i = 0; i < 9; i++)
    {
        currentPixel = normalize(textureOffset(map, uv, offs[i]).xyz * 2.0 - 1.0);

        if (centerDepth < lowPercent)
        {
            interpolatedPixel = currentPixel;
        } else if (centerDepth < highPercent)
        {
            interpolation = a * centerDepth + b;
            interpolatedPixel = currentPixel + (centerPixel - currentPixel) * (1.0 - interpolation);
        } else
        {
            interpolatedPixel = centerPixel;
        }

        // Getting map values in [-1;1]
        g += interpolatedPixel * filter[i];
    }
    return g;
}

//---------------------------------------------------------------------------------------------------------------------

float computeMeanAngle(sampler2D map, float[9] filter)
{
    // Because filters change intervals, we must compute sum of its coefficients
    float norm = 0;
    float g = 0;
    vec3 centerPixel = texture(map,uv).xyz * 2.0 - 1.0;
    centerPixel = normalize(centerPixel);
    vec3 neighborPixel;
    for(int i = 0; i < 9; i++)
    {
        // Getting map values in [-1;1]
        neighborPixel = normalize(textureOffset(map, uv, offs[i]) * 2.0 - 1.0).xyz;

        g += filter[i] * acos(dot(centerPixel, neighborPixel));

        norm += abs(filter[i]);
    }
    return g / norm;
}

//---------------------------------------------------------------------------------------------------------------------

vec3 computeFilterVec4(sampler2D map, float[9] filter)
{
    vec3 g = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        // Getting map values in [-1;1]
        g += (textureOffset(map, uv, offs[i]) * 2.0 - 1.0).xyz * filter[i];
    }
    return g;
}

//---------------------------------------------------------------------------------------------------------------------

float computeConvolution(sampler2D map, float[9] filterX, float[9] filterY, bool normalized)
{
    // Compute horizontal Gx
    float vx = computeFilter(map, filterX, normalized);

    // Compute horizontal Gx
    float vy = computeFilter(map, filterY, normalized);

    // [-1;1] => [0; sqrt(2)]
    float result = sqrt(vx * vx + vy * vy);

    // Getting to the [0,1] interval
    return 0.5 * sqrt(2.0) * result;
}

//---------------------------------------------------------------------------------------------------------------------

void main()
{
    float resultSobel = 0.0;

    float convol;

    if (filterType == 0)
    {
        // TODO : fix wrong interpolation and fix getting triangles for edges
        vec3 sobelX = computeFilterInterpolateWithDistance(normalMap, filterSobelX);
        vec3 sobelY = computeFilterInterpolateWithDistance(normalMap, filterSobelY);
        convol += sqrt(sobelX.r * sobelX.r + sobelY.r * sobelY.r);
        convol += sqrt(sobelX.g * sobelX.g + sobelY.g * sobelY.g);
        convol += sqrt(sobelX.b * sobelX.b + sobelY.b * sobelY.b);
//        convol /= 3;
        // Returning to interval [0;1]
        convol = 0.5 * sqrt(2.0) * convol;
    }
    else if (filterType == 1)
    {
        convol = computeConvolution(normalMap, filterMiniPrewittX, filterMiniPrewittY, normalizeGradient == M_TRUE);
    }
    else if (filterType == 2)
    {
        convol = computeConvolution(normalMap, filterSobelX, filterSobelY, normalizeGradient == M_TRUE);
    }
    else if (filterType == 3)
    {
        convol = computeConvolution(normalMap, filterPrewittX, filterPrewittY, normalizeGradient == M_TRUE);
    }
    else if (filterType == 4)
    {
        float meanAngle = computeMeanAngle(normalMap, filterMeanAngle);
        convol = meanAngle / M_PI;
    }
    else if (filterType == 5)
    {
        vec3 convolX = computeFilterVec4(normalMap, filterSobelX);
        convolX = normalize(convolX);
        vec3 convolY = computeFilterVec4(normalMap, filterSobelY);
        convolY = normalize(convolY);
        vec3 pixelVal = texture(normalMap, uv).xyz;

        // Compute X and Y thetas between pixelVal
        float thetaX = acos(dot(pixelVal, convolX));
        float thetaY = acos(dot(pixelVal, convolY));

        //    float stepLength = radians(180.) / M_PI;
        thetaX = step(normalStep * M_PI, thetaX) * thetaX;
        thetaX = thetaX / M_PI;

        thetaY = step(normalStep * M_PI, thetaY) * thetaY;
        thetaY = thetaY / M_PI;

        convol = sqrt(thetaX * thetaX + thetaY * thetaY);
        convol = 0.5 * sqrt(2.) * convol;
    }

    convol = step(normalStep, convol) * (linearNormalStep == M_TRUE ? convol : 1.0);
    resultSobel += convol;

    // Compute Depth with Sobel (best way)
    float sobelDepth = computeConvolution(depthMap, filterSobelX, filterSobelY, normalizeGradient == M_TRUE);
    sobelDepth = step(depthStep, sobelDepth) * (linearDepthStep == M_TRUE ? sobelDepth : 1.0);
    resultSobel += sobelDepth;

    // Max value is 1.0
    resultSobel = min(1.0, resultSobel);

    // Return blend of normal and depth edge detection
    edgeMap = vec4(vec3(resultSobel), 1.0);
}
