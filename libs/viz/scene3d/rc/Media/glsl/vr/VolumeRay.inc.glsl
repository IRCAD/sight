#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "VolumeRay.glsl.struct"

#define MAX_STEPS 8192

//-----------------------------------------------------------------------------

void vrayAdvance(inout VolumeRay _vray)
{
    _vray.position    += _vray.direction;
    _vray.totalLength -= _vray.stepLength;
    _vray.numSteps    += 1;
}

//-----------------------------------------------------------------------------

bool vrayTerminated(in VolumeRay _vray)
{
    return _vray.totalLength <= 0. || _vray.numSteps > MAX_STEPS;
}