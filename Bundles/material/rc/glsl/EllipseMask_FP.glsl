#version 330

in vec2 uv;

uniform sampler2D u_fragData;
uniform sampler2D u_ellipseTex;

uniform float u_x;
uniform float u_y;
uniform float u_a;
uniform float u_b;
uniform float u_theta;

out vec4 FragColor;

vec2 ellipseCoord()
{
    float cos_theta = cos(u_theta);
    float sin_theta = sin(u_theta);

    vec2 result = uv;
    vec2 origin = vec2(u_x, u_y);

    result = (result - origin) * mat2(cos_theta, sin_theta, -sin_theta, cos_theta);
    result += origin;

    result -= origin;
    result.x =  result.x / u_a;
    result.y =  result.y / u_b;
    result += origin;

    return result;
}

void main()
{
    vec4 frontColorBuffer = texture(u_fragData, uv);

    vec4 ellipse_sample = texture(u_ellipseTex, ellipseCoord());

    if(ellipse_sample.r < 1.0)
    {
        FragColor = frontColorBuffer;
    }
    else
    {
        FragColor = mix(frontColorBuffer, ellipse_sample, 0.5);
    }
}