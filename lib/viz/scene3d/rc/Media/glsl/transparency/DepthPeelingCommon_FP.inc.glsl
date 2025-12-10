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
