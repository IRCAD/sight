#version 330

uniform vec3 u_cameraPos;
uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_specular;
uniform float u_shininess;

uniform vec3 u_lightDir[NUM_LIGHTS];
uniform vec3 u_lightDiffuse[NUM_LIGHTS];
uniform vec3 u_lightSpecular[NUM_LIGHTS];

vec4 lighting(vec3 _normal, vec3 _position)
{
    vec3 vecToCam = normalize(_position - u_cameraPos);

    float fLitDiffuse = 0;
    vec3 diffuse = vec3(0.0);

    float fLitSpecular = 0;
    vec3 specular = vec3(0.0);

    for(int i = 0; i < NUM_LIGHTS; ++i)
    {
        fLitDiffuse += abs(dot( _normal, normalize(u_lightDir[i]) ));
        diffuse += fLitDiffuse * u_lightDiffuse[i];

        vec3 r = reflect(normalize(u_lightDir[i]), _normal);
        fLitSpecular += pow( clamp(dot( vecToCam, r ), 0, 1), u_shininess);
        specular += fLitSpecular * u_lightSpecular[i];
    }

    return vec4(u_ambient.rgb + diffuse + specular, u_diffuse.a);
}
