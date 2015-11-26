#version 330


uniform float w;
uniform float h;
uniform sampler2D scene;
uniform sampler2D Occlusion_map;

out vec4 FragColor;

void main()
{
    vec2 texCoord = gl_FragCoord.xy / vec2(w,h);
    if (texture(Occlusion_map,texCoord).r > 0.9)
    {
        FragColor = texture(scene,texCoord);
    }
    else
    {
        FragColor = texture(scene,texCoord) + vec4(0.5,0,0,0) ;
    }

    //test

//    FragColor = texture(scene,texCoord);
//    FragColor = texture(Occlusion_map,texCoord);



}
