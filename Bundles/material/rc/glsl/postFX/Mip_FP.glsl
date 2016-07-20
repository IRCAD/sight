#version 330

// uniform vec3 c;// ou bien on passe le tableau c ou bien on passe uniquement la valeur de zn qui peut être récupérer depuis Opengl avec un auto
uniform float zn; // dans ce cas on donne le zn directement
uniform float zf;
in vec4 pos;
out float profondeur;

void main()
{

    profondeur = (zn*zf) / ( (gl_FragCoord.z) * (zn-zf) + zf);
}
