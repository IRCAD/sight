//---------------------------------------------------------------------------
// File generated with genRayTracedMaterials.py
//
// DO NOT EDIT MANUALLY !!!
//
//---------------------------------------------------------------------------

{% for name, defines, params in configsFP %}
{% if not 'MImP_AImC' in name %}

//----------------------------------------------

fragment_program RayTracedVolume{{ name }}_FP glsl
{
    source RayTracedVolume_FP.glsl
    attach TransferFunction_FP
{% if defines %}
    preprocessor_defines {{ defines }}
{% endif %}

    default_params
    {
        param_named_auto u_viewportWidth viewport_width
        param_named_auto u_viewportHeight viewport_height
        param_named_auto u_clippingNear near_clip_distance
        param_named_auto u_clippingFar far_clip_distance
{% if 'IDVR' not in defines %}
        param_named_auto u_invWorldViewProj inverse_worldviewproj_matrix
{% endif %}
        param_named u_sampleDistance float 0.002
        param_named u_image int 0
        param_named u_tfTexture int 1
        param_named u_entryPoints int 2
        // Lighting
        param_named_auto u_cameraPos camera_position
        param_named_auto u_shininess surface_shininess
        param_named u_numLights int 1
        param_named u_lightDir[0] float4 0 0 0 0
        param_named u_lightDir[1] float4 0 0 0 0
        param_named u_lightDir[2] float4 0 0 0 0
        param_named u_lightDir[3] float4 0 0 0 0
        param_named u_lightDir[4] float4 0 0 0 0
        param_named u_lightDir[5] float4 0 0 0 0
        param_named u_lightDir[6] float4 0 0 0 0
        param_named u_lightDir[7] float4 0 0 0 0
        param_named u_lightDir[8] float4 0 0 0 0
        param_named u_lightDir[9] float4 0 0 0 0
        param_named u_lightDiffuse[0] float4 0 0 0 0
        param_named u_lightDiffuse[1] float4 0 0 0 0
        param_named u_lightDiffuse[2] float4 0 0 0 0
        param_named u_lightDiffuse[3] float4 0 0 0 0
        param_named u_lightDiffuse[4] float4 0 0 0 0
        param_named u_lightDiffuse[5] float4 0 0 0 0
        param_named u_lightDiffuse[6] float4 0 0 0 0
        param_named u_lightDiffuse[7] float4 0 0 0 0
        param_named u_lightDiffuse[8] float4 0 0 0 0
        param_named u_lightDiffuse[9] float4 0 0 0 0
        param_named u_lightSpecular[0] float4 0 0 0 0
        param_named u_lightSpecular[1] float4 0 0 0 0
        param_named u_lightSpecular[2] float4 0 0 0 0
        param_named u_lightSpecular[3] float4 0 0 0 0
        param_named u_lightSpecular[4] float4 0 0 0 0
        param_named u_lightSpecular[5] float4 0 0 0 0
        param_named u_lightSpecular[6] float4 0 0 0 0
        param_named u_lightSpecular[7] float4 0 0 0 0
        param_named u_lightSpecular[8] float4 0 0 0 0
        param_named u_lightSpecular[9] float4 0 0 0 0
{% for param in params['preIntParams'] %}
        {{ param }}
{% endfor %}
{% if 'IDVR' in defines %}
        // IDVR
        param_named u_IC int 3
{% if 'IDVR=1' in defines %}
        param_named u_JFA int 4
{% endif %}
{% endif %}
{% if   'AMBIENT_OCCLUSION' in defines or
        'COLOR_BLEEDING' in defines or
        'SHADOWS' in defines %}
        // Volume Illumination
        param_named u_illuminationVolume int {% if 'IDVR' in defines %}{% if 'IDVR=1' in defines %}5{% else %}4{% endif %}{% else %}3
{% endif %}

        param_named u_volIllumFactor float4 1.0 1.0 1.0 1.0
{% endif %}
    }
}
{% endif %}
{% endfor %}

//----------------------------------------------

fragment_program VR/RayTracedVolume3D5_FP glsl
{
    source RayTracedVolume_FP.glsl
    attach TransferFunction_FP
    preprocessor_defines MODE3D=1,VIEWPOINTS=5

    default_params
    {
        param_named_auto u_viewportWidth viewport_width
        param_named_auto u_viewportHeight viewport_height
        param_named_auto u_clippingNear near_clip_distance
        param_named_auto u_clippingFar far_clip_distance
        param_named_auto u_cameraPos camera_position
        param_named u_sampleDistance float 0.002
        param_named u_lobeOffset float 1.
        param_named u_image int 0
        param_named u_tfTexture int 1
        param_named u_entryPoints0 int 2
        param_named u_entryPoints1 int 3
        param_named u_entryPoints2 int 4
        param_named u_entryPoints3 int 5
        param_named u_entryPoints4 int 6
        // Lighting
        param_named_auto u_cameraPos camera_position
        param_named_auto u_shininess surface_shininess
        param_named u_numLights int 1
        param_named_auto u_lightDir light_position_array 10
        param_named_auto u_lightDiffuse light_diffuse_colour_array 10
        param_named_auto u_lightSpecular light_specular_colour_array 10
    }
}

