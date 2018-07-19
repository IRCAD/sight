#version 330

uniform sampler3D u_texture;

uniform float u_slice;
uniform int u_threshold = 0;
uniform int u_orientation = 0;
uniform vec4 u_diffuse;
uniform int u_enableAlpha; //bool

in vec2 uv;

//-----------------------------------------------------------------------------
vec4 sampleTransferFunction(float intensity);

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

    vec4 windowedColor = sampleTransferFunction(value);

    float alpha = (1 - u_enableAlpha) * u_diffuse.a + u_enableAlpha * windowedColor.a;

    return vec4( windowedColor.rgb, alpha );
}

// Compute alpha channel
float getFragmentAlpha()
{
    return u_diffuse.a;
}
