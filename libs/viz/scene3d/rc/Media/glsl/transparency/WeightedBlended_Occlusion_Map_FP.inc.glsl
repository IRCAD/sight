out vec4 FragColor;

void process()
{
    if(getFragmentAlpha() != 1.)
    {
        discard;
    }

    FragColor.r = gl_FragCoord.z;
}
