out vec4 FragColor;

void process()
{
    if(getFragmentAlpha() < .999)
    {
        discard;
    }

    FragColor.r = gl_FragCoord.z;
}
