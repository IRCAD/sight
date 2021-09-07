#version 330

uniform vec4 u_viewport;

uniform int scale; // here we take scale = 2
uniform sampler2D AO_map_blur;
in vec4 pos;
//const float gaussian[5] = float[] (0.153170, 0.144893, 0.122649, 0.092902, 0.062970);
//const int R = 4;
const float gaussian[7] = float[] (0.111220,0.107798,0.098151,0.083953,0.067458,0.050920,0.036108);
const int R = 6;
const float epsilon = 0.0001;

out vec4 fragColour;

void main()
{
    // normalisation
    vec2 texCoord = gl_FragCoord.xy* u_viewport.zw;

//    fragColour.rgb = vec3(texture(AO_map,texCoord).rgb);

    vec4 tmp = texture(AO_map_blur,texCoord);


    vec2 key_values = tmp.gb; // the last two values in AO_map(r,g,b)
    // return a number on {0,1}
    float key = key_values.x * (256.0 / 257.0) + key_values.y * (1.0 / 257.0);

    float sum = tmp.r;

    float base_weight = gaussian[0];
    float total_weight = base_weight;

    sum *= total_weight;

    int r;
    for (r=-R; r<=R; ++r)
    {
        if (r!=0)
        {
            // vertical = vec2(0,1)
            vec2 uv =  (gl_FragCoord.xy + vec2(0,1) * (r * scale))* u_viewport.zw ;
            tmp = texture(AO_map_blur,uv);
            float tapKey = tmp.g * (256.0 / 257.0) + tmp.b * (1.0 / 257.0);
            float value = tmp.r;

            // spatial domain -> gaussian tap
            float weight = 0.3 + gaussian[abs(r)];

            // bilateral weight
            weight *= max (0.0, 1.0 - 2000.0 * abs(tapKey - key));

            sum += value * weight;
            total_weight += weight;
        }
    }

    fragColour.rgb = vec3(sum/(total_weight+epsilon));

}
