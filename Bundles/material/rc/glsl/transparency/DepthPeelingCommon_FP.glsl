#version 150

vec3 packNormal(vec3 normal)
{
    return normal.xyz + 1.0 * 0.5;
}

vec3 unpackNormal(vec3 packedNormal)
{
    return normalize(packedNormal * 2. - 1.0);
}

float packColor(vec2 color)
{
    vec2 col = clamp(color, 0, 1);
    return floor(256. * color.r) + color.g * 0.1;
}

vec2 unpackColor(float value)
{
    vec2 color;
    color.r = floor(value);
    color.g = (value - color.r) * 10;
    color.r /= 256.f;
    return color;
}

vec4 packFloatToVec4(float value)
{
    // This is the classic way of packing but it doesn't work...
//  const vec4 bitSh = vec4(256.0*256.0*256.0, 256.0*256.0, 256.0, 1.0);
//  const vec4 bitMsk = vec4(0.0, 1.0/256.0, 1.0/256.0, 1.0/256.0);
//  vec4 res = fract(value * bitSh);
//  res -= res.xxyz * bitMsk;
//  return res;

    const float max24int = 256.0 * 256.0 * 256.0 - 1.0;
    value *= max24int;
    vec3 result = floor(value / vec3(256.0 * 256.0, 256.0, 1.0));
    result.gb -= result.rg * 256.0;
    result /= 255.0;
    return vec4(result,1);
}

float unpackFloatFromVec4(vec4 value)
{
    // This is the classic way of unpacking but it doesn't work...
//  const vec4 bitSh = vec4(1.0/(256.0*256.0*256.0), 1.0/(256.0*256.0), 1.0/256.0, 1.0);
//  return(dot(value, bitSh));

    const float max24int = 256.0 * 256.0 * 256.0 - 1.0;
    float result = 255.0 * dot(value.rgb, vec3(256.0 * 256.0, 256.0, 1.0)) / max24int;
    return result;
}


