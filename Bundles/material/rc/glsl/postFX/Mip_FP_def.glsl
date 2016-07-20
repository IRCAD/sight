#version 330

uniform float eu_vpWidth;
uniform float eu_vpHeight;

uniform sampler2D Mip_prev;
in vec4 pos;
out float profondeur;

void main()
{
    // appliquer la formule en question pour définir la position de pnew en x et y
    vec2 pnew;
    pnew.x = (2*gl_FragCoord.x + (int(gl_FragCoord.y) & int(1) ^ int(1)));
    pnew.y = (2*gl_FragCoord.y + (int(gl_FragCoord.x) & int(1) ^ int(1)));

    // récupérer la valeur de Mip_prev pour p_new
    // normalisation

    pnew = pnew.xy/vec2(eu_vpWidth,eu_vpHeight);

    // optimisation
    profondeur = texture(Mip_prev, pnew).r;
}
