#version 330

uniform sampler3D u_texture;
uniform sampler2D u_tfTexture;

uniform float u_slice;
uniform int u_threshold = 0;
uniform int u_orientation = 0;
uniform vec4 u_diffuse;

in vec2 uv;

vec4 windowLevel(vec4 value)
{
    float scaledValue = value.r * 65535.f;

    // Computes 2D indices from the hounsfield value
    int j = int( scaledValue / 256 );
    int i = int( mod( int(scaledValue), 256 ) );

    // Converts the indices into texture uv coordinates
    vec2 uvTF = vec2(i / 255.f, j / 255.f);

    return texture(u_tfTexture, uvTF);
}

vec4 getFragmentColor()
{
    vec4 color;
    if (u_orientation == 0) // Sagittal
    {
        color = texture(u_texture, vec3(u_slice, uv.y, uv.x));
    }
    else if (u_orientation == 1) // Frontal
    {
        color = texture(u_texture, vec3(uv.x, u_slice, uv.y));
    }
    else if (u_orientation == 2) // Axial
    {
        color = texture(u_texture, vec3(uv, u_slice));
    }

    vec4 windowedColor = windowLevel(color);

    return vec4( windowedColor.rgb, /*windowedColor.a **/ u_diffuse.a );
}
