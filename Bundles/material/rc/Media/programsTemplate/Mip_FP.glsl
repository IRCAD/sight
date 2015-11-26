#version 330

// uniform vec3 c;// ou bien on passe le tableau c ou bien on passe uniquement la valeur de zn qui peut être récupérer depuis Opengl avec un auto
uniform float zn; // dans ce cas on donne le zn directement
uniform float zf;
in vec4 pos;
out float profondeur;

void main()
{
    // correction -> value in [-1;1]
    profondeur = (zn*zf) / ( (gl_FragCoord.z*2.0 -1.0) * (zn-zf) + zf);
}