//----------------------------------------------

fragment_program VR/RayTracedVolume3D8_FP glsl
{
    source RayTracedVolume_FP.glsl
    attach TransferFunction_FP
    preprocessor_defines MODE3D=1,VIEWPOINTS=8

    default_params
    {
        param_named_auto u_viewportWidth viewport_width
        param_named_auto u_viewportHeight viewport_height
        param_named_auto u_clippingNear near_clip_distance
        param_named_auto u_clippingFar far_clip_distance
        param_named_auto u_cameraPos camera_position
        param_named u_sampleDistance float 0.002
        param_named u_lobeOffset float 1.
        param_named u_image int 0
        param_named u_tfTexture int 1
        param_named u_entryPoints0 int 2
        param_named u_entryPoints1 int 3
        param_named u_entryPoints2 int 4
        param_named u_entryPoints3 int 5
        param_named u_entryPoints4 int 6
        param_named u_entryPoints5 int 7
        param_named u_entryPoints6 int 8
        param_named u_entryPoints7 int 9
        // Lighting
        param_named_auto u_cameraPos camera_position
        param_named_auto u_shininess surface_shininess
        param_named u_numLights int 1
        param_named_auto u_lightDir light_position_array 10
        param_named_auto u_lightDiffuse light_diffuse_colour_array 10
        param_named_auto u_lightSpecular light_specular_colour_array 10
    }
}

//----------------------------------------------

{% for name, defines, params in configsFP %}
{% if not 'MImP_AImC' in name %}

//----------------------------------------------

material RayTracedVolume{{ name }}
{
    technique
    {
        pass
        {
            cull_hardware{% if 'AMBIENT_OCCLUSION' in defines or
                               'COLOR_BLEEDING' in defines or
                               'SHADOWS' in defines or
                               'PREINTEGRATION' in defines %} anticlockwise{% else %} none{% endif %}

            cull_software none
            scene_blend alpha_blend

            vertex_program_ref RenderQuad_VP
            {
            }

            fragment_program_ref RayTracedVolume{{ name }}_FP
            {
            }

            texture_unit image
            {
                tex_address_mode clamp
                filtering bilinear
            }

            texture_unit transferFunction
            {
                filtering none
            }

            texture_unit entryPoints
            {
                tex_address_mode clamp
                filtering none
            }

{% if 'IDVR' in defines %}
            texture_unit IC
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit JFA
            {
                tex_address_mode clamp
                filtering none
            }
{% endif %}

{% if   'AMBIENT_OCCLUSION' in defines or
        'COLOR_BLEEDING' in defines or
        'SHADOWS' in defines %}
            texture_unit illuminationVolume
            {
                tex_address_mode clamp
                filtering bilinear
            }
{% endif %}
        }
    }
}
{% endif %}
{% endfor %}

//----------------------------------------------

material RayTracedVolume3D5
{
    technique
    {
        pass
        {
            cull_hardware none
            cull_software none
            scene_blend alpha_blend

            vertex_program_ref RenderQuad_VP
            {
            }

            fragment_program_ref VR/RayTracedVolume3D5_FP
            {
            }

            texture_unit image
            {
                tex_address_mode clamp
                filtering bilinear
            }

            texture_unit transferFunction
            {
                filtering none
            }

            texture_unit entryPoints0
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints1
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints2
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints3
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints4
            {
                tex_address_mode clamp
                filtering none
            }
        }
    }
}

//----------------------------------------------

material RayTracedVolume3D8
{
    technique
    {
        pass
        {
            cull_hardware none
            cull_software none
            scene_blend alpha_blend

            vertex_program_ref RenderQuad_VP
            {
            }

            fragment_program_ref VR/RayTracedVolume3D8_FP
            {
            }

            texture_unit image
            {
                tex_address_mode clamp
                filtering bilinear
            }

            texture_unit transferFunction
            {
                filtering none
            }

            texture_unit entryPoints0
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints1
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints2
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints3
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints4
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints5
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints6
            {
                tex_address_mode clamp
                filtering none
            }

            texture_unit entryPoints7
            {
                tex_address_mode clamp
                filtering none
            }
        }
    }
}