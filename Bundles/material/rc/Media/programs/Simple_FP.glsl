// requires OpenGL >= 3.2
#version 150

uniform float defaultParam;

out vec4 fragColor;

void main(void)
{
  fragColor = vec4(defaultParam, 1.0, 0.0, 1.0);
}
