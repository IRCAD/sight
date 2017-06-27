#version 330

// The blend function is set to "minimum"
// We try to find the entry depth in x and exit depth in y
out vec2 fragColour;

#define FLT_MAX 1e9f

void main()
{
#if BACK_FACES_MAX == 1

    // FIRST STEP - Back Faces max: find the exit points of the geometry.
    fragColour = vec2(FLT_MAX, -gl_FragCoord.z);

#endif // BACK_FACES

#if BACK_FACES == 1

    // SECOND STEP - Back Faces: find the closest back faces and initialize the entry points with their depths.
    fragColour = vec2(gl_FragCoord.z, FLT_MAX);

#endif // BACK_FACES

#if FRONT_FACES == 1

    // THIRD STEP - Front Faces: update the entry points depths with the closest front faces occluding closest back faces
    fragColour = vec2(gl_FragCoord.z, FLT_MAX);

#endif // FRONT_FACES

#if NEAR_PLANE == 1

    // FOURTH STEP - Near Plane: update the entry points depths with the near plane depth when when it is inside a brick
    fragColour = vec2(0., FLT_MAX);

#endif //  NEAR_PLANE
}
