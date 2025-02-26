#version 420

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

#include "TransferFunction.inc.glsl"

layout(binding=0) uniform sampler3D u_texture;
layout(binding=1) uniform sampler2D u_s2TFTexture;
uniform vec3 u_f3TFWindow;

uniform float u_slice;
uniform int u_orientation = 0;
uniform vec4 u_diffuse;
uniform int u_enableAlpha; //bool

in vec2 uv;

//-----------------------------------------------------------------------------
vec4 trilinear_filter(in vec3 _pos, in vec4 _values[8])
{
    const vec3 floored_pos = floor(_pos);
    const vec3 step        = _pos - floored_pos;

    return mix(mix(
                    mix(_values[0], _values[3], step.z), /* [0][0][0] | [0][0][1] */
                    mix(_values[2], _values[6], step.z), /* [0][1][0] | [0][1][1] */
                    step.y
                      ),

                mix(
                     mix(_values[1], _values[5], step.z), /* [1][0][0] | [1][0][1] */
                     mix(_values[4], _values[7], step.z), /* [1][1][0] | [1][1][1] */
                     step.y
                      ),
                step.x
                );
}

vec4 sample_negato()
{
    ivec3 texture_size = textureSize(u_texture, 0);
    const ivec3 hx = ivec3(1, 0, 0);
    const ivec3 hy = ivec3(0, 1, 0);
    const ivec3 hz = ivec3(0, 0, 1);

    const ivec3 positions [8] = ivec3[]
    (
        ivec3(0, 0, 0),
        hx,
        hy,
        hz,
        hx + hy,
        hx + hz,
        hy + hz,
        hx + hy + hz
    );

    vec4 neighbours [8] = vec4[]
    (
        vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0),
        vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)
    );

    vec3 tex_coord;
    if (u_orientation == 0) // Sagittal
    {
        tex_coord = vec3(u_slice, uv.y, uv.x) * texture_size;
    }
    else if (u_orientation == 1) // Frontal
    {
        tex_coord = vec3(uv.x, u_slice, uv.y) * texture_size;
    }
    else if (u_orientation == 2) // Axial
    {
        tex_coord = vec3(uv, u_slice) * texture_size;
    }
    for(int n = 0; n < 8; n++)
    {
        float value = texelFetch(u_texture, ivec3(tex_coord) + positions[n], 0).r;
        vec4 classified_sample = sampleTransferFunction(value, u_s2TFTexture, u_f3TFWindow);
        neighbours[n] = classified_sample;
    }

    vec4 windowedColor = trilinear_filter(tex_coord, neighbours);

    float tfAlpha = (1 - u_enableAlpha) + u_enableAlpha * windowedColor.a;
    float alpha   = tfAlpha * u_diffuse.a;

    return vec4( windowedColor.rgb, alpha );
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
