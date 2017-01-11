#version 150
in vec4 position;
in vec3 normal;
in vec2 uv0;

uniform mat4 u_worldViewProj;
uniform mat4 u_worldViewMatrix;
uniform mat4 u_viewMatrix;
uniform mat4 u_normalMatrix;
uniform vec4 u_lightPosition;
uniform vec3 u_cameraPos;

out Data {
    vec3 normal_VS;
    vec3 normal_WS;
    vec3 eyeDir_VS;
    vec3 lightDir_VS;
    vec2 texCoord;
} DataOut;

void main(void)
{
    vec4 pos = u_worldViewMatrix * position;
    DataOut.normal_VS = vec3(u_normalMatrix * vec4(normal, 0.f));
    DataOut.normal_WS = normal.xyz;
    DataOut.eyeDir_VS = u_cameraPos;
    DataOut.lightDir_VS = -u_lightPosition.xyz;
    DataOut.texCoord = uv0;

    gl_Position = u_worldViewProj * position;
}
