#version 150

float packNormal(vec3 normal)
{
    return clamp(int(256. * normal.r), 0, 256)/256.
            + clamp(int(256. * normal.g), 0, 256)/65536.
            + clamp(int(256. * normal.b), 0, 256)/16777216.;
}

vec3 unpackNormal(float buffer)
{
    vec3 normal;
    normal.r = clamp(int(256. * buffer), 0, 256)/256.;
    normal.g = clamp(int(65536. * (buffer-normal.r)), 0, 256)/256.;
    normal.b = clamp(int(16777216. * (buffer-normal.r-normal.g/256.)), 0, 256)/256.;
    return normal;
}

float packColor(vec2 color)
{
    vec2 col = clamp(color, 0, 1);
    return floor(256. * color.r) + color.g * 0.1;
}

vec2 unpackColor(float buffer)
{
    vec2 color;
    color.r = floor(buffer);
    color.g = (buffer - color.r) * 10;
    color.r /= 256.f;
    return color;
}
