//---------------------------------------------------------------------------
// File generated with genMaterials.py
//
// DO NOT EDIT MANUALLY !!!
//
//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Vertex shader materials
//-----------------------------------------------------------------------------

{% for shading, defines, params in configsVP %}

//---------------------------------------------------------------------------

vertex_program R2VB/{{ shading }}_VP glsl
{
    source RenderScene_VP.glsl

    preprocessor_defines R2VB=1{% if defines %},{{ defines }}{% endif %}

}
{% endfor %}

//-----------------------------------------------------------------------------
// Geometry shader materials
//-----------------------------------------------------------------------------

{% for shading, defines, params in configsGP %}

//---------------------------------------------------------------------------

geometry_program R2VB/{{ shading }}_GP glsl
{
    source RenderScene_GP.glsl

    {% if defines %}preprocessor_defines {{ defines }}{% endif %}

    default_params
    {
{% for param in params['renderSceneGP'] %}
        {{ param }}
{% endfor %}
    }
}
{% endfor %}
