/************************************************************************
 *
 * Copyright (C) 2016-2026 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "viz/scene3d/vr/summed_area_table.hpp"

#include <utility>
#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreDepthBuffer.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreRenderSystem.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <cmath>

namespace sight::viz::scene3d::vr
{

/// Listener dedicated to watching the initialisation.
class summed_area_table::summed_area_table_init_compositor_listener : public Ogre::CompositorInstance::Listener
{
public:

    explicit summed_area_table_init_compositor_listener(float& _current_slice_index) :
        m_current_slice_depth(_current_slice_index)
    {
    }

    //------------------------------------------------------------------------------

    void notifyMaterialRender(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& _mat) override
    {
        if(_mat->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = _mat->getTechnique(0);

            if(technique->getNumPasses() > 0)
            {
                Ogre::Pass* const sat_init_pass                      = technique->getPass(0);
                Ogre::GpuProgramParametersSharedPtr init_pass_params = sat_init_pass->getFragmentProgramParameters();

                init_pass_params->setNamedConstant("u_sliceDepth", m_current_slice_depth);
            }
        }
    }

private:

    float& m_current_slice_depth;
};

/// Listener dedicated to the compositor.
class summed_area_table::summed_area_table_compositor_listener : public Ogre::CompositorInstance::Listener
{
public:

    summed_area_table_compositor_listener(
        int& _read_offset,
        int& _pass_orientation,
        std::size_t& _current_slice_index
    ) :
        m_read_offset(_read_offset),
        m_pass_orientation(_pass_orientation),
        m_current_slice_index(_current_slice_index)
    {
    }

    //------------------------------------------------------------------------------

    void notifyMaterialRender(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& _mat) override
    {
        if(_mat->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = _mat->getTechnique(0);

            if(technique != nullptr)
            {
                if(technique->getNumPasses() > 0)
                {
                    Ogre::Pass* const sat_pass = technique->getPass(0);

                    if(sat_pass != nullptr)
                    {
                        Ogre::GpuProgramParametersSharedPtr sat_pass_params = sat_pass->getFragmentProgramParameters();

                        sat_pass_params->setNamedConstant("u_readOffset", m_read_offset);
                        sat_pass_params->setNamedConstant("u_passOrientation", m_pass_orientation);
                        sat_pass_params->setNamedConstant("u_sliceIndex", static_cast<int>(m_current_slice_index));
                    }
                }
            }
        }
    }

private:

    int& m_read_offset;

    int& m_pass_orientation;

    std::size_t& m_current_slice_index;
};

//-----------------------------------------------------------------------------

summed_area_table::summed_area_table(std::string _parent_id, Ogre::SceneManager* _scene_manager) :
    m_sat_size({0, 0, 0}),
    m_parent_id(std::move(_parent_id)),
    m_scene_manager(_scene_manager)
{
    auto& material_manager = Ogre::MaterialManager::getSingleton();
    m_init_material = material_manager.getByName("summed_area_table_init", RESOURCE_GROUP)->clone(
        m_parent_id + "_summed_area_table_init"
    );
    m_table_material = material_manager.getByName("summed_area_table", RESOURCE_GROUP)->clone(
        m_parent_id + "_summed_area_table"
    );
}

//-----------------------------------------------------------------------------

summed_area_table::~summed_area_table()
{
    Ogre::TextureManager& texture_manager = Ogre::TextureManager::getSingleton();

    if(m_table_material != nullptr)
    {
        Ogre::MaterialManager::getSingleton().remove(m_table_material);
        m_table_material.reset();
    }

    if(m_init_material != nullptr)
    {
        Ogre::MaterialManager::getSingleton().remove(m_init_material);
        m_init_material.reset();
    }

    if(m_source_buffer != nullptr)
    {
        m_source_buffer->unload();
        texture_manager.remove(m_source_buffer);
    }

    if(m_target_buffer != nullptr)
    {
        m_target_buffer->unload();
        texture_manager.remove(m_target_buffer);
    }

    if(m_dummy_camera != nullptr)
    {
        m_scene_manager->destroyCamera(m_dummy_camera);
    }

    if(m_init_material != nullptr)
    {
        Ogre::MaterialManager::getSingleton().remove(m_init_material);
    }

    if(m_table_material != nullptr)
    {
        Ogre::MaterialManager::getSingleton().remove(m_table_material);
    }

    //Members of m_listeners are freed by the manager upon destruction. Freeing them here causes a double-free.
}

//-----------------------------------------------------------------------------

void summed_area_table::compute_parallel(
    const texture::sptr& _img_texture,
    const transfer_function::sptr& _gpu_tf,
    float _sample_distance,
    unsigned int _ratio
)
{
    SIGHT_ASSERT("imgTexture cannot be nullptr", _img_texture != nullptr);
    SIGHT_ASSERT("tf cannot be nullptr", _gpu_tf != nullptr);
    SIGHT_ASSERT("Ratio must be greater than 0", _ratio > 0);

    const auto source_size = _img_texture->size();
    const auto scaled_size = std::array<std::size_t, 3> {
        source_size[0] / _ratio,
        source_size[1] / _ratio,
        source_size[2] / _ratio
    };
    SIGHT_ASSERT("Size is null", source_size[0] > 0 && source_size[1] > 0 && source_size[2] > 0);

    this->resize(scaled_size);

    //Material (init)
    {
        auto init_pass_mtl = m_init_material;

        if(init_pass_mtl->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = init_pass_mtl->getTechnique(0);

            if(technique->getNumPasses() > 0)
            {
                Ogre::Pass* const sat_init_pass = technique->getPass(0);
                _img_texture->bind(sat_init_pass, "image");

                auto fp_params = sat_init_pass->getFragmentProgramParameters();
                fp_params->setNamedConstant("u_sampleDistance", _sample_distance);

                _gpu_tf->bind(sat_init_pass, "transferFunction", fp_params);
            }
        }
    }

    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
    const std::size_t depth                     = m_sat_size[2];

    // Copy our original image to the source buffer.
    for(std::size_t slice_index = 0 ; slice_index < depth ; ++slice_index)
    {
        Ogre::RenderTexture* const target = m_source_buffer->getBuffer()->getRenderTarget(slice_index);
        m_current_slice_depth = static_cast<float>(slice_index) / static_cast<float>(depth);

        if(target->getNumViewports() > 0)
        {
            Ogre::Viewport* const vp = target->getViewport(0);

            compositor_manager.setCompositorEnabled(vp, "summed_area_table_init", true);
            target->update(false);
            compositor_manager.setCompositorEnabled(vp, "summed_area_table_init", false);
        }
    }

    // Enable SAT compositor.
    for(std::size_t slice_index = 0 ; slice_index < depth ; ++slice_index)
    {
        //Source
        {
            Ogre::RenderTexture* const target = m_source_buffer->getBuffer()->getRenderTarget(slice_index);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);

                compositor_manager.setCompositorEnabled(vp, "summed_area_table", true);
            }
        }

        //Target
        {
            Ogre::RenderTexture* const target = m_target_buffer->getBuffer()->getRenderTarget(slice_index);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);
                compositor_manager.setCompositorEnabled(vp, "summed_area_table", true);
            }
        }
    }

    //Material (table)
    {
        Ogre::MaterialPtr sat_mtl = m_table_material;

        if(sat_mtl->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = sat_mtl->getTechnique(0);
            if(technique->getNumPasses() > 0)
            {
                Ogre::Pass* const sat_pass                  = technique->getPass(0);
                Ogre::TextureUnitState* const src_img_state = sat_pass->getTextureUnitState("source");

                if(src_img_state != nullptr)
                {
                    for(m_pass_orientation = 0 ; m_pass_orientation < 3 ; ++m_pass_orientation)
                    {
                        const int dim =
                            static_cast<int>(m_sat_size[static_cast<std::size_t>(m_pass_orientation)]);
                        const int nb_passes = static_cast<int>(std::ceil(std::log(dim) / std::log(NB_TEXT_READS)));

                        m_read_offset = 1;

                        for(int pass_index = 0 ; pass_index < nb_passes ; ++pass_index)
                        {
                            src_img_state->setTexture(m_source_buffer);

                            for(m_slice_index = 0 ; m_slice_index < depth ; ++m_slice_index)
                            {
                                auto* target = m_target_buffer->getBuffer()->getRenderTarget(m_slice_index);
                                target->update(false);
                            }

                            m_read_offset *= NB_TEXT_READS;

                            // Ping-pong swap.
                            std::swap(m_source_buffer, m_target_buffer);
                        }
                    }
                }
            }
        }
    }

    // Disable SAT compositor.
    for(std::size_t slice_index = 0 ; slice_index < depth ; ++slice_index)
    {
        //Source
        {
            Ogre::RenderTexture* const target = m_source_buffer->getBuffer()->getRenderTarget(slice_index);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);

                compositor_manager.setCompositorEnabled(vp, "summed_area_table", false);
            }
        }

        //Target
        {
            Ogre::RenderTexture* const target = m_target_buffer->getBuffer()->getRenderTarget(slice_index);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);
                compositor_manager.setCompositorEnabled(vp, "summed_area_table", false);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void summed_area_table::resize(std::array<std::size_t, 3> _sat_size)
{
    if(m_source_buffer != nullptr
       && m_target_buffer != nullptr
       && m_sat_size[0] == _sat_size[0]
       && m_sat_size[1] == _sat_size[1]
       && m_sat_size[2] == _sat_size[2])
    {
        return;
    }

    m_sat_size = _sat_size;

    const auto width  = static_cast<Ogre::uint>(m_sat_size[0]);
    const auto height = static_cast<Ogre::uint>(m_sat_size[1]);
    const auto depth  = static_cast<Ogre::uint>(m_sat_size[2]);

    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
    Ogre::TextureManager& texture_manager       = Ogre::TextureManager::getSingleton();

    if(m_table_material != nullptr)
    {
        Ogre::MaterialManager::getSingleton().remove(m_table_material);
        m_table_material.reset();
    }

    SIGHT_INFO(
        "SAT texture references after compositor-chain removal source_use_count="
        << m_source_buffer.use_count() << " target_use_count=" << m_target_buffer.use_count()
    );

    // Remove the ping-pong buffers before recreating them at the new size.
    if(m_source_buffer != nullptr)
    {
        m_source_buffer->unload();
        texture_manager.remove(m_source_buffer);
    }

    if(m_target_buffer != nullptr)
    {
        m_target_buffer->unload();
        texture_manager.remove(m_target_buffer);
    }

    // Release the old GPU resources before allocating their replacements.
    m_source_buffer.reset();
    m_target_buffer.reset();

    m_listeners = {};

    m_table_material = Ogre::MaterialManager::getSingleton().getByName("summed_area_table", RESOURCE_GROUP)->clone(
        m_parent_id + "_summed_area_table"
    );

    //Create the camera if this is the first call
    if(m_dummy_camera == nullptr)
    {
        m_dummy_camera = m_scene_manager->createCamera(m_parent_id + "_summed_area_table_DummyCamera");
    }

    //Create the buffers with the update information
    {
        m_source_buffer = texture_manager.createManual(
            m_parent_id + SOURCE_BUFFER_NAME,
            viz::scene3d::RESOURCE_GROUP,
            Ogre::TEX_TYPE_3D,
            width,
            height,
            depth,
            0, //Mipmap
            Ogre::PF_FLOAT32_RGBA,
            Ogre::TU_RENDERTARGET
        );

        m_target_buffer = texture_manager.createManual(
            m_parent_id + TARGET_BUFFER_NAME,
            viz::scene3d::RESOURCE_GROUP,
            Ogre::TEX_TYPE_3D,
            width,
            height,
            depth,
            0, //Mipmap
            Ogre::PF_FLOAT32_RGBA,
            Ogre::TU_RENDERTARGET
        );
    }

    //Update the listeners
    {
        //Listeners updated with the current parameters
        auto* const new_initlistener =
            new summed_area_table_init_compositor_listener(m_current_slice_depth);

        auto* const new_tablelistener =
            new summed_area_table_compositor_listener(
                m_read_offset,
                m_pass_orientation,
                m_slice_index
            );

        for(std::size_t slice_index = 0 ; slice_index < depth ; ++slice_index)
        {
            //Source buffer
            {
                Ogre::RenderTarget* const render_target = m_source_buffer->getBuffer()->getRenderTarget(slice_index);
                render_target->setDepthBufferPool(Ogre::DepthBuffer::POOL_NO_DEPTH);
                Ogre::Viewport* const vp = render_target->addViewport(m_dummy_camera);

                vp->setOverlaysEnabled(false);

                compositor_manager.addCompositor(vp, "summed_area_table_init");
                compositor_manager.addCompositor(vp, "summed_area_table");

                compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_table_init")
                ->getTechnique()->getOutputTargetPass()->getPasses()[0]
                ->setMaterialName(m_init_material->getName());

                compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_table")
                ->getTechnique()->getOutputTargetPass()->getPasses()[0]
                ->setMaterialName(m_table_material->getName());

                //Init listener
                {
                    Ogre::CompositorInstance* const comp_instance =
                        compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_table_init");

                    //Remove the old listener
                    if(m_listeners.init != nullptr)
                    {
                        comp_instance->removeListener(m_listeners.init);
                    }

                    //Replace it by the new
                    comp_instance->addListener(new_initlistener);
                }

                //Table listener
                {
                    Ogre::CompositorInstance* const comp_instance =
                        compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_table");

                    //Remove the old listener
                    if(m_listeners.table != nullptr)
                    {
                        comp_instance->removeListener(m_listeners.table);
                    }

                    //Replace it by the new
                    comp_instance->addListener(new_tablelistener);
                }
            }

            //Target buffer
            {
                Ogre::RenderTarget* const render_target = m_target_buffer->getBuffer()->getRenderTarget(slice_index);
                render_target->setDepthBufferPool(Ogre::DepthBuffer::POOL_NO_DEPTH);
                Ogre::Viewport* const vp = render_target->addViewport(m_dummy_camera);

                vp->setOverlaysEnabled(false);

                compositor_manager.addCompositor(vp, "summed_area_table_init");
                compositor_manager.addCompositor(vp, "summed_area_table");

                compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_table_init")
                ->getTechnique()->getOutputTargetPass()->getPasses()[0]
                ->setMaterialName(m_init_material->getName());

                compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_table")
                ->getTechnique()->getOutputTargetPass()->getPasses()[0]
                ->setMaterialName(m_table_material->getName());

                //Init listener
                {
                    Ogre::CompositorInstance* const comp_instance =
                        compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_table_init");

                    //Remove the old listener
                    if(m_listeners.init != nullptr)
                    {
                        comp_instance->removeListener(m_listeners.init);
                    }

                    comp_instance->addListener(new_initlistener);
                }

                //Table listener
                {
                    Ogre::CompositorInstance* const comp_instance =
                        compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_table");

                    //Remove the old listener
                    if(m_listeners.table != nullptr)
                    {
                        comp_instance->removeListener(m_listeners.table);
                    }

                    //Replace it by the new
                    comp_instance->addListener(new_tablelistener);
                }
            }
        }

        //Update the pointers
        m_listeners.init  = new_initlistener;
        m_listeners.table = new_tablelistener;
    }
}

//-----------------------------------------------------------------------------

void summed_area_table::compute_sequential(data::image::sptr _image, data::transfer_function::sptr _tf)
{
    std::vector<glm::vec4> buffer(m_sat_size[0] * m_sat_size[1] * m_sat_size[2]);

    //Convenience wrapper to get the value corresponding to a certain voxel
    static const auto s_VALUE_AT =
        [this](const std::vector<glm::vec4>& _sat_buffer, int _x, int _y, int _z)
        {
            if(_x < 0 || _y < 0 || _z < 0)
            {
                return glm::vec4(0.F);
            }

            const std::size_t index = static_cast<std::size_t>(_x) + m_sat_size[0]
                                      * static_cast<std::size_t>(_y) + m_sat_size[0]
                                      * m_sat_size[1] * static_cast<std::size_t>(_z);

            //No range check here. It is assumed to be correct.
            return _sat_buffer[index];
        };

    //Convenience wrapper to set the value corresponding to a certain voxel
    static const auto s_SET_VALUE =
        [this](std::vector<glm::vec4>& _sat_buffer, const glm::vec4& _value, int _x, int _y, int _z)
        {
            const std::size_t index = static_cast<std::size_t>(_x) + m_sat_size[0]
                                      * static_cast<std::size_t>(_y) + m_sat_size[0]
                                      * m_sat_size[1] * static_cast<std::size_t>(_z);

            //No range check here. It is assumed to be correct.
            _sat_buffer[index] = _value;
        };

    const auto dump_lock = _image->dump_lock();

    for(int z = 0 ; std::cmp_less(z, m_sat_size[2]) ; ++z)
    {
        for(int y = 0 ; std::cmp_less(y, m_sat_size[1]) ; ++y)
        {
            for(int x = 0 ; std::cmp_less(x, m_sat_size[0]) ; ++x)
            {
                const std::int16_t img_value =
                    _image->at<std::int16_t>(
                        static_cast<std::size_t>(x),
                        static_cast<std::size_t>(y),
                        static_cast<std::size_t>(z)
                    );

                const glm::vec4 saturation = glm::vec4(_tf->sample(img_value))
                                             + s_VALUE_AT(buffer, x - 1, y - 1, z - 1)
                                             + s_VALUE_AT(buffer, x, y, z - 1)
                                             + s_VALUE_AT(buffer, x, y - 1, z)
                                             + s_VALUE_AT(buffer, x - 1, y, z)
                                             - s_VALUE_AT(buffer, x - 1, y - 1, z)
                                             - s_VALUE_AT(buffer, x, y - 1, z - 1)
                                             - s_VALUE_AT(buffer, x - 1, y, z - 1);

                s_SET_VALUE(buffer, saturation, x, y, z);
            }
        }
    }

    Ogre::HardwarePixelBufferSharedPtr pix_buffer = m_source_buffer->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pix_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    Ogre::PixelBox pix_box = pix_buffer->getCurrentLock();

    auto* const p_dest = static_cast<std::uint8_t*>(pix_box.data);

    std::memcpy(p_dest, buffer.data(), buffer.size() * sizeof(glm::vec4));

    pix_buffer->unlock();
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
