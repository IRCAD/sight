#version 330 core

// Lighting functions
// This shader is included in lighting techniques fragment shaders.
// Uniforms are defined in those files.

vec3 lighting(const in vec3 pos, const in vec3 n)
{
    // Calculate the ambient component
    float ambientCoeff = 5.0;
    vec3 ambient = ambientCoeff * u_f3AmbientCol;

    // Calculate the vector from the light to the fragment
    vec3 s = normalize(vec3(viewMatrix * vec4(u_f3LightPosition, 1.0)) - pos);

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    vec3 v = normalize(-pos);

    // Reflect the light beam using the normal at this fragment
    vec3 r = reflect(-s, n);

    // Calculate the diffuse component
    float diffuse = max(dot(s, n), 0.0);

    // Calculate the specular component
    float specular = 0.0;
    if (dot(s, n) > 0.0)
        specular = pow(max(dot(r, v), 0.0), u_fShininess);

    // Combine the ambient, diffuse and specular contributions
    return u_f3LightIntensity * (u_f3DiffuseCol * diffuse + u_f3SpecularCol * specular + ambient);
}