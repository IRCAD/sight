#version 150

in vec2 uv;

uniform sampler2D colorMap;
uniform sampler2D depthMap;
uniform sampler2D normalMap;

out vec4 fragColor;

void main()
{
    vec4 color = texture(colorMap, uv);
    vec4 depth = texture(depthMap, uv);
    fragColor = color * (vec4(1.0) - depth);
//    fragColor = color;
//    fragColor = depth;
}
