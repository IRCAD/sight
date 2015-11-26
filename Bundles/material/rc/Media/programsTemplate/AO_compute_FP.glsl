#version 400

// PI value
const float PI = 3.141592653589793238462643383;
in vec4 vertex;
uniform sampler2D MipMap_tex; // MIP MAP texture
uniform mat4 MVP; // modelViewProj matrix
uniform float h; // height
uniform float w; // width
uniform int s; // samples number
uniform float r; // radius (spiral)
//uniform int tau; // number spiral turn
uniform int MAX_MIP; // max mip level
uniform float VerticalFieldOfView; // fov computed by ogre
uniform float FAR_PLANE; //z far


uniform int q; // = log(q') in the paper

// values given in the paper,
// k = 1
// sigma = 1
// Epsilon = 0.0001
// beta = 10⁻⁴
uniform int k;
uniform int sigma;
uniform float epsilon;
uniform float beta;

// test 1 -> return only the occlusion value
//out float a;

// test
out vec4 fragColor;

void main()
{

    // we must stay with the unit model
    float r_fake = r;


    // best value for tau is 7 when s = 9
    // cast in float for best precision
    int tau = int(floor(float(s) * float(7)/float(9)));


    // get the texture coordinate
    vec2 text_coord = gl_FragCoord.xy/vec2(w,h);

    // we first compute the depth of the camera point
    float zc = textureLod(MipMap_tex,text_coord,0).r;



    //-------------------------
    // Calcul de xc, yc
    //-------------------------

    vec2 c_ = zc * vec2 ( ((1.0f - MVP[0][2])/MVP[0][0]) - ((2.0f*(gl_FragCoord.x +0.5f)) / (w* MVP[0][0])) ,
                        ((1.0f + MVP[1][2])/MVP[1][1]) - ((-2.0f*(gl_FragCoord.y +0.5f)) / (h* MVP[1][1])) );


    //--------------------------------------------------------------------------
    // Calcul de nc -> dérivation du point c en x et y puis dot et normalize
    //--------------------------------------------------------------------------

    vec3 c = vec3(c_,zc);
    // we can use dFdx function instead of doing the calcul directly

    vec3 nc = normalize(cross(dFdy(c),dFdx(c)));


    // ATTENTION PEUT-ÊTRE INCORRECT CAR UTILISATION DE FONCTION G3D POUR CETTE PARTIE DANS LE PAPIER ORIGINAL

    //---------------------------------------------------------------------------------
    // Calcul de S qui est la taille en pixel d'un objet de 1m à une profondeur z = 1m
    //---------------------------------------------------------------------------------
//    vec4 bottom_object = vec4(0,0,-1,1); // bottom position of the object in the space
//    vec4 top_object = vec4(0,1,-1,1);
//    vec4 bottom_screen = MVP * bottom_object;
//    vec4 top_screen = MVP * top_object;
//    float S = distance(bottom_screen,top_screen); // compute the distance on the screen ?
//    float S = 500.0;

    // correct calcul from paper
    float S = -h/ (2.0f * tan(VerticalFieldOfView * 0.5));

    //-------------------------
    // Calcul de r'
    //-------------------------
    float r_ = -r_fake * S / zc;


    //-------------------------------------
    // Calcul de fi, invariant dans la boucle
    //-------------------------------------
    int fi = ((int(30) * int(gl_FragCoord.x)) ^ int(gl_FragCoord.y)) + (10*int(gl_FragCoord.x)*int(gl_FragCoord.y));


//    vec2 q[s];

    //-------------------------
    // Main sample loop
    //-------------------------
    int i;
    float sum=0.0f; // experimental

    for (i=0; i < s; ++i)
    {
        float alpha_i = (float(i)+0.5)/s;
        float h_i = r_ * alpha_i;


        float theta_i = 2 * PI * alpha_i * tau + float(fi); // radian

        vec2 u_i = vec2(cos(theta_i),sin(theta_i));

//         int m = clamp (findMSB(int (h)) - log_q);

        // On préfèrera une implémentation directe plutôt que l'utilisation de clamp, find MSB , etc
        int m_i = int (clamp(int(floor(log(h_i))) - q,0,MAX_MIP)); // à prendre avec des précautions!


        // screen-space pixel with the correct size pow(2.0,m_i)
        vec2 pixel_i = vec2(vec2(gl_FragCoord.xy) + vec2(h_i*u_i));

//        vec2 pixel_i_tmp = vec2(int(pixel_i.x) >> m_i, int(pixel_i.y) >> m_i);


        // convert in texture coordinate -> normalisation par
        float viewport_h_i = h/pow(2.0,m_i);
        float viewport_w_i = w/pow(2.0,m_i);
        vec2 pixel_coord_i = pixel_i/vec2(viewport_w_i,viewport_h_i);

//        vec2 uv_i = clamp (pixel_coord_i,vec2(0),textureSize(MipMap_tex,m_i) - vec2(1));

        // get the depth in the correct texture
        float z_i = textureLod(MipMap_tex, pixel_coord_i ,m_i).r;

        // compute the location of the q[i]

        vec2 q_ = z_i * vec2( ((1.0f - MVP[0][2])/MVP[0][0]) - ((2.0f*(pixel_i.x+0.5f)) / (w* MVP[0][0])) ,
                            ((1.0f + MVP[1][2])/MVP[1][1]) - ((-2.0f*(pixel_i.y+0.5f)) / (h* MVP[1][1])) );

        vec3 qi = vec3(q_,z_i);
        // vector Vi = vector from point C to point Qi

        vec3 vi = qi - c;

//        sum += (max(0.0f,(dot (vi,nc) + zc* beta)/(dot(vi,vi)+epsilon)));

        float f = max (pow(r_fake,2.0) - dot(vi,vi),0.0);
        sum += (f*f*f* max( ( (dot(vi,nc)-0.01) / (0.01+dot(vi,vi)) ) ,
                                    0.0 ));

    }



    //-------------------------
    // Compute the obscurance
    //-------------------------

//    float r_3 = pow(r_fake,3);
//    sum /= r_3 * r_3;

    // here we take sigma = 1, epsilon = 0.0001, Beta = 10⁻⁴ and k = 1
//    float a = pow (max (0.0, 1.0f-(2.0f* sigma *sum)/s),k);
    float a = max (0.0,1.0 - sum * (1.0/ pow(r_fake,6.0)) *(5.0 / s));


    // 2*2 bilateral reconstruction filter
    if (abs(dFdx(zc)) < 0.02) // si on a une faible variance de la profondeur suivant l'axe des X
    {
        a -= dFdx(a) * ((int(gl_FragCoord.x) & 1) - 0.5);
    }
    if (abs(dFdy(zc)) < 0.02)
    {
        a -= dFdy(a) * ((int(gl_FragCoord.y) & 1) - 0.5);
    }

    float zcToKey = clamp (zc * (1.0/FAR_PLANE),0.0,1.0);

    float tmp = floor(zcToKey * 256.0);

    fragColor.rgb = vec3(a ,tmp*(1.0/256.0),zcToKey*256.0 - tmp);

    // normal test
//    fragColor.rgb = (nc *0.5)+0.5 ;

}
