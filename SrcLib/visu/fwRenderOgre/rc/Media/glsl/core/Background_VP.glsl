#version 150

in vec4 position;

uniform mat4 u_worldViewProj;

out vec2 uv;

// Specific for generating background with an ::Ogre::Rectangle2D
void main()
{
        // transform is the classic transformation apply to a point (matrixviewprojection) in fixed function pipeline
        gl_Position = u_worldViewProj * position;

        // sign return -1 for x<0, 0 for x=0 and 1 for x>1
        vec2 inPos = sign(position.xy);

        uv = (vec2(inPos.x, -inPos.y) + 1.0)/2.0;
}
