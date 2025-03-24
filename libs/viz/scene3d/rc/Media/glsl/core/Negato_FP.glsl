#version 420

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "TransferFunction.inc.glsl"

layout(binding=0) uniform sampler3D u_texture;
layout(binding=1) uniform sampler2D u_s2TFTexture;

#ifdef MASK
layout(binding=2) uniform sampler3D u_mask;
#endif // MASK

uniform vec3 u_f3TFWindow;

uniform float u_slice;
uniform int u_orientation = 0;
uniform vec4 u_diffuse;
uniform int u_enableAlpha; //bool

in vec2 uv;

//-----------------------------------------------------------------------------

vec4 sample_negato()
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

    vec4 windowedColor = sampleTransferFunction(value, u_s2TFTexture, u_f3TFWindow);

    float tfAlpha = (1 - u_enableAlpha) + u_enableAlpha * windowedColor.a;
    float alpha   = tfAlpha * u_diffuse.a;

    vec4 negato_color = vec4( windowedColor.rgb, alpha );

#ifdef MASK
    float mask;
    if (u_orientation == 0) // Sagittal
    {
        mask = texture(u_mask, vec3(u_slice, uv.y, uv.x)).r;
    }
    else if (u_orientation == 1) // Frontal
    {
        mask = texture(u_mask, vec3(uv.x, u_slice, uv.y)).r;
    }
    else if (u_orientation == 2) // Axial
    {
        mask = texture(u_mask, vec3(uv, u_slice)).r;
    }
    // The threshold values correspond to the values "off", "neighborhood" and "on" we use when creating masks
    negato_color.a = negato_color.a * (smoothstep(0.2, 0.4, mask) * 0.8 + smoothstep(0.4, 0.8, mask));
#endif // MASK

    return negato_color;
}

//-----------------------------------------------------------------------------

vec4 getFragmentColor()
{
    return sample_negato();
}

//-----------------------------------------------------------------------------

float getFragmentAlpha()
{
    return sample_negato().a;
}

//-----------------------------------------------------------------------------

#include "Transparency.inc.glsl"

void main(void)
{
    processFragment();
}
