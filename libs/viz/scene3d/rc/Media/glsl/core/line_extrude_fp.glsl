#version 420

#ifdef GLSL_LANG_VALIDATOR
#extension GL_GOOGLE_include_directive : enable
#endif // GLSL_LANG_VALIDATOR

// Input semantics
layout(location = 0) in float v_gradient;

uniform vec4 u_fog_center_radius;
uniform vec4 u_fog_color;

uniform vec4 u_diffuse;

uniform mat4 u_world_view;

#define HAS_CLIP_UNIFORM // Prevents double declaration with OIT
uniform float u_near;
uniform float u_far;

float linearize_depth(float depth, float near, float far)
{
    float z = depth * 2.0 - 1.0;
    return -(2.0 * near * far) / (far + near - z * (far - near));
}

// Compute illumination or forward illumination received from the vertex shader
vec4 getFragmentColor()
{
    float z_view = linearize_depth(gl_FragCoord.z, u_near, u_far);

    vec4 center = u_world_view * vec4(u_fog_center_radius.xyz, 1.0);
    float fogFactor = smoothstep(center.z - u_fog_center_radius.w, center.z + u_fog_center_radius.w, z_view);

    vec4 color = mix(u_fog_color, u_diffuse, fogFactor);
    color.a = color.a * (1.-(abs(v_gradient)));
    return color;
}

// Compute alpha channel
float getFragmentAlpha()
{
    float z_view = linearize_depth(gl_FragCoord.z, u_near, u_far);

    vec4 center = u_world_view * vec4(u_fog_center_radius.xyz, 1.0);
    float fogFactor = smoothstep(center.z - u_fog_center_radius.w, center.z + u_fog_center_radius.w, z_view);

    float color = mix(u_fog_color.a, u_diffuse.a, fogFactor) * (1.-(abs(v_gradient)));
    return color;
}

#include "Transparency.inc.glsl"

void main(void)
{
    processFragment();
}
