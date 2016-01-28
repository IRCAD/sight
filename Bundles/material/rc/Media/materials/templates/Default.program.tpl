//---------------------------------------------------------------------------
// File generated with genMaterials.py
//
// DO NOT EDIT MANUALLY !!!
//
//---------------------------------------------------------------------------


vertex_program Lighting_VP glsl
{
    source Lighting.glsl
    preprocessor_defines NUM_LIGHTS=1
}

//---------------------------------------------------------------------------

fragment_program Lighting_FP glsl
{
    source Lighting.glsl
    preprocessor_defines NUM_LIGHTS=1
}

//---------------------------------------------------------------------------

fragment_program DepthPeelingCommon_FP glsl
{
    source DepthPeelingCommon_FP.glsl
}

//-----------------------------------------------------------------------------
// Vertex shader materials
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsVP %}

//---------------------------------------------------------------------------

vertex_program Default/{{ shading }}_VP glsl
{
    source RenderScene_VP.glsl
    {% if shadersVP %}attach {{ shadersVP }}{% endif %}

    {% if defines %}preprocessor_defines {{ defines }}{% endif %}

    default_params
    {
        // Common
        param_named_auto u_worldViewProj worldviewproj_matrix
        param_named_auto u_world world_matrix
        param_named_auto u_normalMatrix inverse_transpose_world_matrix
{% for param in params['renderSceneVP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}

//-----------------------------------------------------------------------------
// Common color materials
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program MaterialColor/{{ shading }}_FP glsl
{
    source MaterialColor_FP.glsl
    {% if defines %}preprocessor_defines {{ defines }}{% endif %}

}
{% endfor %}

//-----------------------------------------------------------------------------
// Default technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsFP %}

//-----------------------------------------------------------------------------

fragment_program Default/{{ shading }}_FP glsl
{
    source Default_FP.glsl
    attach MaterialColor/{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
{% for param in params['defaultFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}

//-----------------------------------------------------------------------------
// Depth Peeling technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program DepthPeeling/peel/{{ shading }}_FP glsl
{
    source DepthPeelingPeel_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor/{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        // DepthPeeling
        param_named u_fragData0 int 1
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_diffuse surface_diffuse_colour
{% for param in params['depthPeelingFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}

//-----------------------------------------------------------------------------
// Dual Depth Peeling technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program DualDepthPeeling/peel/{{ shading }}_FP glsl
{
    source DualDepthPeelingPeel_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor/{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        // DualDepthPeeling
        param_named u_nearestDepthBuffer int 1
        param_named u_farthestDepthBuffer int 2
        param_named u_forwardColorBuffer int 3
        param_named u_forwardAlphasBuffer int 4
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_diffuse surface_diffuse_colour
{% for param in params['dualDepthPeelingFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}

//-----------------------------------------------------------------------------
// Hybrid Transparency technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program HybridTransparency/peel/{{ shading }}_FP glsl
{
    source DepthPeelingPeel_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor/{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        // HybridTransparency
        param_named u_fragData0 int 1
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_diffuse surface_diffuse_colour
{% for param in params['depthPeelingFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}


{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program HybridTransparency/weightBlend/{{ shading }}_FP glsl
{
    source WeightedBlended_Weight_Blend_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor/{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    preprocessor_defines HYBRID=1
    default_params
    {
        // HybridTransparency
        param_named u_frontDepthBuffer int 1
        param_named u_occlusionDepthBuffer int 2
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_near near_clip_distance
        param_named_auto u_far far_clip_distance
{% for param in params['HT_weight_blendFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}


//-----------------------------------------------------------------------------
// Weighted Blended technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsFP %}

//---------------------------------------------------------------------------

fragment_program WeightedBlended/weightBlend/{{ shading }}_FP glsl
{
    source WeightedBlended_Weight_Blend_FP.glsl
    attach MaterialColor/{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        // WeightedBlended
        param_named u_occlusionDepthBuffer int 1
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_near near_clip_distance
        param_named_auto u_far far_clip_distance
{% for param in params['weighted_blendFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}


//-----------------------------------------------------------------------------
// Cell shading + depth peeling technique
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsVP %}

//---------------------------------------------------------------------------

vertex_program CelShadingDepthPeeling/{{ shading }}_VP glsl
{
    source RenderScene_VP.glsl
    {% if shadersVP %}attach {{ shadersVP }}{% endif %}

    preprocessor_defines CEL_SHADING=1{% if defines %},{{ defines }}{% endif %}

    default_params
    {
        // CelShadingDepthPeeling
        param_named_auto u_worldViewProj worldviewproj_matrix
        param_named_auto u_world world_matrix
        param_named_auto u_normalMatrix inverse_transpose_world_matrix
{% for param in params['renderSceneVP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}


{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsFP %}

//-----------------------------------------------------------------------------

fragment_program CelShadingDepthPeeling/peel/{{ shading }}_FP glsl
{
    source CelShadingDepthPeelingPeel_FP.glsl
    attach DepthPeelingCommon_FP
    attach MaterialColor/{{ shading }}_FP
    {% if shadersFP %}attach {{ shadersFP }}{% endif %}

    default_params
    {
        // CelShadingDepthPeeling
        param_named u_bufferDepth int 1
        param_named_auto u_vpWidth viewport_width
        param_named_auto u_vpHeight viewport_height
        param_named_auto u_diffuse surface_diffuse_colour
{% for param in params['depthPeelingFP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}
