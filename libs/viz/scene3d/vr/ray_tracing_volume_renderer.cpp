/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "viz/scene3d/vr/ray_tracing_volume_renderer.hpp"

#include "viz/scene3d/compositor/core.hpp"
#include "viz/scene3d/compositor/listener/ray_exit_depth.hpp"
#include "viz/scene3d/helper/camera.hpp"
#include "viz/scene3d/helper/shading.hpp"
#include "viz/scene3d/ogre.hpp"
#include "viz/scene3d/render.hpp"
#include "viz/scene3d/utils.hpp"

#include <core/profiling.hpp>

#include <data/helper/medical_image.hpp>

#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <utility>

namespace sight::viz::scene3d::vr
{

//-----------------------------------------------------------------------------

class ray_tracing_volume_renderer::CameraListener : public Ogre::Camera::Listener
{
/// Renderer to which this object listens
const ray_tracing_volume_renderer& m_renderer;

/// The renderer layer
std::weak_ptr<layer> m_layer;

/// Material name used to retrieve the pass
std::string m_currentMtlName;

/// Frame ID.
int m_frameId {0};

public:

    /// Constructor
    CameraListener(const ray_tracing_volume_renderer& _renderer, layer::wptr _layer) :
        m_renderer(_renderer),
        m_layer(std::move(_layer)),
        m_currentMtlName("VolIllum")
    {
    }

    //------------------------------------------------------------------------------

    void cameraPreRenderScene(Ogre::Camera* /*cam*/) override
    {
        if(auto layer = m_layer.lock())
        {
            const int frame_id = layer->getRenderService()->getInteractorManager()->getFrameId();
            if(frame_id != m_frameId)
            {
                if(m_renderer.m_shadows.parameters.soft_shadows)
                {
                    // Set light directions in shader.
                    const Ogre::LightList closest_lights =
                        m_renderer.m_volumeSceneNode->getAttachedObject(0)->queryLights();

                    if(!closest_lights.empty())
                    {
                        Ogre::Vector3 light_dir = m_renderer.m_volumeSceneNode->convertLocalToWorldDirection(
                            closest_lights[0]->getDerivedDirection(),
                            true
                        );

                        const auto& material = Ogre::MaterialManager::getSingleton().getByName(
                            m_currentMtlName,
                            RESOURCE_GROUP
                        );
                        const Ogre::Pass* const sat_illum_pass = material->getTechnique(0)->getPass(0);

                        Ogre::GpuProgramParametersSharedPtr params = sat_illum_pass->getFragmentProgramParameters();

                        params->setNamedConstant("u_lightDir", light_dir);
                    }
                }

                m_frameId = frame_id;
            }
        }
    }

    //------------------------------------------------------------------------------

