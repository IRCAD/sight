#version 330
#define MAX_LIGHTS 10

uniform vec3 u_f3CameraPos;
uniform vec4 u_f4AmbientCol;
uniform vec4 u_f4DiffuseCol;
uniform vec4 u_f4SpecularCol;
uniform float u_fShininess;

uniform float u_fNumLights;
uniform vec4 u_f4LightAmbientCol;

uniform vec4 u_f4LightPos[MAX_LIGHTS];
uniform vec3 u_f3LightDiffuseCol[MAX_LIGHTS];
uniform vec3 u_f3LightSpecularCol[MAX_LIGHTS];

//-----------------------------------------------------------------------------

vec4 lighting(vec3 _f3NormalDir_N, vec3 _f3Pos)
{
    vec3 f3VecToCamDir_N = normalize(u_f3CameraPos - _f3Pos);

    vec3 f3DiffuseCol = vec3(0.0);
    vec3 f3SpecularCol = vec3(0.0);

    // HACK: loop in reverse
    // For some unexplained reason the older version would generate an infinite loop on
    // macOS Mojave 10.14.5 with AMD Radeon R9 M395X 4 Go.
    // There might be a bug in the GLSL compiler but we haven't confirmed it yet.
    //  for(int i = 0; i < int(u_fNumLights); ++i)
    for(int i = int(u_fNumLights); --i >= 0 ;)
    {
        vec3 f3LightDir = u_f4LightPos[i].w * _f3Pos - u_f4LightPos[i].xyz;
        vec3 f3LightDir_N = normalize(f3LightDir);

        float fCosTheta  = abs(dot(-f3LightDir_N, _f3NormalDir_N ));
        f3DiffuseCol += fCosTheta * u_f3LightDiffuseCol[i] * u_f4DiffuseCol.rgb;

        vec3 r = reflect(f3LightDir_N, _f3NormalDir_N);
        float fCosAlpha  = clamp(abs(dot(f3VecToCamDir_N, r)), 0, 1);
        f3SpecularCol += pow(fCosAlpha, u_fShininess) * u_f3LightSpecularCol[i] * u_f4SpecularCol.rgb;
    }

    return vec4((u_f4LightAmbientCol.rgb * u_f4AmbientCol.rgb) + f3DiffuseCol + f3SpecularCol, u_f4DiffuseCol.a);
}

//-----------------------------------------------------------------------------

vec3 lightingBlinnPhong(vec3 _f3NormalDir_N, vec3 _f3Pos, vec3 _f3DiffuseCol)
{
    vec3 f3VecToCamDir_N = normalize(u_f3CameraPos - _f3Pos);

    vec3 f3DiffuseCol = vec3(0.);
    vec3 f3SpecularCol = vec3(0.);

    // Oddly enough, the same loop works fine here ...
    // (look at "for(int i = int(u_fNumLights)...")
    for(int i = 0; i < int(u_fNumLights); ++i)
    {
        vec3 f3LightDir = u_f4LightPos[i].w * _f3Pos - u_f4LightPos[i].xyz;

        // We use the Blinn-Phong lighting model.
        float fLitDiffuseCol_N = clamp(abs(dot( normalize(f3LightDir), _f3NormalDir_N )), 0, 1);
        f3DiffuseCol += fLitDiffuseCol_N * u_f3LightDiffuseCol[i] * _f3DiffuseCol;

        vec3 H = normalize(f3LightDir + f3VecToCamDir_N);
        float fLitSpecular = clamp(pow( abs(dot( _f3NormalDir_N, H )), u_fShininess), 0, 1);
        f3SpecularCol += fLitSpecular * u_f3LightSpecularCol[i];
    }

    return vec3(u_f4LightAmbientCol.rgb + f3DiffuseCol + f3SpecularCol);
}
