#version 330

out vec2 fragColour;

void main()
{
    fragColour = vec2(gl_FragCoord.z, - gl_FragCoord.z);
}
