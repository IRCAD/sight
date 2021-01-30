//---------------------------------------------------------------------------
// File generated with genMaterials.py
//
// DO NOT EDIT MANUALLY !!!
//
//---------------------------------------------------------------------------


vertex_program Lighting_VP glsl
{
    source Lighting.glsl
}

//---------------------------------------------------------------------------

fragment_program Lighting_FP glsl
{
    source Lighting.glsl
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
    source Main_FP.glsl
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
