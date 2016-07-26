#version 400

// PI value
const float PI = 3.141592653589793238462643383;
in vec4 vertex;
uniform sampler2D MipMap_tex; // MIP MAP texture
uniform float h; // height
uniform float w; // width
uniform int u_numSamples; // samples number
uniform float u_radius; // radius (spiral)
//uniform int tau; // number spiral turn
uniform int eu_MaxMipLevels; // max mip level
uniform float VerticalFieldOfView; // fov computed by ogre
uniform float FAR_PLANE; //z far

// we need here to use the Projection matrix
/**  vec4(-2.0f / (width*P[0][0]),
          -2.0f / (height*P[1][1]),
          ( 1.0f - P[0][2]) / P[0][0],
          ( 1.0f + P[1][2]) / P[1][1]) */
uniform vec4 eu_projInfo;

uniform int u_q; // = log(q') in the paper

// values given in the paper,
// k = 1
// sigma = 1
// Epsilon = 0.0001
// beta = 10⁻⁴
uniform int u_k;
uniform int u_sigma;
uniform float u_epsilon;
uniform float u_beta;

// test 1 -> return only the occlusion value
//out float a;

// test
out vec4 fragColor;

/** Reconstruct camera-space P.xyz from screen-space S = (x, y) in
    pixels and camera-space z < 0.  Assumes that the upper-left pixel center
    is at (0.5, 0.5) [but that need not be the location at which the sample tap
    was placed!]

    Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.
  */
vec3 reconstructCSPosition(vec2 S, float z)
{
    // compute the postion from the Projection Matrix
    return vec3 ((S.xy * eu_projInfo.xy + eu_projInfo.zw) * z, z);
}

/** Read the camera-space position of the point at screen-space pixel ssP */
vec3 getPosition(ivec2 ssP, float z)
{
    // Offset to pixel center
    vec3 P = reconstructCSPosition(vec2(ssP) + vec2(0.5), z);
    return P;
}

void main()
{

    // we must stay with the unit model
    float r_fake = u_radius *10;


    // best value for tau is 7 when s = 9
    // cast in float for best precision
    int tau = int(floor(float(u_numSamples) * float(7)/float(9)));


    // compute the screen-Space location of C
    ivec2 ssC = ivec2(gl_FragCoord.xy);


    //-------------------------------------
    // Calcul de fi, invariant dans la boucle
    //-------------------------------------
    float randomPatternRotationAngle = (3 * ssC.x ^ ssC.y + ssC.x * ssC.y) * 10;


    // get the texture coordinate
    vec2 text_coord = gl_FragCoord.xy/vec2(w,h);

    // we first compute the depth of the camera point
    float zc = textureLod(MipMap_tex,text_coord,0).r;



    //-------------------------
    // Calcul de C (xc,yc,zc)
    //-------------------------

    vec3 c = getPosition(ivec2(gl_FragCoord.xy),zc);


    //--------------------------------------------------------------------------
    // Calcul de nc -> dérivation du point c en x et y puis dot et normalize
    //--------------------------------------------------------------------------

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


    int fi = ((int(30) * int(gl_FragCoord.x)) ^ int(gl_FragCoord.y)) + (10*int(gl_FragCoord.x)*int(gl_FragCoord.y));


//    vec2 q[s];

    //-------------------------
    // Main sample loop
    //-------------------------
    int i;
    float sum=0.0f; // experimental

    for (i=0; i < u_numSamples; ++i)
    {
        float alpha_i = (float(i)+0.5)/u_numSamples;
        float h_i = r_ * alpha_i;


        float theta_i = 2 * PI * alpha_i * tau + float(randomPatternRotationAngle); // radian

        vec2 u_i = vec2(cos(theta_i),sin(theta_i));


//        int m_i = int (clamp(int(floor(log(h_i))) - q,0,eu_MaxMipLevels)); // à prendre avec des précautions!
        int m_i = clamp(findMSB(int(h_i)) - u_q, 0, eu_MaxMipLevels);

        // screen-space pixel with the correct size pow(2.0,m_i)
        vec2 pixel_i = vec2(vec2(gl_FragCoord.xy) + vec2(h_i*u_i));

//        vec2 pixel_i_tmp = vec2(int(pixel_i.x) >> m_i, int(pixel_i.y) >> m_i);


        // convert in texture coordinate -> normalisation
        vec2 pixel_coord_i = pixel_i/vec2(w,h);

//        vec2 uv_i = clamp (pixel_coord_i,vec2(0),textureSize(MipMap_tex,m_i) - vec2(1));

        // get the depth in the correct texture
        float z_i = textureLod(MipMap_tex, pixel_coord_i ,m_i).r;

        // compute the location of the q[i]
        vec3 qi = getPosition(ivec2(pixel_i.xy + vec2(0.5)),z_i);

        // vector Vi = vector from point C to point Qi

        vec3 vi = qi - c;

//        sum += (max(0.0f,(dot (vi,nc) + zc* u_beta)/(dot(vi,vi)+epsilon)));

        const float epsilon = 0.01;
        const float bias = 0.15;

        float vv = dot (vi,vi);

        float f = max (r_fake * r_fake - vv,0.0);
        sum += (f*f*f* max( ( (dot(vi,nc)-bias) / (epsilon+vv) ), 0.0 ));

    }



    //-------------------------
    // Compute the obscurance
    //-------------------------

//    float r_3 = pow(r_fake,3);
//    sum /= r_3 * r_3;

    // here we take sigma = 1, epsilon = 0.0001, Beta = 10⁻⁴ and k = 1
//    float a = pow (max (0.0, 1.0f-(2.0f* u_sigma *sum)/s),u_k);
    const float intensity = 2.0;
    float a = max (0.0,1.0 - sum * (intensity/ pow(r_fake,6.0)) *(5.0 / u_numSamples));


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
