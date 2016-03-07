#version 330

uniform float u_vpWidth;
uniform float u_vpHeight;

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

    pnew = pnew.xy/vec2(u_vpWidth,u_vpHeight);

    // optimisation
    profondeur = texture(Mip_prev, pnew).r;
}
