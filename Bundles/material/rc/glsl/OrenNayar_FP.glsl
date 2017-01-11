#version 410

// Uniforms
uniform vec4 u_diffuse;
uniform vec3 u_cameraPos;
uniform vec3 u_lightDir;

uniform int u_roughness;

// Input semantics
layout(location = 0) in vec3 inNormal_WS;
layout(location = 1) in vec3 inPosition_WS;

// Compute illumination or forward illumination received from the vertex shader
vec4 getFragmentColor()
{
    // Input vectors
    vec3 normal = normalize(inNormal_WS);
    vec3 light = normalize(u_lightDir);
    vec3 view = normalize(u_cameraPos - inPosition_WS);

    // Roughness, A and B
    float roughness = u_roughness/100.;
    float roughness2 = roughness * roughness;
    vec2 oren_nayar_fraction = roughness2 / (roughness2 + vec2(0.33, 0.09));
    vec2 oren_nayar = vec2(1, 0) + vec2(-0.5, 0.45) * oren_nayar_fraction;

    // Theta and phi
    vec2 cos_theta = clamp(vec2(abs(dot(normal, light)), dot(normal, view)), 0, 1);
    vec2 cos_theta2 = cos_theta * cos_theta;
    float sin_theta = sqrt((1-cos_theta2.x) * (1-cos_theta2.y));
    vec3 light_plane = normalize(light - cos_theta.x * normal);
    vec3 view_plane = normalize(view - cos_theta.y * normal);
    float cos_phi = clamp(dot(light_plane, view_plane), 0, 1);

    // Composition
    float diffuse_oren_nayar = cos_phi * sin_theta / max(cos_theta.x, cos_theta.y);
    float diffuse = cos_theta.x * (oren_nayar.x + oren_nayar.y * diffuse_oren_nayar);

    return vec4(diffuse * u_diffuse.rgb, u_diffuse.a);
}
