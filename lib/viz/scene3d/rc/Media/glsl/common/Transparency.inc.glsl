
#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#if defined(RAY_EXIT_POINTS)
out vec2 fragColour;
#elif defined(DEPTH_PEELING)
#include "DepthPeelingPeel_FP.inc.glsl"
#elif defined(DUAL_DEPTH_PEELING)
#include "DualDepthPeelingPeel_FP.inc.glsl"
#elif defined(CELL_SHADING_DEPTH_PEELING)
#include "CelShadingDepthPeelingPeel_FP.inc.glsl"
#elif defined(WBOIT)
#include "WeightedBlended_Weight_Blend_FP.inc.glsl"
#elif defined(WBOIT_TRANSMIT)
#include "WeightedBlended_Transmittance_Blend_FP.inc.glsl"
#elif defined(WBOIT_OCCLUSION)
#include "WeightedBlended_Occlusion_Map_FP.inc.glsl"
#else
out vec4 fragColor;
#endif

// Main function, which assumes getFragmentColor() and getFragmentAlpha() is previously defined
void processFragment(void)
{
#if defined(RAY_EXIT_POINTS)
    // FOURTH STEP - Front faces min : find the exit points when a ray hits a surface.
    // The red channel is disabled, writes to the green channel only.
    float alpha = getFragmentAlpha();
    if(alpha < 0.01f)
        discard;
    fragColour.g = -gl_FragCoord.z;
#elif defined(DEPTH_PEELING) || defined(DUAL_DEPTH_PEELING) || defined(CELL_SHADING_DEPTH_PEELING) || defined(WBOIT) || defined(WBOIT_TRANSMIT) || defined(WBOIT_OCCLUSION)
    process();
#else
    fragColor = getFragmentColor();
#endif
}