    void setCurrentMtlName(const std::string& _current_mtl_name)
    {
        m_currentMtlName = _current_mtl_name;
    }
};

//-----------------------------------------------------------------------------

ray_tracing_volume_renderer::ray_tracing_volume_renderer(
    std::string _parent_id,
    layer::sptr _layer,
    Ogre::SceneNode* const _parent_node,
    sight::data::image::csptr _image,
    sight::data::image::csptr _mask,
    sight::data::transfer_function::csptr _tf,
    std::uint16_t _samples,
    bool _buffer,
    bool _preintegration,
    std::optional<shadows_parameters_t> _shadows,
    std::optional<illum_ambient_occlusion_sat::sat_parameters_t> _sat,
    std::optional<std::string> _shader
) :
    volume_renderer(_parent_id,
                    _layer->getSceneManager(),
                    _parent_node,
                    _image,
                    _mask,
                    _tf,
                    _samples,
                    _buffer,
                    _preintegration),
    m_shader(_shader.value_or("RayTracedVolume_FP.glsl")),
    m_shadows({_shadows.value_or(shadows_parameters_t {})}),
    m_layer(_layer),
    m_sat(M_PARENT_ID,
          M_SCENE_MANAGER,
          (m_shadows.parameters.ao.enabled || m_shadows.parameters.colour_bleeding.enabled),
          m_shadows.parameters.ao.enabled || m_shadows.parameters.colour_bleeding.enabled,
          _sat.value_or(illum_ambient_occlusion_sat::sat_parameters_t {}))
{
    //Listeners
    {
        auto* exit_depth_listener = new compositor::listener::ray_exit_depth_listener();
        Ogre::MaterialManager::getSingleton().addListener(exit_depth_listener);
    }

    //Compositor parameters
    const std::uint8_t num_view_points   = m_layer.lock()->numCameras();
    const auto stereo_mode               = _layer->getStereoMode();
    const auto ray_entry_compositor_name = "VolumeEntries" + std::to_string(num_view_points);

    //Compositor for ray entry
    m_rayEntryCompositor = std::make_unique<ray_entry_compositor>(
        ray_entry_compositor_name,
        defines::PROXY_GEOMETRY_RQ_GROUP,
        stereo_mode,
        true
    );

    //Compositor manager
    {
        Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
        auto* const viewport                        = _layer->getViewport();
        auto* const compositor_instance             =
            compositor_manager.addCompositor(viewport, ray_entry_compositor_name, 0);
        SIGHT_ERROR_IF("Compositor '" + ray_entry_compositor_name + "' not found.", compositor_instance == nullptr);
        compositor_instance->setEnabled(true);
    }

    //Shader parameters
    {
        const std::string rtv_shared_params_name = _parent_id + "_RTVParams";

        // First check that we did not already instance Shared parameters
        // This can happen when reinstancing this class (e.g. switching 3D mode)
        Ogre::GpuProgramManager::SharedParametersMap sp_map =
            Ogre::GpuProgramManager::getSingleton().getAvailableSharedParameters();
        if(!sp_map[rtv_shared_params_name])
        {
            m_RTVSharedParameters = Ogre::GpuProgramManager::getSingleton().createSharedParameters(
                rtv_shared_params_name
            );
        }
        else
        {
            m_RTVSharedParameters = sp_map[rtv_shared_params_name];
        }

        // define the shared param structure
        m_RTVSharedParameters->addConstantDefinition("u_f3TFWindow", Ogre::GCT_FLOAT3);
        m_RTVSharedParameters->addConstantDefinition("u_fSampleDis_Ms", Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_f4VolIllumFactor", Ogre::GCT_FLOAT4);
        m_RTVSharedParameters->addConstantDefinition("u_f3VolumeClippingBoxMinPos_Ms", Ogre::GCT_FLOAT3);
        m_RTVSharedParameters->addConstantDefinition("u_f3VolumeClippingBoxMaxPos_Ms", Ogre::GCT_FLOAT3);
        m_RTVSharedParameters->addConstantDefinition("u_iMinImageValue", Ogre::GCT_INT1);
        m_RTVSharedParameters->addConstantDefinition("u_iMaxImageValue", Ogre::GCT_INT1);
        m_RTVSharedParameters->addConstantDefinition("u_fOpacityCorrectionFactor", Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_window", Ogre::GCT_FLOAT2);
        m_RTVSharedParameters->setNamedConstant("u_fOpacityCorrectionFactor", m_opacityCorrectionFactor);
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::update(const data::transfer_function::csptr& _tf)
{
    initEntryPoints(); //Does nothing after the first call
    updateRayTracingMaterial();
    setSampling(m_nbSlices, _tf);
}

//-----------------------------------------------------------------------------

ray_tracing_volume_renderer::~ray_tracing_volume_renderer()
{
    if(m_camera != nullptr)
    {
        m_camera->removeListener(m_cameraListener.get());
        m_cameraListener.reset();
    }

    //Notes:
    //- Cannot use smart pointers otherwise the destructor is called twice (which is bad)

    if(m_entryPointGeometry != nullptr)
    {
        m_volumeSceneNode->detachObject(m_entryPointGeometry);
        M_SCENE_MANAGER->destroyManualObject(m_entryPointGeometry);
        m_entryPointGeometry = nullptr;
    }

    if(m_proxyGeometry != nullptr)
    {
        m_volumeSceneNode->detachObject(m_proxyGeometry);
        M_SCENE_MANAGER->destroyMovableObject(m_proxyGeometry);
        m_proxyGeometry = nullptr;
    }

    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
    auto* const viewport                        = m_layer.lock()->getViewport();

    const auto& ray_entry_compositor_name = m_rayEntryCompositor->getName();
    compositor_manager.setCompositorEnabled(viewport, ray_entry_compositor_name, false);
    compositor_manager.removeCompositor(viewport, ray_entry_compositor_name);

    m_RTVSharedParameters->removeAllConstantDefinitions();

    // FIXME_DW: Doesn't seem to be a resource any longer
//    Ogre::GpuProgramManager::getSingleton().remove(m_RTVSharedParameters->getName(), RESOURCE_GROUP);

    Ogre::MaterialManager::getSingleton().remove(m_currentMtlName, RESOURCE_GROUP);
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateImage(const data::image::csptr _image, const data::transfer_function::csptr _tf)
{
    if(!data::helper::medical_image::check_image_validity(_image))
    {
        return;
    }

    this->scaleTranslateCube(_image->getSpacing(), _image->getOrigin());

    const data::image::Size& new_size = _image->size();

    // Create new grid texture + proxy geometry if image size changed.
    if(m_imageSize != new_size)
    {
        m_imageSize = new_size;

        m_proxyGeometry->updateGridSize();
    }
    else
    {
        m_proxyGeometry->computeGrid();
    }

    const auto material = Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName, RESOURCE_GROUP);
    if(m_preintegration)
    {
        m_preIntegrationTable.imageUpdate(_image, _tf, m_sampleDistance);

        // After having updated the preintegrated transfer function
        // We update the corresponding shader parameters
        const auto min_max = m_preIntegrationTable.get_min_max();

        m_RTVSharedParameters->setNamedConstant("u_iMinImageValue", min_max.first);
        m_RTVSharedParameters->setNamedConstant("u_iMaxImageValue", min_max.second);
    }
    else
    {
        const auto* const technique = material->getTechnique(0);
        SIGHT_ASSERT("technique not found", technique);
        const auto* const pass = technique->getPass(0);
        SIGHT_ASSERT("Pass not found", pass);
        m_gpuVolumeTF->bind(pass, defines::VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
    }

    m_RTVSharedParameters->setNamedConstant("u_window", m_3DOgreTexture->window());

    // The depth technique always used the transfer function
    const auto* const technique = material->getTechnique(1);
    SIGHT_ASSERT("technique not found", technique);
    const auto* const pass = technique->getPass(0);
    m_gpuVolumeTF->bind(pass, defines::VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateMask(const data::image::csptr _mask)
{
    if(!data::helper::medical_image::check_image_validity(_mask))
    {
        return;
    }

    this->loadMask();

    m_proxyGeometry->computeGrid();
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateSATSizeRatio(unsigned _ratio)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.updateSatFromRatio(static_cast<float>(_ratio));
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateSATShellsNumber(unsigned _count)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.setNbShells(_count);
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateSATShellRadius(unsigned _radius)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.setShellRadius(_radius);
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateSATConeAngle(float _angle)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.setConeAngle(_angle);
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateSATConeSamples(unsigned _samples)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.setSamplesAlongCone(_samples);
        updateSAT();
    }
}

//------------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateSAT()
{
    m_sat.SATUpdate(m_3DOgreTexture, m_gpuVolumeTF, m_sampleDistance);
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateVolumeTF(const data::transfer_function::csptr& _tf)
{
    //Update the attributes
    {
        m_gpuVolumeTF->update();
    }

    if(m_preintegration)
    {
        m_preIntegrationTable.tfUpdate(_tf, m_sampleDistance);
    }

    const auto material = Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName, RESOURCE_GROUP);

    if(!m_preintegration)
    {
        const auto* const technique = material->getTechnique(0);
        SIGHT_ASSERT("technique not found", technique);
        const auto* const pass = technique->getPass(0);
        SIGHT_ASSERT("Pass not found", pass);
        m_gpuVolumeTF->bind(pass, defines::VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
    }

    // The depth technique always uses the transfer function
    const auto* const technique = material->getTechnique(1);
    SIGHT_ASSERT("technique not found", technique);

    const auto* const pass = technique->getPass(0);
    SIGHT_ASSERT("Pass not found", pass);
    m_gpuVolumeTF->bind(pass, defines::VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);

    m_proxyGeometry->computeGrid();

    const auto& shadows = m_shadows.parameters;

    if(shadows.ao.enabled || shadows.colour_bleeding.enabled || shadows.soft_shadows)
    {
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::setSampling(std::uint16_t _nb_samples, const data::transfer_function::csptr& _tf)
{
    if(_nb_samples == 0)
    {
        throw std::invalid_argument("Sample count cannot be 0.");
    }

    m_nbSlices = _nb_samples;

    updateSampleDistance();

    // Update the sample distance in the shaders
    m_RTVSharedParameters->setNamedConstant("u_fSampleDis_Ms", m_sampleDistance);
    if(m_shadows.parameters.enabled())
    {
        updateSAT();
    }

    if(m_preintegration)
    {
        m_preIntegrationTable.tfUpdate(_tf, m_sampleDistance);
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::setOpacityCorrection(int _opacity_correction)
{
    m_opacityCorrectionFactor = static_cast<float>(_opacity_correction);

    // Update shader parameter
    m_RTVSharedParameters->setNamedConstant("u_fOpacityCorrectionFactor", m_opacityCorrectionFactor);
}

//-----------------------------------------------------------------------------

bool ray_tracing_volume_renderer::setAOFactor(float _factor)
{
    m_shadows.parameters.ao.factor = _factor;

    m_shadows.factors =
    {
        m_shadows.parameters.colour_bleeding.r,
        m_shadows.parameters.colour_bleeding.g,
        m_shadows.parameters.colour_bleeding.b,
        m_shadows.parameters.ao.factor
    };

    // Update the shader parameter
    m_RTVSharedParameters->setNamedConstant("u_f4VolIllumFactor", m_shadows.factors);

    return m_shadows.parameters.ao.enabled;
}

//-----------------------------------------------------------------------------

bool ray_tracing_volume_renderer::setColorBleedingFactor(float _factor)
{
    return setColorBleedingFactor({_factor, _factor, _factor});
}

//------------------------------------------------------------------------------

bool ray_tracing_volume_renderer::setColorBleedingFactor(const Ogre::Vector3& _factors)
{
    //Update the parameters
    {
        m_shadows.parameters.colour_bleeding.r = _factors.x;
        m_shadows.parameters.colour_bleeding.g = _factors.x;
        m_shadows.parameters.colour_bleeding.b = _factors.x;
    }

    m_shadows.factors =
    {
        m_shadows.parameters.colour_bleeding.r,
        m_shadows.parameters.colour_bleeding.g,
        m_shadows.parameters.colour_bleeding.b,
        m_shadows.parameters.ao.factor
    };

    // Update the shader parameter
    m_RTVSharedParameters->setNamedConstant("u_f4VolIllumFactor", m_shadows.factors);

    return m_shadows.parameters.colour_bleeding.enabled;
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::toggleAmbientOcclusion(bool _enable)
{
    if(_enable)
    {
        enableAmbientOcclusion();
    }
    else
    {
        disableAmbientOcclusion();
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::enableAmbientOcclusion()
{
    if(!m_shadows.parameters.ao.enabled)
    {
        m_shadows.parameters.ao.enabled = true;
        m_sat.setAO(true);

        this->updateSAT();
        this->updateRayTracingMaterial();
        this->updateVolumeIlluminationMaterial();
    }
}

//------------------------------------------------------------------------------

void ray_tracing_volume_renderer::disableAmbientOcclusion()
{
    if(m_shadows.parameters.ao.enabled) //Don't reallocate the material if it makes no sense
    {
        m_shadows.parameters.ao.enabled = false;
        m_sat.setAO(false);

        this->updateRayTracingMaterial();
        this->updateVolumeIlluminationMaterial();
        this->updateSAT();
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::setPreIntegratedRendering(bool _pre_integrated_rendering)
{
    m_preintegration = _pre_integrated_rendering;

    this->updateRayTracingMaterial();
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::toggleColorBleeding(bool _enable)
{
    m_shadows.parameters.colour_bleeding.enabled = _enable;

    this->updateRayTracingMaterial();
    this->updateVolumeIlluminationMaterial();
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::toggleShadows(bool _enable)
{
    m_shadows.parameters.soft_shadows = _enable;
    m_sat.setShadows(_enable);

    this->updateSAT();
    this->updateRayTracingMaterial();
    this->updateVolumeIlluminationMaterial();
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::setFocalLength(float _focal_length)
{
    m_focalLength = _focal_length;
    computeRealFocalLength();
}

//------------------------------------------------------------------------------

void ray_tracing_volume_renderer::clipImage(const Ogre::AxisAlignedBox& _clipping_box)
{
    const Ogre::AxisAlignedBox max_box_size(Ogre::Vector3::ZERO, Ogre::Vector3(1.F, 1.F, 1.F));
    const Ogre::AxisAlignedBox clamped_clipping_box = max_box_size.intersection(_clipping_box);

    volume_renderer::clipImage(clamped_clipping_box);

    if(m_entryPointGeometry == nullptr) //Prevent misusage
    {
        initEntryPoints();
    }

    m_entryPointGeometry->beginUpdate(0);
    {
        for(const auto& face : s_cubeFaces)
        {
            const CubeFacePositionList& face_position_list = face.second;

            m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[0]]);
            m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[1]]);
            m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[2]]);
            m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[2]]);
            m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[3]]);
            m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[0]]);
        }
    }
    m_entryPointGeometry->end();

    m_proxyGeometry->clipGrid(clamped_clipping_box);

    m_RTVSharedParameters->setNamedConstant("u_f3VolumeClippingBoxMinPos_Ms", clamped_clipping_box.getMinimum());
    m_RTVSharedParameters->setNamedConstant("u_f3VolumeClippingBoxMaxPos_Ms", clamped_clipping_box.getMaximum());
}

//-----------------------------------------------------------------------------

bool ray_tracing_volume_renderer::isVisible() const
{
    return m_entryPointGeometry->isVisible() && m_proxyGeometry->isVisible();
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::setRayCastingPassTextureUnits(Ogre::Pass* const _ray_casting_pass) const
{
    //Fragment shaders parameters
    Ogre::GpuProgramParametersSharedPtr fp_params = _ray_casting_pass->getFragmentProgramParameters();

    // The argument passes to setNamedConstant. Increased each call.
    int num_tex_unit = 0;

    // Volume data
    {
        Ogre::TextureUnitState* const tex_unit_state = _ray_casting_pass->createTextureUnitState();
        m_3DOgreTexture->bind(tex_unit_state, Ogre::TEX_TYPE_3D, Ogre::TFO_BILINEAR, Ogre::TextureUnitState::TAM_CLAMP);
    }
    fp_params->setNamedConstant("u_s3Image", num_tex_unit++);

    // Crop mask
    {
        Ogre::TextureUnitState* const tex_unit_state = _ray_casting_pass->createTextureUnitState();
        m_maskTexture->bind(tex_unit_state, Ogre::TEX_TYPE_3D, Ogre::TFO_BILINEAR, Ogre::TextureUnitState::TAM_CLAMP);
    }
    fp_params->setNamedConstant("u_s1Mask", num_tex_unit++);

    // Transfer function
    if(m_options.fragment.find(defines::PREINTEGRATION) != std::string::npos)
    {
        Ogre::TextureUnitState* const tex_unit_state =
            _ray_casting_pass->createTextureUnitState(m_preIntegrationTable.getTexture()->getName());
        tex_unit_state->setTextureFiltering(Ogre::TFO_BILINEAR);
        fp_params->setNamedConstant("u_s2PreintegratedTFTexture", num_tex_unit++);
    }
    else
    {
        Ogre::TextureUnitState* const tex_unit_state = _ray_casting_pass->createTextureUnitState();
        tex_unit_state->setName(defines::VOLUME_TF_TEXUNIT_NAME);
        tex_unit_state->setTextureFiltering(Ogre::TFO_BILINEAR);
        tex_unit_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        m_gpuVolumeTF->bind(_ray_casting_pass, tex_unit_state->getName(), fp_params);
        fp_params->setNamedConstant("u_s2TFTexture", num_tex_unit++);
    }

    if(m_options.fragment.find(defines::AO) != std::string::npos) //i.e. ambient occlusion is enabled
    {
        Ogre::TextureUnitState* const tex_unit_state = _ray_casting_pass->createTextureUnitState();
        tex_unit_state->setTextureFiltering(Ogre::TFO_BILINEAR);
        tex_unit_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        tex_unit_state->setTexture(m_sat.getIlluminationVolume());

        // Update the shader parameter
        fp_params->setNamedConstant("u_s3IlluminationVolume", num_tex_unit++);
        m_RTVSharedParameters->setNamedConstant("u_f4VolIllumFactor", m_shadows.factors);
    }

    // Entry points texture
    Ogre::TextureUnitState* const tex_unit_state = _ray_casting_pass->createTextureUnitState();
    tex_unit_state->setName("entryPoints");

    if(m_layer.lock()->numCameras() == 1)
    {
        const auto& ray_entry_compositor_name = m_rayEntryCompositor->getName();
        tex_unit_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        tex_unit_state->setCompositorReference(ray_entry_compositor_name, ray_entry_compositor_name + "texture");
    }

    tex_unit_state->setTextureFiltering(Ogre::TFO_NONE);
    tex_unit_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    fp_params->setNamedConstant("u_s2EntryPoints", num_tex_unit++);
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateRayTracingMaterial()
{
    //Update the current options
    updateOptions();

    m_currentMtlName = "RTV_Mat_" + M_PARENT_ID;

    Ogre::MaterialManager& material_manager = Ogre::MaterialManager::getSingleton();

    // The material needs to be destroyed only if it already exists
    {
        const Ogre::ResourcePtr mat_resource = material_manager.getResourceByName(m_currentMtlName, RESOURCE_GROUP);
        if(mat_resource)
        {
            material_manager.remove(mat_resource);
            // Our manual object still references the material and uses the material name to know if it should modify
            // its pointer (see manual_object::ManualObjectSection::setMaterialName() in OgreManualObject.cpp)
            // So we just force it to release the pointer otherwise the material is not destroyed
            if(m_entryPointGeometry != nullptr)
            {
                m_entryPointGeometry->setMaterialName(0, "");
            }
        }
    }

    Ogre::HighLevelGpuProgramManager& gpm = Ogre::HighLevelGpuProgramManager::getSingleton();
    const Ogre::String vp_name("RTV_VP_" + std::to_string(m_options.hash));
    const Ogre::String fp_name("RTV_FP_" + std::to_string(m_options.hash));

    // Compile the common vertex shader
    {
        if(!gpm.resourceExists(vp_name, RESOURCE_GROUP))
        {
            Ogre::HighLevelGpuProgramPtr vsp = gpm.createProgram(
                vp_name,
                RESOURCE_GROUP,
                "glsl",
                Ogre::GPT_VERTEX_PROGRAM
            );
            vsp->setSourceFile("RayTracedVolume_VP.glsl");

            if(!m_options.vertex.empty())
            {
                vsp->setParameter("preprocessor_defines", m_options.vertex);
            }
        }
    }

    // Compile fragment shader
    {
        if(!gpm.resourceExists(fp_name, RESOURCE_GROUP))
        {
            Ogre::HighLevelGpuProgramPtr fsp =
                gpm.createProgram(fp_name, RESOURCE_GROUP, "glsl", Ogre::GPT_FRAGMENT_PROGRAM);
            fsp->setSourceFile(m_shader);

            if(!m_options.fragment.empty())
            {
                fsp->setParameter("preprocessor_defines", m_options.fragment);
            }
        }
    }

    // Create the material
    const Ogre::MaterialPtr mat = material_manager.create(m_currentMtlName, viz::scene3d::RESOURCE_GROUP);

    // Create the technique
    {
        // Ensure that we have the color parameters set for the current material
        this->setMaterialLightParams(mat);
        // Get the already created pass through the already created technique
        const Ogre::Technique* const tech = mat->getTechnique(0);

        Ogre::Pass* const pass = tech->getPass(0);
        pass->setCullingMode(Ogre::CULL_ANTICLOCKWISE);
        pass->setSeparateSceneBlendingOperation(Ogre::SBO_ADD, Ogre::SBO_ADD);
        pass->setSeparateSceneBlending(
            Ogre::SBF_SOURCE_ALPHA,
            Ogre::SBF_ONE_MINUS_SOURCE_ALPHA,
            Ogre::SBF_ONE,
            Ogre::SBF_ONE_MINUS_SOURCE_ALPHA
        );
        pass->setDepthCheckEnabled(true);
        pass->setDepthWriteEnabled(true);

        // Vertex program
        pass->setVertexProgram(vp_name);
        Ogre::GpuProgramParametersSharedPtr vp_params = pass->getVertexProgramParameters();
        vp_params->setNamedAutoConstant("u_worldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

        // Fragment program
        pass->setFragmentProgram(fp_name);
        Ogre::GpuProgramParametersSharedPtr fp_params = pass->getFragmentProgramParameters();
        fp_params->setNamedAutoConstant("u_viewportSize", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
        fp_params->setNamedAutoConstant("u_clippingNearDis", Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
        fp_params->setNamedAutoConstant("u_clippingFarDis", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
        fp_params->setNamedAutoConstant("u_f3CameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
        fp_params->setNamedAutoConstant("u_fShininess", Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
        fp_params->setNamedAutoConstant("u_fNumLights", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
        fp_params->setNamedAutoConstant(
            "u_f4LightPos",
            Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY,
            8
        );
        fp_params->setNamedAutoConstant(
            "u_f3LightDiffuseCol",
            Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_ARRAY,
            8
        );
        fp_params->setNamedAutoConstant(
            "u_f3LightSpecularCol",
            Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR_ARRAY,
            8
        );
        fp_params->setNamedAutoConstant(
            "u_invWorldViewProj",
            Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEWPROJ_MATRIX
        );
        fp_params->setNamedAutoConstant("u_worldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
        fp_params->addSharedParameters(m_RTVSharedParameters->getName());

        // Setup texture unit states
        this->setRayCastingPassTextureUnits(pass);
    }

    // Compile the depth fragment shader
    const Ogre::String fp_depth_name("RTVD_FP_" + std::to_string(m_options.hash));
    if(!gpm.resourceExists(fp_depth_name, RESOURCE_GROUP))
    {
        Ogre::HighLevelGpuProgramPtr fsp = gpm.createProgram(
            fp_depth_name,
            RESOURCE_GROUP,
            "glsl",
            Ogre::GPT_FRAGMENT_PROGRAM
        );
        fsp->setSourceFile("RayTracedVolumeDepth_FP.glsl");
        fsp->setParameter("attach", "DepthPeelingCommon_FP");

        if(!m_options.fragment.empty())
        {
            fsp->setParameter("preprocessor_defines", m_options.fragment);
        }
    }

    // Create the depth technique
    {
        using program_parameters_t = Ogre::GpuProgramParameters;

        Ogre::Technique* const tech = mat->createTechnique();

        //technique scheme name
        {
            tech->setSchemeName("DepthPeeling/depthMap");
        }

        Ogre::Pass* const pass = tech->createPass();

        //Culling and depth
        {
            pass->setCullingMode(Ogre::CULL_ANTICLOCKWISE);
            pass->setDepthCheckEnabled(true);
            pass->setDepthWriteEnabled(true);
        }

        // Vertex program
        {
            pass->setVertexProgram(vp_name);
            Ogre::GpuProgramParametersSharedPtr vp_params = pass->getVertexProgramParameters();
            vp_params->setNamedAutoConstant("u_worldViewProj", program_parameters_t::ACT_WORLDVIEWPROJ_MATRIX);
        }

        // Fragment program
        {
            pass->setFragmentProgram(fp_depth_name);
            Ogre::GpuProgramParametersSharedPtr fp_params = pass->getFragmentProgramParameters();
            fp_params->setNamedAutoConstant("u_viewportSize", program_parameters_t::ACT_VIEWPORT_SIZE);
            fp_params->setNamedAutoConstant("u_clippingNearDis", program_parameters_t::ACT_NEAR_CLIP_DISTANCE);
            fp_params->setNamedAutoConstant("u_clippingFarDis", program_parameters_t::ACT_FAR_CLIP_DISTANCE);
            fp_params->setNamedAutoConstant(
                "u_invWorldViewProj",
                program_parameters_t::ACT_INVERSE_WORLDVIEWPROJ_MATRIX
            );
            fp_params->setNamedAutoConstant("u_worldViewProj", program_parameters_t::ACT_WORLDVIEWPROJ_MATRIX);
            fp_params->addSharedParameters(m_RTVSharedParameters->getName());
        }

        //texture unit state updates
        {
            //The argument passes to setNamedConstant. Increased each call.
            int num_tex_unit = 0;

            Ogre::GpuProgramParametersSharedPtr fp_params = pass->getFragmentProgramParameters();
            //Input texture
            {
                Ogre::TextureUnitState* const tex_unit_state = pass->createTextureUnitState();
                SIGHT_ASSERT("texUnitState is nullptr.", tex_unit_state != nullptr);

                m_3DOgreTexture->bind(
                    tex_unit_state,
                    Ogre::TEX_TYPE_3D,
                    Ogre::TFO_BILINEAR,
                    Ogre::TextureUnitState::TAM_CLAMP
                );
                fp_params->setNamedConstant("u_s3Image", num_tex_unit++);
            }

            // Crop mask
            {
                Ogre::TextureUnitState* const tex_unit_state = pass->createTextureUnitState();
                m_maskTexture->bind(
                    tex_unit_state,
                    Ogre::TEX_TYPE_3D,
                    Ogre::TFO_BILINEAR,
                    Ogre::TextureUnitState::TAM_CLAMP
                );
            }
            fp_params->setNamedConstant("u_s1Mask", num_tex_unit++);

            //Transfer function and compositor
            {
                //GPU TF binding
                {
                    Ogre::TextureUnitState* const tex_unit_state = pass->createTextureUnitState();
                    tex_unit_state->setName(defines::VOLUME_TF_TEXUNIT_NAME);
                    tex_unit_state->setTextureFiltering(Ogre::TFO_BILINEAR);
                    tex_unit_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                    m_gpuVolumeTF->bind(pass, tex_unit_state->getName(), fp_params);
                    fp_params->setNamedConstant("u_s2TFTexture", num_tex_unit++);
                }

                //Ray compositor
                {
                    Ogre::TextureUnitState* const tex_unit_state = pass->createTextureUnitState();
                    tex_unit_state->setName("entryPoints");

                    if(m_layer.lock()->numCameras() == 1)
                    {
                        const auto& ray_entry_compositor_name = m_rayEntryCompositor->getName();
                        tex_unit_state->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                        tex_unit_state->setCompositorReference(
                            ray_entry_compositor_name,
                            ray_entry_compositor_name + "texture"
                        );
                    }

                    tex_unit_state->setTextureFiltering(Ogre::TFO_NONE);
                    tex_unit_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                    fp_params->setNamedConstant("u_s2EntryPoints", num_tex_unit++);
                }
            }
        }
    }

    //3D texture to render
    {
        Ogre::MaterialManager& mm          = Ogre::MaterialManager::getSingleton();
        Ogre::MaterialPtr current_material = mm.getByName(m_currentMtlName, RESOURCE_GROUP);
        SIGHT_ASSERT("Missing material '" + m_currentMtlName + "'.", current_material);
        const Ogre::Technique* const tech = current_material->getTechnique(0);
        SIGHT_ASSERT("Material '" + m_currentMtlName + "' has no techniques.", tech);
        Ogre::Pass* const pass = tech->getPass(0);
        SIGHT_ASSERT("Material '" + m_currentMtlName + "' has no passes.", pass);

        Ogre::TextureUnitState* const tex_unit_state = pass->getTextureUnitState(0);
        SIGHT_ASSERT("Material '" + m_currentMtlName + "' has no texture units.", tex_unit_state);
        tex_unit_state->setTextureName(m_3DOgreTexture->name(), Ogre::TEX_TYPE_3D);

        m_proxyGeometry->set3DImageTexture(m_3DOgreTexture);
        m_proxyGeometry->setMaskTexture(m_maskTexture);
    }

    if(static_cast<bool>(m_entryPointGeometry))
    {
        m_entryPointGeometry->setMaterialName(0, m_currentMtlName, RESOURCE_GROUP);
    }
}

//------------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateOptions()
{
    std::ostringstream vp_pp_defs;
    std::ostringstream fp_pp_defs;

    vp_pp_defs.str("");
    fp_pp_defs.str("");

    if(m_shadows.parameters.ao.enabled)
    {
        fp_pp_defs << (fp_pp_defs.str().empty() ? "" : ",") << defines::AO;
    }

    if(m_shadows.parameters.colour_bleeding.enabled)
    {
        fp_pp_defs << (fp_pp_defs.str().empty() ? "" : ",") << defines::COLOR_BLEEDING;
    }

    if(m_shadows.parameters.soft_shadows)
    {
        fp_pp_defs << (fp_pp_defs.str().empty() ? "" : ",") << defines::SHADOWS;
    }

    if(m_preintegration)
    {
        fp_pp_defs << (fp_pp_defs.str().empty() ? "" : ",") << defines::PREINTEGRATION;
    }

    const std::string vertex   = vp_pp_defs.str();
    const std::string fragment = fp_pp_defs.str();

    m_options = options_t(vertex, fragment);
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::initEntryPoints()
{
    //Entry point geometry
    if(m_entryPointGeometry == nullptr)
    {
        m_entryPointGeometry = M_SCENE_MANAGER->createManualObject(M_PARENT_ID + "_RayTracingVREntryPoints");

        {
            // Use the default material before the raytracing material is created otherwise we get an error.
            m_entryPointGeometry->begin("Default", Ogre::RenderOperation::OT_TRIANGLE_LIST, RESOURCE_GROUP);
            for(const auto& face : s_cubeFaces)
            {
                const CubeFacePositionList& face_position_list = face.second;

                m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[0]]);
                m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[1]]);
                m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[2]]);
                m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[2]]);
                m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[3]]);
                m_entryPointGeometry->position(m_clippedImagePositions[face_position_list[0]]);
            }

            m_entryPointGeometry->end();
        }

        // Render volumes after surfaces.
        m_entryPointGeometry->setRenderQueueGroup(rq::s_VOLUME_ID);

        m_volumeSceneNode->attachObject(m_entryPointGeometry);
    }

    //Proxy geometry
    if(m_proxyGeometry == nullptr)
    {
        m_proxyGeometry =
            viz::scene3d::vr::grid_proxy_geometry::make(
                this->M_PARENT_ID + "_GridProxyGeometry",
                M_SCENE_MANAGER,
                m_3DOgreTexture,
                m_maskTexture,
                m_gpuVolumeTF,
                "RayEntryPoints"
            );

        m_proxyGeometry->setRenderQueueGroup(defines::PROXY_GEOMETRY_RQ_GROUP);

        m_volumeSceneNode->attachObject(m_proxyGeometry);
    }

    //Camera listener
    if(m_cameraListener == nullptr)
    {
        m_cameraListener = std::make_unique<CameraListener>(*this, m_layer);
        m_camera->addListener(m_cameraListener.get());
    }
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::computeRealFocalLength()
{
    const Ogre::Plane camera_plane(m_camera->getRealDirection(), m_camera->getRealPosition());
    const auto camera_dist_comparator = [&camera_plane](const Ogre::Vector3& _v1, const Ogre::Vector3& _v2)
                                        {return camera_plane.getDistance(_v1) < camera_plane.getDistance(_v2);};

    const auto closest_furthest_img_points =
        std::minmax_element(m_clippedImagePositions.begin(), m_clippedImagePositions.end(), camera_dist_comparator);

    const auto focus_point = *closest_furthest_img_points.first + m_focalLength
                             * (*closest_furthest_img_points.second - *closest_furthest_img_points.first);

    const float real_focal_length = m_camera->getRealPosition().distance(focus_point);

    m_camera->setFocalLength(real_focal_length);
}

//-----------------------------------------------------------------------------

void ray_tracing_volume_renderer::updateVolumeIlluminationMaterial()
{
    std::string vol_illum_mtl = "VolIllum";

    vol_illum_mtl += m_shadows.parameters.ao.enabled || m_shadows.parameters.colour_bleeding.enabled ? "_AO" : "";
    vol_illum_mtl += m_shadows.parameters.soft_shadows ? "_Shadows" : "";

    SIGHT_ASSERT("Camera listener not instantiated", m_cameraListener);
    m_cameraListener->setCurrentMtlName(vol_illum_mtl);
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
