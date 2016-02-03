#version 330

in vec2 uv;
uniform sampler2D u_bufferColor;
uniform sampler2D u_bufferDepth;
uniform sampler2D u_bufferNormal;
uniform vec4 u_invTexSize;
uniform float u_near;
uniform float u_far;
out vec4 FragColor;
ivec2 offs[9] = ivec2[](
    ivec2(-1, -1), //Top Left     0
    ivec2( 0, -1), //Top          1
    ivec2( 1, -1), //Top Right    2
    ivec2(-1,  0), //Left         3
    ivec2( 0,  0), //Center       4
    ivec2( 0,  1), //Right        5
    ivec2(-1,  1), //Bottom Left  6
    ivec2( 0,  1), //Bottom       7
    ivec2( 1,  1)  //Bottom Right 8
);


//---------------------------------------------------------------------------------------------------------------------

float linearizeDepth(in float depth) {
    return (2.0*u_near) / (u_far+u_near - depth *(u_far-u_near));
}

//---------------------------------------------------------------------------------------------------------------------

vec3 unpackNormal(vec3 packedNormal);
float unpackFloatFromVec4(vec4 value);

//---------------------------------------------------------------------------------------------------------------------

vec3 computeFilterInterpolateWithDistance(sampler2D map, float[9] filters)
{
    vec3 g = vec3(0.0);

    float lowPercent = .3;
    float highPercent = .8;

    float depthRange = u_far - u_near;
    float lowPass = u_near + depthRange * lowPercent;
    float highCut = u_near + depthRange * highPercent;

    float depth = unpackFloatFromVec4(texture(u_bufferDepth, uv));
    float centerDepth = linearizeDepth(depth);

    vec3 centerPixel = unpackNormal(texture(map, uv).xyz);
    vec3 currentPixel, interpolatedPixel;

    // Compute linear function for interpolation coefficient
    float a,b;
    a = - (1 / (highPercent - lowPercent));
    b = 1 - a * lowPercent;
    float interpolation;

    // TODO : Find best interpolation function (linear or not)

    for(int i = 0; i < 9; i++)
    {
        currentPixel = unpackNormal(texture(map, uv + (offs[i] * u_invTexSize.rg) ).xyz);

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
        g += interpolatedPixel * filters[i];
    }
    return g;
}

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

//---------------------------------------------------------------------------------------------------------------------

float computeFilter(sampler2D map, float[9] filters)
{
    // Because filters change intervals, we must compute sum of its coefficients
    float normalize = 0;
    /*vec3 g = vec3(0.0);
    for(int i = 0; i < 9; i++)
    {
        // Getting map values in [-1;1]
        g += (textureOffset(map, uv, offs[i]) * 2.0 - 1.0).xyz * filters[i];
        normalize += abs(filters[i]);
    }
    return length(g) / normalize;*/
    float g = 0.;
    for(int i = 0; i < 9; i++)
    {
        // Getting map values in [-1;1]
        g += linearizeDepth(texture(map, uv + (offs[i] * u_invTexSize.rg) ).r) * filters[i];
    }
    return g;
}

//---------------------------------------------------------------------------------------------------------------------

float computeConvolution(sampler2D map, float[9] filterX, float[9] filterY)
{
    // Compute horizontal Gx
    float vx = computeFilter(map, filterX);

    // Compute horizontal Gx
    float vy = computeFilter(map, filterY);

    // [-1;1] => [0; sqrt(2)]
    float result = sqrt(vx * vx + vy * vy);

    // Getting to the [0,1] interval
    return 0.5 * sqrt(2.0) * result;
}

//---------------------------------------------------------------------------------------------------------------------

float computeSobel() {

    float resultSobel = 0.0;
    float convol;

    // TODO : fix wrong interpolation and fix getting triangles for edges
    vec3 sobelX = computeFilterInterpolateWithDistance(u_bufferNormal, filterSobelX);
    vec3 sobelY = computeFilterInterpolateWithDistance(u_bufferNormal, filterSobelY);
    convol += sqrt(sobelX.r * sobelX.r + sobelY.r * sobelY.r);
    convol += sqrt(sobelX.g * sobelX.g + sobelY.g * sobelY.g);
    convol += sqrt(sobelX.b * sobelX.b + sobelY.b * sobelY.b);
    // Returning to interval [0;1]
    convol = 0.5 * sqrt(2.0) * convol;

    float normalStep = 0.3;
    convol = step(normalStep, convol) * convol;
    resultSobel += convol;

    // Compute Depth with Sobel (best way)
    float sobelDepth = computeConvolution(u_bufferDepth, filterSobelX, filterSobelY);
    float depthStep = 0.05;
    sobelDepth = step(depthStep, sobelDepth);
    resultSobel += sobelDepth;

    // Max value is 1.0
    resultSobel = min(1.0, resultSobel);

    return 1.-resultSobel;
}

/** Blend shader :
  * Blend front passes
  **/
void main()
{
    float sobel = 1.;
    float frontDepthBuffer = unpackFloatFromVec4(texture(u_bufferDepth, uv));
    if( frontDepthBuffer != 1. )
    {
        sobel = computeSobel();
    }
    vec4 colorOut = texture(u_bufferColor, uv);
    FragColor = vec4(colorOut.rgb*sobel, colorOut.a);
}
