#version 410

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

layout(location = 0) in vec3 vNormal[];

#ifdef VERTEX_COLOR
layout(location = 1) in vec4 vColor[];
#endif // VERTEX_COLOR

#ifdef DIFFUSE_TEX
layout(location = 2) in vec2 vTexCoord[];
#endif // DIFFUSE_TEX

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
    oNormal = vNormal[index];

#ifdef DIFFUSE_TEX
    oUv0 = vTexCoord[index];
#endif // DIFFUSE_TEX

#ifdef VERTEX_COLOR
    vec4 color;
    color = vColor[index];
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

    vec2 range = max(vec2(1.0, 1.0), u_colorPrimitiveTextureSize - 1.0);
    vec2 uv = vec2(mod / range.x , div/ range.y);
    ppcolor = texture(u_colorPrimitiveTexture, uv );
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
