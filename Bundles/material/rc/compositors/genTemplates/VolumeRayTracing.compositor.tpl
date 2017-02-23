//---------------------------------------------------------------------------
// File generated with genRayTracedMaterials.py
//
// DO NOT EDIT MANUALLY !!!
//
//---------------------------------------------------------------------------

{% for name, defines, params in configsFP %}
{% if not 'MImP_AImC' in name and
	  not 'MImP_VPImC' in name and
	  not 'AImC_VPImC' in name %}

//----------------------------------------------

compositor RayTracedVolume{{ name }}_Comp
{
	technique
	{
{% if 'IDVR=1' in defines %}
        texture_ref mrt_IC IDVR_MImP_Comp mrt_IC
        texture_ref JFA JFAInit JFAFinal
{% endif %}
{% if 'IDVR=2' in defines %}
		texture_ref IC IDVR_AImC_Comp IC
{% endif %}
{% if 'IDVR=3' in defines %}
		texture_ref IC IDVR_VPImC_Comp IC
{% endif %}
	
		// Final pass
		target_output
		{
			input none
			pass clear
			{
				buffers depth
			}

			pass render_quad
			{
				material RayTracedVolume{{ name }}
{% if 'IDVR=1' in defines %}
                input 3 mrt_IC 0
                input 4 JFA
{% elif 'IDVR' in defines %}
                input 3 IC
{% endif %}
			}
		}
	}
}
{% endif %}
{% endfor %}

//----------------------------------------------

compositor RayTracedVolume3D5
{
    technique
    {
        // Final pass
        target_output
        {
            input previous
            pass clear
            {
                buffers depth
            }

            pass render_quad
            {
                material RayTracedVolume3D5
            }
        }
    }
}

//----------------------------------------------

compositor RayTracedVolume3D8
{
    technique
    {
        // Final pass
        target_output
        {
            input previous
            pass clear
            {
                buffers depth
            }

            pass render_quad
            {
                material RayTracedVolume3D8
            }
        }
    }
}