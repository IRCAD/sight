#version 330

uniform vec3 u_cameraPos;
uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_specular;
uniform float u_shininess;

uniform vec4 u_lightAmbient;

uniform int u_numLights;

uniform vec3 u_lightDir[MAX_LIGHTS];
uniform vec3 u_lightDiffuse[MAX_LIGHTS];
uniform vec3 u_lightSpecular[MAX_LIGHTS];

vec4 lighting(vec3 _normal, vec3 _position)
{
    vec3 vecToCam = normalize(u_cameraPos - _position);

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for(int i = 0; i < u_numLights; ++i)
    {
        float fLitDiffuse = clamp(dot( normalize(-u_lightDir[i]), _normal ), 0, 1);
        diffuse += fLitDiffuse * u_lightDiffuse[i] * u_diffuse.rgb;

        vec3 r = reflect(u_lightDir[i], _normal);
        float fLitSpecular = pow( clamp(dot( r, vecToCam ), 0, 1), u_shininess);
        specular += fLitSpecular * u_lightSpecular[i] * u_specular.rgb;
    }

    return vec4((u_lightAmbient.rgb * u_ambient.rgb) + diffuse + specular, u_diffuse.a);
}
