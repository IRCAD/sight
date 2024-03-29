/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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

#include "viz/scene3d/vr/illum_ambient_occlusion_sat.hpp"

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

//-----------------------------------------------------------------------------

namespace sight::viz::scene3d::vr
{

///@brief Internal listener used to update the material according to the parameters.
class vol_illum_compositor_listener : public Ogre::CompositorInstance::Listener
{
public:

    vol_illum_compositor_listener(
        const int& _current_slice_index,
        const illum_ambient_occlusion_sat::sat_parameters_t& _parameters
    ) :
        m_current_slice_index(_current_slice_index),
        m_parameters(_parameters)
    {
    }

    //------------------------------------------------------------------------------

    void notifyMaterialRender(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& _mat) override
    {
        Ogre::Pass* pass                                     = _mat->getTechnique(0)->getPass(0);
        Ogre::GpuProgramParametersSharedPtr vol_illum_params = pass->getFragmentProgramParameters();

        vol_illum_params->setNamedConstant("u_sliceIndex", m_current_slice_index);
    }

    //------------------------------------------------------------------------------

    void notifyMaterialSetup(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& _mat) override
    {
        Ogre::Pass* pass                                     = _mat->getTechnique(0)->getPass(0);
        Ogre::GpuProgramParametersSharedPtr vol_illum_params = pass->getFragmentProgramParameters();

        vol_illum_params->setNamedConstant("u_nbShells", m_parameters.shells);
        vol_illum_params->setNamedConstant("u_shellRadius", m_parameters.radius);

        vol_illum_params->setNamedConstant("u_scatteringConeAngle", m_parameters.angle);
        vol_illum_params->setNamedConstant("u_nbSamplesAlongCone", m_parameters.samples);
    }

private:

    const int& m_current_slice_index;

