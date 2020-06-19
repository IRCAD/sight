#version 330
in vec2 uv;

uniform sampler2D u_fragData0;
uniform sampler2D u_fragData1;

out vec4 FragColor;

float unpackFloatFromVec4(vec4 value);

/** Final shader :
  * Blend scene with blended object
  **/
void main()
{
    vec4 frontColorBuffer = texture(u_fragData0, uv);
    float depthMap = unpackFloatFromVec4(texture(u_fragData1, uv));

    // Manage surrounding background (like a stencil)
    if(depthMap == 0)
    {
        FragColor.a = 0;
        gl_FragDepth = 1.f;
    }
    else
    {
        FragColor = vec4(frontColorBuffer.rgb, 1.-frontColorBuffer.a);
        gl_FragDepth = depthMap;
    }
}
