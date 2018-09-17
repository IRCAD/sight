#version 410

in vec2 uv;

uniform sampler2D u_fragData;
uniform sampler2D u_map;

uniform vec4 u_viewport;

out vec4 FragColor;

void main()
{
    // We have to compute the uv inside the map according to the location of the video plane
    // This plane is always stretched in height and centered in X
    ivec2 mapSize = textureSize(u_map, 0).xy;
    float videoRatio = u_viewport.y / mapSize.y;
    float normalizedVideoPlaneWidth = mapSize.x * videoRatio * u_viewport.z;

    float normalizedCenterX = uv.x - 0.5;
    float scaledX = normalizedCenterX / normalizedVideoPlaneWidth;
    vec2 clampedUV = vec2(scaledX + 0.5, uv.y);

    vec2 remap = texture(u_map, clampedUV).xy;

    vec2 normalizedRemap = remap/mapSize;
    float remapX = normalizedRemap.x - 0.5;
    float remapScaledX = remapX * normalizedVideoPlaneWidth;
    vec2 remapClampedUV = vec2(remapScaledX + 0.5, normalizedRemap.y);

    FragColor = texture(u_fragData, remapClampedUV);
}
