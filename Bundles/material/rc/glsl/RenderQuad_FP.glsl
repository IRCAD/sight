#version 330

in vec2 uv;

uniform sampler2D u_fragData;

out vec4 FragColor;

void main()
{
    vec4 frontColorBuffer = texture(u_fragData, uv);
    FragColor = frontColorBuffer;
}
