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
    vec2 origin = vec2(u_x,u_y);

    //translation
    result += origin;

    //rotation
    result -= 0.5f;
    result = result * mat2(cos_theta, -sin_theta, sin_theta, cos_theta);

    //Scale in percentage
    result =  result / vec2( u_a /100, u_b/100);

    result += 0.5f;

    return result;
}

void main()
{
    vec4 frontColorBuffer = texture(u_fragData, uv);
    vec4 ellipse_sample = texture(u_ellipseTex, ellipseCoord());

#ifdef BACKGROUND
    if(ellipse_sample.r == 1.0)
    {
        FragColor = frontColorBuffer;
    }
    else
    {
        FragColor = vec4(frontColorBuffer.rgb, ellipse_sample.r);
    }

#else
    if(ellipse_sample.r < 1.0)
    {
        float alpha = 1.0 - ellipse_sample.r;
        FragColor = vec4(frontColorBuffer.rgb,alpha);

    }
#endif
}
