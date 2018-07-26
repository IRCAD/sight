#version 330

// The blend function is set to "minimum"
// We try to find the entry depth in x and exit depth in y
#if FRONT_FACES == 1 || BACK_FACES == 1
out float rayEntry; // Use only the red channel when writing entry points.
#else
out vec2 fragColour;
#endif // FRONT_FACES == 1 || BACK_FACES == 1

#define FLT_MAX 1e9f

void main()
{

#if BACK_FACES == 1

    // FIRST STEP - Back Faces : find the nearest back face depth.
    // Don't do anything, we're not writing any colours anyway we just want the depth.

#endif BACK_FACES == 1

#if FRONT_FACES == 1

    // SECOND STEP - Front Faces: update the entry points depths with the closest front faces occluding closest back faces
    rayEntry = gl_FragCoord.z;

#endif // FRONT_FACES

#if BACK_FACES_MAX == 1

    // THIRD STEP - Back Faces max: find the exit points of the geometry.
    fragColour = vec2(FLT_MAX, -gl_FragCoord.z);

#endif // BACK_FACES_MAX

#if FRONT_FACES_MIN == 1

    // FOURTH STEP - Front faces min : find the exit points when a ray hits a surface.
    fragColour = vec2(-FLT_MAX, -gl_FragCoord.z);

#endif // FRONT_FACES_MIN

}
