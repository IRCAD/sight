#version 330 core

uniform mat4 viewMatrix;

uniform vec3 u_f3LightPosition;
uniform vec3 u_f3LightIntensity;

uniform vec3 u_f3AmbientCol;
uniform vec3 u_f3DiffuseCol;
uniform vec3 u_f3SpecularCol;
uniform float u_fShininess;

// Lighting function.
// This shader is included in lighting techniques fragment shaders.

vec3 lighting(const in vec3 _f3FragPos, const in vec3 _f3FragNormal)
{
    // Calculates ambient component.
    float fAmbientCoeff = 5.0;
    vec3 f3Ambient = fAmbientCoeff * u_f3AmbientCol;

    // Calculates vector from the light to the fragment.
    vec3 f3LightToFragmentDirN = normalize(vec3(viewMatrix * vec4(u_f3LightPosition, 1.0)) - _f3FragPos);

    // Calculates vector from the fragment to the eye position.
    // (origin since this is in "eye" or "camera" space).
    vec3 f3FragToOriginDirN = normalize(-_f3FragPos);

    // Reflects light beam using the normal at this fragment.
    vec3 f3Reflection = reflect(-f3LightToFragmentDirN, _f3FragNormal);

    // Calculates diffuse component.
    float f3Diffuse = max(dot(f3LightToFragmentDirN, _f3FragNormal), 0.0);

    // Calculates specular component.
    float fSpecular = 0.0;
    if (dot(f3LightToFragmentDirN, _f3FragNormal) > 0.0)
        fSpecular = pow(max(dot(f3Reflection, f3FragToOriginDirN), 0.0), u_fShininess);

    // Combines ambient, diffuse and specular contributions.
    return u_f3LightIntensity * (u_f3DiffuseCol * f3Diffuse + u_f3SpecularCol * fSpecular + f3Ambient);
}