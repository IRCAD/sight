#version 330

out vec2 fragColour;

void main()
{
#if FRONT_FACES == 1

    fragColour = vec2(gl_FragCoord.z, 10000000.0);

#endif // FRONT_FACES

#if BACK_FACES_MAX == 1

    fragColour = vec2(100000000.0, -gl_FragCoord.z);

#endif // BACK_FACES

#if BACK_FACES == 1

    fragColour = vec2(gl_FragCoord.z, 10000000.0);

#endif // BACK_FACES

#if NEAR_PLANE == 1

    fragColour = vec2(0., 10000000.0);

#endif //  NEAR_PLANE
}
