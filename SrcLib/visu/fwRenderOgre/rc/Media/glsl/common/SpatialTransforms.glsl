#version 150

uniform vec4 u_viewportSize;

uniform float u_clippingNearDis;
uniform float u_clippingFarDis;

/// Converts OpenGL fragment coordinates to normalized device coordinates (NDC).
vec3 fragCoordsToNDC(in vec3 _f3FragPos_Ss)
{
    vec3 f3FragPos_Ns  = vec3(_f3FragPos_Ss.xy * u_viewportSize.zw, _f3FragPos_Ss.z);
    f3FragPos_Ns = (f3FragPos_Ns - 0.5) * 2.0;
    return f3FragPos_Ns;
}

//-----------------------------------------------------------------------------

/// Converts OpenGL normalized device coordinates (NDC) to  fragment coordinates.
vec3 ndcToFragCoord(in vec3 _f3FragPos_Ns)
{
    vec3 f3FragPos_Ss  = (_f3FragPos_Ns + 1.0) * 0.5;
    return vec3(f3FragPos_Ss.xy * u_viewportSize.xy, f3FragPos_Ss.z);
}

//-----------------------------------------------------------------------------

/// Converts a position in OpenGL's normalized device coordinates (NDC) to the specified space.
vec4 ndcToSpecificSpacePosition(in vec3 _f3FragPos_Ns, in mat4 _m4Inverse)
{
    vec4 clipPos;
    clipPos.w   = (2.0 * u_clippingNearDis * u_clippingFarDis)  / (u_clippingNearDis + u_clippingFarDis + _f3FragPos_Ns.z * (u_clippingNearDis - u_clippingFarDis));
    clipPos.xyz = _f3FragPos_Ns * clipPos.w;

    return _m4Inverse * clipPos;
}