    const illum_ambient_occlusion_sat::sat_parameters_t& m_parameters;
};

//-----------------------------------------------------------------------------

illum_ambient_occlusion_sat::illum_ambient_occlusion_sat(
    std::string _parent_id,
    Ogre::SceneManager* _scene_manager,
    bool _ao,
    bool _shadows,
    std::optional<sat_parameters_t> _parameters
) :
    m_ao(_ao),
    m_shadows(_shadows),
    m_parameters(_parameters.value_or(sat_parameters_t {})),
    m_parent_id(std::move(_parent_id)),
    m_scene_manager(_scene_manager),
    m_sat(m_parent_id, m_scene_manager, m_parameters.size_ratio)
{
}

//-----------------------------------------------------------------------------

illum_ambient_occlusion_sat::~illum_ambient_occlusion_sat()
{
    if(m_dummy_camera != nullptr)
    {
        m_scene_manager->destroyCamera(m_dummy_camera.get());
    }
}

//-----------------------------------------------------------------------------

void illum_ambient_occlusion_sat::update_sat_from_ratio(float _sat_size_ratio)
{
    m_parameters.size_ratio = _sat_size_ratio;
    m_sat.update_sat_from_ratio(m_parameters.size_ratio);
    update_texture();
}

//-----------------------------------------------------------------------------

void illum_ambient_occlusion_sat::sat_update(
    const texture::sptr& _img,
    const transfer_function::sptr& _tf,
    float _sample_distance
)
{
    m_sat.compute_parallel(_img, _tf, _sample_distance);
    this->update_volume_illumination();
}

//-----------------------------------------------------------------------------

void illum_ambient_occlusion_sat::update_volume_illumination()
{
    // Do this for now but at the end we should use our own texture
    m_illumination_volume = m_sat.get_spare_texture();

    if(m_illumination_volume != nullptr)
    {
        const int depth = static_cast<int>(m_illumination_volume->getDepth());

        //Should never happen, but who knows
        if(depth > 0)
        {
            Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();

            const std::string current_material_name = "VolIllum"
                                                      + std::string(m_ao ? "_AO" : "")
                                                      + std::string(m_shadows ? "_Shadows" : "");

            Ogre::MaterialPtr vol_illum_mtl = Ogre::MaterialManager::getSingleton().getByName(
                current_material_name,
                RESOURCE_GROUP
            );

            if(vol_illum_mtl->getNumTechniques() > 0)
            {
                Ogre::Technique* const technique = vol_illum_mtl->getTechnique(0);
                if(technique->getNumPasses() > 0)
                {
                    Ogre::Pass* const pass                      = technique->getPass(0);
                    Ogre::TextureUnitState* const sat_img_state = pass->getTextureUnitState("sat");

                    if(sat_img_state != nullptr)
                    {
                        sat_img_state->setTexture(m_sat.get_texture());

                        //Update illumination volume slice by slice.
                        for(m_current_slice_index = 0 ; m_current_slice_index < depth ; ++m_current_slice_index)
                        {
                            //The current render target index
                            const auto target = static_cast<size_t>(m_current_slice_index);

                            Ogre::RenderTarget* const rt =
                                m_illumination_volume->getBuffer()->getRenderTarget(target);

                            if(rt->getNumViewports() > 0)
                            {
                                Ogre::Viewport* const vp = rt->getViewport(0);

                                //Temporary listener used to update shaders parameters when the target is updated.s
                                vol_illum_compositor_listener listener(m_current_slice_index, m_parameters);

                                //Add compositor
                                compositor_manager.addCompositor(vp, "VolumeIllumination");
                                compositor_manager.setCompositorEnabled(vp, "VolumeIllumination", true);

                                Ogre::CompositorChain* const comp_chain =
                                    compositor_manager.getCompositorChain(vp);
                                Ogre::CompositorInstance* const comp_instance = comp_chain->getCompositor(
                                    "VolumeIllumination"
                                );

                                //Compositor instance technique
                                auto* const tech                       = comp_instance->getTechnique();
                                const auto& passes                     = tech->getOutputTargetPass()->getPasses();
                                Ogre::CompositionPass* const comp_pass = passes[0];

                                if(comp_pass->getMaterial()->getName() != current_material_name)
                                {
                                    comp_pass->setMaterialName(current_material_name);
                                }

                                //Add the temporary listener
                                comp_instance->addListener(&listener);

                                // Compute the volume illumination
                                rt->update(false);

                                // Remove it
                                comp_instance->removeListener(&listener);

                                compositor_manager.setCompositorEnabled(vp, "VolumeIllumination", false);
                                compositor_manager.removeCompositor(vp, "VolumeIllumination");
                            }
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void illum_ambient_occlusion_sat::update_texture()
{
    Ogre::TexturePtr sat_texture = m_sat.get_spare_texture();

    Ogre::TextureManager& texture_manager = Ogre::TextureManager::getSingleton();

    // Removes the ping pong buffers if they have to be resized
    texture_manager.remove(m_parent_id + BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);

    m_illumination_volume = texture_manager.createManual(
        m_parent_id + BUFFER_NAME,
        viz::scene3d::RESOURCE_GROUP,
        Ogre::TEX_TYPE_3D,
        sat_texture->getWidth(),
        sat_texture->getHeight(),
        sat_texture->getDepth(),
        0,
        Ogre::PF_A8R8G8B8,
        Ogre::TU_RENDERTARGET
    );

    if(m_dummy_camera == nullptr)
    {
        m_dummy_camera.reset(m_scene_manager->createCamera(m_parent_id + "_VolumeIllumination_DummyCamera"));
    }

    const int depth = static_cast<int>(sat_texture->getDepth());
    for(int slice_index = 0 ; slice_index < depth ; ++slice_index)
    {
        // Init source buffer
        Ogre::RenderTarget* render_target =
            m_illumination_volume->getBuffer()->getRenderTarget(static_cast<std::size_t>(slice_index));
        Ogre::Viewport* vp = render_target->addViewport(m_dummy_camera.get());

        vp->setOverlaysEnabled(false);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
