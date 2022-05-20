#version 420

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "TransferFunction.inc.glsl"

layout(binding=0) uniform sampler3D u_texture;
layout(binding=1) uniform sampler1D u_s1TFTexture;
uniform vec2 u_f2TFWindow;

uniform float u_slice;
uniform int u_orientation = 0;
uniform vec4 u_diffuse;
uniform int u_enableAlpha; //bool

in vec2 uv;

//-----------------------------------------------------------------------------

vec4 getFragmentColor()
{
    float value;
    if (u_orientation == 0) // Sagittal
    {
        value = texture(u_texture, vec3(u_slice, uv.y, uv.x)).r;
    }
    else if (u_orientation == 1) // Frontal
    {
        value = texture(u_texture, vec3(uv.x, u_slice, uv.y)).r;
    }
    else if (u_orientation == 2) // Axial
    {
        value = texture(u_texture, vec3(uv, u_slice)).r;
    }

    vec4 windowedColor = sampleTransferFunction(value, u_s1TFTexture, u_f2TFWindow);

    float tfAlpha = (1 - u_enableAlpha) + u_enableAlpha * windowedColor.a;
    float alpha   = tfAlpha * u_diffuse.a;

    return vec4( windowedColor.rgb, alpha );
}

//-----------------------------------------------------------------------------

// Compute alpha channel
float getFragmentAlpha()
{
    return u_diffuse.a;
}

#include "Transparency.inc.glsl"

void main(void)
{
    processFragment();
}
