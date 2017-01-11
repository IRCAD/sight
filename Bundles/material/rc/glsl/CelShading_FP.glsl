#version 150

in Data {
	vec3 normal_VS;
        vec3 normal_WS;
	vec3 eyeDir_VS;
	vec3 lightDir_VS;
} DataIn;

uniform vec3 u_ambientMaterial;
uniform vec3 u_diffuseMaterial;
uniform vec3 u_specularMaterial;
uniform float u_shininess;

out vec4 fragColor;

void main(void)
{
	vec3 N = normalize(DataIn.normal_VS);
	vec3 L = normalize(DataIn.lightDir_VS);
	vec3 E = normalize(DataIn.eyeDir_VS);
	vec3 H = normalize(L + E);

	// Threshold values for celshading
	const float A = 0.1;
        const float B = 0.3;
        const float C = 0.5;
        const float D = 0.7;

	// Compute diffuse factor
	float df = max(0.0, dot(N, L));
	if (df < A) df = 0.0;
	else if (df < B) df = B;
	else if (df < C) df = C;
	else df = D;

	// Compute specular factor
	float sf = max(0.0, dot(N, H));
	sf = pow(sf, u_shininess);
	sf = step(0.5, sf);

        vec3 color = u_ambientMaterial + df * u_diffuseMaterial + sf * u_specularMaterial;

        fragColor = vec4(color, 1.0);
}
