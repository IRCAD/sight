#version 330

uniform vec4 u_viewport;
uniform sampler2D Occlusion_Map;
uniform sampler2D scene;
uniform int u_blend;
uniform float aoIntensity;

out vec4 FragColor;

// Parameters controling AO tuning
#define MIN_AMBIENT_LIGHT 0.1

void main()
{
    vec2 texCoord = gl_FragCoord.xy * u_viewport.zw;

    // get ao value for the current point
    float ao = texture(Occlusion_Map,texCoord).r;

    const float scaleIntensity = 5.0;
    // modulate ao with the intensity
    ao = (clamp(1.0 - (1.0 - ao) * aoIntensity * scaleIntensity, 0.0, 1.0) + MIN_AMBIENT_LIGHT) /  (1.0 + MIN_AMBIENT_LIGHT);

    if(u_blend == 0)
    {
        FragColor = vec4(vec3(ao), 1.0);
    }
    else
    {
        // get the color of the scene for the current pixel
        vec4 color = texture(scene,texCoord);

        FragColor = vec4(color.rgb * ao, color.a);
    }

}
