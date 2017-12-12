#version 330

out vec4 FragColor;

float getFragmentAlpha();

void main()
{
    if(getFragmentAlpha() != 1.)
    {
        discard;
    }

    FragColor.r = gl_FragCoord.z;
}
