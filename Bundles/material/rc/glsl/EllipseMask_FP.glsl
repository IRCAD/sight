#version 330

in vec2 uv;

uniform sampler2D u_fragData;
uniform sampler2D u_ellipseTex;
uniform float u_r;
uniform float u_g;
uniform float u_b;

out vec4 FragColor;

void main()
{
    vec4 frontColorBuffer = texture(u_fragData, uv);
    vec4 ellipse_sample = texture(u_ellipseTex,uv);

    if(ellipse_sample.r < 1.0)
    {
        FragColor = frontColorBuffer;
    }
    else
    {
        FragColor = mix(frontColorBuffer,ellipse_sample,0.5);

    }
}
