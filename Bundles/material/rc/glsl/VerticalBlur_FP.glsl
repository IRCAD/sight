#version 330

// Note, this won't work on ATI which is why it's not used
// the issue is with the array initializers
// no card supports GL_3DL_array_objects but it does work on nvidia, not on ATI
//#extension GL_3DL_array_objects : enable

in vec2 texCoord;

out vec4 fragColour;

uniform sampler2D Blur0;
uniform float u_blurWeight;

//----------------------------------------------

vec2 pos[11]  = vec2[11](
    vec2(0., -5.),
    vec2(0., -4.),
    vec2(0., -3.),
    vec2(0., -2.),
    vec2(0., -1.),
    vec2(0.,  0.),
    vec2(0.,  1.),
    vec2(0.,  2.),
    vec2(0.,  3.),
    vec2(0.,  4.),
    vec2(0.,  5.)
);

// We use the Normal-gauss distribution formula
// f(x) being the formula, we used f(0.5)-f(-0.5); f(1.5)-f(0.5)...
float samples[11] = float[11] 
(//stddev=2.0
    0.01222447,
    0.02783468,
    0.06559061,
    0.12097757,
    0.17466632,

    0.19741265,

    0.17466632,
    0.12097757,
    0.06559061,
    0.02783468,
    0.01222447
);

//----------------------------------------------

void main()
{
    vec4 sum = vec4(0.);

    for(int i = 0; i < 11; ++i)
    {
        sum += texture(Blur0, texCoord + pos[i] * u_blurWeight) * samples[i];
    }

    fragColour = sum;
}
