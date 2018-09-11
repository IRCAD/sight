#version 330

vec3 hue2rgb(float H)
{
    float R = abs(H * 6. - 3.) - 1.;
    float G = 2. - abs(H * 6. - 2.);
    float B = 2. - abs(H * 6. - 4.);

    return clamp(vec3(R, G, B), 0., 1.);
}

//-----------------------------------------------------------------------------

float Epsilon = 1e-10;

vec3 rgb2hcv(vec3 RGB)
{
    // Based on work by Sam Hocevar and Emil Persson
    vec4 P = (RGB.g < RGB.b) ? vec4(RGB.bg, -1., 2. / 3.) : vec4(RGB.gb, 0., -1. / 3.);
    vec4 Q = (RGB.r < P.x) ? vec4(P.xyw, RGB.r) : vec4(RGB.r, P.yzx);
    float C = Q.x - min(Q.w, Q.y);
    float H = abs((Q.w - Q.y) / (6. * C + Epsilon) + Q.z);

    return vec3(H, C, Q.x);
}

//-----------------------------------------------------------------------------

vec3 hsv2rgb(vec3 HSV)
{
    vec3 RGB = hue2rgb(HSV.x);
    return ((RGB - 1.) * HSV.y + 1.) * HSV.z;
}

//-----------------------------------------------------------------------------

vec3 hsl2rgb(vec3 HSL)
{
    vec3 RGB = hue2rgb(HSL.x);
    float C = (1. - abs(2. * HSL.z - 1.)) * HSL.y;

    return (RGB - 0.5) * C + HSL.z;
}

//-----------------------------------------------------------------------------

// The weights of RGB contributions to luminance.
// Should sum to unity.
vec3 HCYwts = vec3(0.299, 0.587, 0.114);

vec3 hcy2rgb(vec3 HCY)
{
    vec3 RGB = hue2rgb(HCY.x);
    float Z = dot(RGB, HCYwts);

    if(HCY.z < Z)
    {
        HCY.y *= HCY.z / Z;
    }
    else if(Z < 1.)
    {
        HCY.y *= (1. - HCY.z) / (1. - Z);
    }

    return (RGB - Z) * HCY.y + HCY.z;
}

//-----------------------------------------------------------------------------

float HCLgamma = 3;
float HCLy0 = 100;
float HCLmaxL = 0.530454533953517; // == exp(HCLgamma / HCLy0) - 0.5
float PI = 3.1415926536;

vec3 hcl2rgb(vec3 HCL)
{
    vec3 RGB = vec3(0.);

    if (HCL.z != 0.)
    {
        float H = HCL.x;
        float C = HCL.y;
        float L = HCL.z * HCLmaxL;
        float Q = exp((1. - C / (2. * L)) * (HCLgamma / HCLy0));
        float U = (2. * L - C) / (2. * Q - 1.);
        float V = C / Q;
        float T = tan((H + min(fract(2 * H) / 4, fract(-2 * H) / 8)) * PI * 2);
        H *= 6.;

        if(H <= 1.)
        {
            RGB.r = 1.;
            RGB.g = T / (1. + T);
        }
        else if(H <= 2.)
        {
            RGB.r = (1. + T) / T;
            RGB.g = 1.;
        }
        else if(H <= 3.)
        {
            RGB.g = 1.;
            RGB.b = 1. + T;
        }
        else if(H <= 4.)
        {
            RGB.g = 1. / (1. + T);
            RGB.b = 1.;
        }
        else if(H <= 5.)
        {
            RGB.r = -1. / T;
            RGB.b = 1.;
        }
        else
        {
            RGB.r = 1.;
            RGB.b = -T;
        }

        RGB = RGB * V + U;
    }

    return RGB;
}

//-----------------------------------------------------------------------------

vec3 rgb2hsv(vec3 RGB)
{
    vec3 HCV = rgb2hcv(RGB);
    float S = HCV.y / (HCV.z + Epsilon);

    return vec3(HCV.x, S, HCV.z);
}

//-----------------------------------------------------------------------------

vec3 rgb2hsl(vec3 RGB)
{
    vec3 HCV = rgb2hcv(RGB);
    float L = HCV.z - HCV.y * 0.5;
    float S = HCV.y / (1. - abs(L * 2. - 1.) + Epsilon);

    return vec3(HCV.x, S, L);
}

//-----------------------------------------------------------------------------

vec3 rgb2hcy(vec3 RGB)
{
    // Corrected by David Schaeffer
    vec3 HCV = rgb2hcv(RGB);
    float Y = dot(RGB, HCYwts);
    float Z = dot(hue2rgb(HCV.x), HCYwts);

    if(Y < Z)
    {
        HCV.y *= Z / (Epsilon + Y);
    }
    else
    {
        HCV.y *= (1. - Z) / (Epsilon + 1. - Y);
    }

    return vec3(HCV.x, HCV.y, Y);
}

//-----------------------------------------------------------------------------

vec3 rgb2hcl(vec3 RGB)
{
    vec3 HCL;
    float H = 0.;
    float U = min(RGB.r, min(RGB.g, RGB.b));
    float V = max(RGB.r, max(RGB.g, RGB.b));
    float Q = HCLgamma / HCLy0;
    HCL.y = V - U;

    if(HCL.y != 0.)
    {
        H = atan(RGB.r - RGB.g, RGB.g - RGB.b) / PI;
        Q *= U / V;
    }

    Q = exp(Q);
    HCL.x = fract(H / 2. - min(fract(H), fract(-H)) / 6.);
    HCL.y *= Q;
    HCL.z = mix(-U, V, Q) / (HCLmaxL * 2.);

    return HCL;
}