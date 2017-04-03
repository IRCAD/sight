#version 330

#if BACK_FACES == 1
uniform sampler2D u_frontFaces;
uniform float u_vpWidth;
uniform float u_vpHeight;
#endif // BACK_FACE

in vec3 localPosition;

out vec4 fragColour;

void main()
{
#if FRONT_FACES == 1

    fragColour = vec4(localPosition, 1.);

#endif // FRONT_FACES

#if BACK_FACES == 1

    vec2 texCoord = gl_FragCoord.xy / vec2( u_vpWidth, u_vpHeight );
    vec3 frontFacesSample = texture(u_frontFaces, texCoord).rgb;

    vec3 ray = localPosition - frontFacesSample;

    vec3 rayDir     = normalize(ray);
    float rayLength = length(ray);

    fragColour = vec4(rayDir, rayLength);

#endif // BACK_FACES
}
