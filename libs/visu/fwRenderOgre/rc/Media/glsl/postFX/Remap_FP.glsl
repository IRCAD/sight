#version 410

in vec2 uv;

uniform sampler2D u_fragData;
uniform sampler2D u_map;

uniform vec4 u_viewport;

out vec4 FragColor;

void main()
{
    ivec2 mapSize = textureSize(u_map, 0).xy;

    if(mapSize == ivec2(u_viewport.xy))
    {
        // Ignore filtering when the distortion map matches the viewport.
        // This is needed when rendering distorted stereo pairs.
        vec2 remap = texture(u_map, uv).xy;
        FragColor  = texelFetch(u_fragData, ivec2(remap), 0);
    }
    else
    {
        // We have to compute the uv inside the map according to the location of the video plane
        // This plane is always stretched in height and centered in X
        float videoRatio = u_viewport.y / mapSize.y;
        float normalizedVideoPlaneWidth = mapSize.x * videoRatio * u_viewport.z;

        float normalizedCenterX = uv.x - 0.5;
        float scaledX = normalizedCenterX / normalizedVideoPlaneWidth;
        vec2 clampedUV = vec2(scaledX + 0.5, uv.y);

        vec2 remap = texture(u_map, clampedUV).xy;
        vec2 normalizedRemap = remap/(mapSize - ivec2(1));

        float remapX = normalizedRemap.x - 0.5;
        float remapScaledX = remapX * normalizedVideoPlaneWidth;
        vec2 remapClampedUV = vec2(remapScaledX + 0.5, normalizedRemap.y);

        FragColor = texture(u_fragData, remapClampedUV);
    }
}
