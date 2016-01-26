//---------------------------------------------------------------------------
// File generated with genMaterials.py
//
// DO NOT EDIT MANUALLY !!!
//
//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Vertex shader materials
//-----------------------------------------------------------------------------

{% for shading, defines, shadersVP, shadersFP, useAdjInfo, params in configsVP %}

//---------------------------------------------------------------------------

vertex_program RenderScene_R2VB_{{ shading }}_VP_glsl glsl
{
    source RenderScene_VP.glsl

    preprocessor_defines R2VB=1{% if defines %}, {{ defines }}{% endif %}

}
{% endfor %}

//-----------------------------------------------------------------------------
// Geometry shader materials
//-----------------------------------------------------------------------------

{% for shading, defines, shadersGP, shadersFP, useAdjInfo, params in configsGP %}

//---------------------------------------------------------------------------

geometry_program RenderScene_{{ shading }}_GP_glsl glsl
{
    source RenderScene_GP.glsl

    {% if defines %}preprocessor_defines {{ defines }}{% endif %}

    {% if useAdjInfo == "1" %}uses_adjacency_information true{% endif %}

    default_params
    {
{% for param in params['renderSceneGP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}
