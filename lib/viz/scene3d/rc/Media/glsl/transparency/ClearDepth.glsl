#version 330

out vec4 FragColor;

void main()
{
    // Write far depth (1.0) directly to R channel
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
