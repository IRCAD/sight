#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


in VertexDataIn
{
#ifdef PIXEL_LIT
    vec3 oPosition_WS;
    vec3 oNormal_WS;
    vec4 oColor;
#else
    vec4 oColor;
#endif // PIXEL_LIT
    vec2 oTexCoord;

} vertexIn[];

out VertexDataOut
{
#ifdef PIXEL_LIT
    vec3 oPosition_WS;
    vec3 oNormal_WS;
    vec4 oColor;
#else
    vec4 oColor;
#endif // VERTEX_COLOR
    vec2 oTexCoord;
} vertexOut;

uniform sampler2D u_colorPrimitiveTexture;
uniform vec2 u_colorPrimitiveTextureSize;

void main(void)
{

    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;

        vertexOut.oTexCoord = vertexIn[i].oTexCoord;
#ifdef PIXEL_LIT
        vertexOut.oPosition_WS = vertexIn[i].oPosition_WS;
        vertexOut.oNormal_WS = vertexIn[i].oNormal_WS;

        // compute offset
        int div = gl_PrimitiveIDIn / int(u_colorPrimitiveTextureSize[0]);
        int mod = gl_PrimitiveIDIn - int(u_colorPrimitiveTextureSize[0]) * div;

        vec2 uv = vec2(mod / u_colorPrimitiveTextureSize[0] , div/ u_colorPrimitiveTextureSize[1]);
        vertexOut.oColor = texture(u_colorPrimitiveTexture, uv );
#else
//        vertexOut.oColor = vertexIn[i].oColor;
#endif // VERTEX_COLOR
        EmitVertex();
    }
    EndPrimitive();
}
