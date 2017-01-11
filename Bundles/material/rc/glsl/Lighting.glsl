#version 330

uniform vec3 u_cameraPos;
uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_specular;
uniform float u_shininess;
uniform vec3 u_lightDir[NUM_LIGHTS];

vec4 lighting(vec3 _normal, vec3 _position)
{
    vec3 vecToCam = normalize(_position - u_cameraPos);

    float fLitDiffuse = 0;
    float fLitSpecular = 0;

    for(int i = 0; i < NUM_LIGHTS; ++i)
    {
        fLitDiffuse += abs(dot( _normal, normalize(u_lightDir[i]) ));

        vec3 r = reflect(normalize(u_lightDir[i]), _normal);
        fLitSpecular += pow( clamp(dot( vecToCam, r ), 0, 1), u_shininess);
    }

    return vec4(u_ambient.rgb + u_diffuse.rgb * fLitDiffuse + u_specular.rgb * fLitSpecular, u_diffuse.a);
}
