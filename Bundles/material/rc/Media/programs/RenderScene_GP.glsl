#version 400

#if defined(QUAD)
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 4) out;
#else
#if defined(TETRA)
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 6) out;
#else
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
#endif
#endif

in VertexDataIn
{
    vec3 oNormal;

#ifdef VERTEX_COLOR
    vec4 oColor;
#endif // VERTEX_COLOR

#ifdef DIFFUSE_TEX
    vec2 oTexCoord;
#endif // DIFFUSE_TEX

} vertexIn[];

out vec3 oPos;
out vec3 oNormal;
#if defined(VERTEX_COLOR) || defined(PER_PRIMITIVE_COLOR)
out uint oColour;
#endif // VERTEX_COLOR
#ifdef DIFFUSE_TEX
out vec2 oUv0;
#endif // DIFFUSE_TEX

#ifdef PER_PRIMITIVE_COLOR
uniform sampler2D u_colorPrimitiveTexture;
uniform vec2 u_colorPrimitiveTextureSize;

vec4 ppcolor;
#endif // PER_PRIMITIVE_COLOR

void emit(int index)
{
    oPos = gl_in[index].gl_Position.xyz;
    oNormal = vertexIn[index].oNormal;

#ifdef DIFFUSE_TEX
    oUv0 = vertexIn[index].oTexCoord;
#endif // DIFFUSE_TEX

#ifdef VERTEX_COLOR
    vec4 color;
    color = vertexIn[index].oColor;
#   ifdef PER_PRIMITIVE_COLOR
    color = ppcolor * color;
#   endif // PER_PRIMITIVE_COLOR
    oColour = packUnorm4x8(color);
#else
#   ifdef PER_PRIMITIVE_COLOR
    oColour = packUnorm4x8(ppcolor);
#   endif // PER_PRIMITIVE_COLOR
#endif // VERTEX_COLOR

    EmitVertex();
}

void main(void)
{
#ifdef PER_PRIMITIVE_COLOR
    // compute offset
    int div = gl_PrimitiveIDIn / int(u_colorPrimitiveTextureSize[0]);
    int mod = gl_PrimitiveIDIn - int(u_colorPrimitiveTextureSize[0]) * div;

    vec2 uv = vec2(mod / u_colorPrimitiveTextureSize[0] , div/ u_colorPrimitiveTextureSize[1]);
    ppcolor = texture(u_colorPrimitiveTexture, uv ).bgra;
#endif // PER_PRIMITIVE_COLOR

#if defined(QUAD)
    emit(0); emit(1); emit(3); emit(2);
#else
#if defined(TETRA)
    emit(0); emit(1); emit(2); emit(3); emit(0); emit(1);
#else
    emit(0); emit(1); emit(2);
#endif
#endif
    EndPrimitive();
}
