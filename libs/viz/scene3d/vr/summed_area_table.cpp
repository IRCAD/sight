/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

#include "viz/scene3d/utils.hpp"

#include <viz/scene3d/ogre.hpp>

#include <glm/glm.hpp>

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

#include <cmath>

namespace sight::viz::scene3d::vr
{

/// Listener dedicated to watching the initialisation.
class summed_area_table::summed_area_tableInitCompositorListener : public Ogre::CompositorInstance::Listener
{
public:

    explicit summed_area_tableInitCompositorListener(float& _current_slice_index) :
        m_currentSliceDepth(_current_slice_index)
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

                init_pass_params->setNamedConstant("u_sliceDepth", m_currentSliceDepth);
            }
        }
    }

private:

    float& m_currentSliceDepth;
};

/// Listener dedicated to the compositor.
class summed_area_table::summed_area_tableCompositorListener : public Ogre::CompositorInstance::Listener
{
public:

    summed_area_tableCompositorListener(int& _read_offset, int& _pass_orientation, std::size_t& _current_slice_index) :
        m_readOffset(_read_offset),
        m_passOrientation(_pass_orientation),
        m_currentSliceIndex(_current_slice_index)
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

                        sat_pass_params->setNamedConstant("u_readOffset", m_readOffset);
                        sat_pass_params->setNamedConstant("u_passOrientation", m_passOrientation);
                        sat_pass_params->setNamedConstant("u_sliceIndex", static_cast<int>(m_currentSliceIndex));
                    }
                }
            }
        }
    }

private:

    int& m_readOffset;

    int& m_passOrientation;

    std::size_t& m_currentSliceIndex;
};

//-----------------------------------------------------------------------------

summed_area_table::summed_area_table(std::string _parent_id, Ogre::SceneManager* _scene_manager, float _size_ratio) :
    m_satSizeRatio(_size_ratio),
    m_satSize(
    {
        0, 0, 0
    }),
    m_currentImageSize({0, 0, 0}),
    m_parentId(std::move(_parent_id)),
    m_sceneManager(_scene_manager)
{
}

//-----------------------------------------------------------------------------

summed_area_table::~summed_area_table()
{
    Ogre::TextureManager& texture_manager = Ogre::TextureManager::getSingleton();

    if(m_sourceBuffer != nullptr)
    {
        texture_manager.remove(m_parentId + SOURCE_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    }

    if(m_targetBuffer != nullptr)
    {
        texture_manager.remove(m_parentId + TARGET_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    }

    if(m_dummyCamera != nullptr)
    {
        m_sceneManager->destroyCamera(m_dummyCamera);
    }

    //Members of m_listeners are freed by the manager upon destruction. Freeing them here causes a double-free.
}

//-----------------------------------------------------------------------------

void summed_area_table::computeParallel(
    const texture::sptr& _img_texture,
    const transfer_function::sptr& _gpu_tf,
    float _sample_distance
)
{
    SIGHT_ASSERT("imgTexture cannot be nullptr", _img_texture != nullptr);
    SIGHT_ASSERT("tf cannot be nullptr", _gpu_tf != nullptr);

    this->updateSatFromTexture(_img_texture);

    //Material (init)
    {
        Ogre::MaterialPtr init_pass_mtl =
            Ogre::MaterialManager::getSingleton().getByName("summed_area_tableInit", RESOURCE_GROUP);

        if(init_pass_mtl->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = init_pass_mtl->getTechnique(0);

            if(technique->getNumPasses() > 0)
            {
                Ogre::Pass* const sat_init_pass            = technique->getPass(0);
                Ogre::TextureUnitState* const tex3_d_state = sat_init_pass->getTextureUnitState("image");
                SIGHT_ASSERT("'image' texture unit is not found", tex3_d_state);
                tex3_d_state->setTexture(_img_texture->get());

                auto fp_params = sat_init_pass->getFragmentProgramParameters();
                fp_params->setNamedConstant("u_sampleDistance", _sample_distance);

                _gpu_tf->bind(sat_init_pass, "transferFunction", fp_params);
            }
        }
    }

    Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();
    const std::size_t depth                     = m_satSize[2];

    // Copy our original image to the source buffer.
    for(std::size_t slice_index = 0 ; slice_index < depth ; ++slice_index)
    {
        Ogre::RenderTexture* const target = m_sourceBuffer->getBuffer()->getRenderTarget(slice_index);
        m_currentSliceDepth = static_cast<float>(slice_index) / static_cast<float>(depth);

        if(target->getNumViewports() > 0)
        {
            Ogre::Viewport* const vp = target->getViewport(0);

            compositor_manager.setCompositorEnabled(vp, "summed_area_tableInit", true);
            target->update(false);
            compositor_manager.setCompositorEnabled(vp, "summed_area_tableInit", false);
        }
    }

    // Enable SAT compositor.
    for(std::size_t slice_index = 0 ; slice_index < depth ; ++slice_index)
    {
        //Source
        {
            Ogre::RenderTexture* const target = m_sourceBuffer->getBuffer()->getRenderTarget(slice_index);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);

                compositor_manager.setCompositorEnabled(vp, "summed_area_table", true);
            }
        }

        //Target
        {
            Ogre::RenderTexture* const target = m_targetBuffer->getBuffer()->getRenderTarget(slice_index);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);
                compositor_manager.setCompositorEnabled(vp, "summed_area_table", true);
            }
        }
    }

    //Material (table)
    {
        Ogre::MaterialPtr sat_mtl =
            Ogre::MaterialManager::getSingleton().getByName("summed_area_table", RESOURCE_GROUP);

        if(sat_mtl->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = sat_mtl->getTechnique(0);
            if(technique->getNumPasses() > 0)
            {
                Ogre::Pass* const sat_pass                  = technique->getPass(0);
                Ogre::TextureUnitState* const src_img_state = sat_pass->getTextureUnitState("source");

                if(src_img_state != nullptr)
                {
                    for(m_passOrientation = 0 ; m_passOrientation < 3 ; ++m_passOrientation)
                    {
                        const int dim       = static_cast<int>(m_satSize[static_cast<std::size_t>(m_passOrientation)]);
                        const int nb_passes = static_cast<int>(std::ceil(std::log(dim) / std::log(s_nbTextReads)));

                        m_readOffset = 1;

                        for(int pass_index = 0 ; pass_index < nb_passes ; ++pass_index)
                        {
                            src_img_state->setTexture(m_sourceBuffer);

                            for(m_sliceIndex = 0 ; m_sliceIndex < depth ; ++m_sliceIndex)
                            {
                                m_targetBuffer->getBuffer()->getRenderTarget(static_cast<std::size_t>(m_sliceIndex))->
                                update(
                                    false
                                );
                            }

                            m_readOffset *= s_nbTextReads;

                            // Ping-pong swap.
                            std::swap(m_sourceBuffer, m_targetBuffer);
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
            Ogre::RenderTexture* const target = m_sourceBuffer->getBuffer()->getRenderTarget(slice_index);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);

                compositor_manager.setCompositorEnabled(vp, "summed_area_table", false);
            }
        }

        //Target
        {
            Ogre::RenderTexture* const target = m_targetBuffer->getBuffer()->getRenderTarget(slice_index);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);
                compositor_manager.setCompositorEnabled(vp, "summed_area_table", false);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void summed_area_table::updateSatFromTexture(const texture::sptr& _img_texture)
{
    SIGHT_ASSERT("texture cannot be nullptr", _img_texture != nullptr);

    m_currentImageSize =
    {
        static_cast<std::size_t>(_img_texture->width()),
        static_cast<std::size_t>(_img_texture->height()),
        static_cast<std::size_t>(_img_texture->depth())
    };

    const auto width  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[0]) * m_satSizeRatio);
    const auto height = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[1]) * m_satSizeRatio);
    const auto depth  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[2]) * m_satSizeRatio);

    m_satSize = {width, height, depth};

    this->updateBuffers();
}

//-----------------------------------------------------------------------------

void summed_area_table::updateSatFromRatio(float _size_ratio)
{
    m_satSizeRatio = _size_ratio;

    const auto width  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[0]) * m_satSizeRatio);
    const auto height = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[1]) * m_satSizeRatio);
    const auto depth  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[2]) * m_satSizeRatio);

    m_satSize = {width, height, depth};

    this->updateBuffers();
}

//-----------------------------------------------------------------------------

void summed_area_table::updateBuffers()
{
    const auto width  = static_cast<Ogre::uint>(m_satSize[0]);
    const auto height = static_cast<Ogre::uint>(m_satSize[1]);
    const auto depth  = static_cast<Ogre::uint>(m_satSize[2]);

    Ogre::TextureManager& texture_manager = Ogre::TextureManager::getSingleton();

    // Removes the ping pong buffers if they have to be resized
    if(m_sourceBuffer != nullptr)
    {
        texture_manager.remove(m_parentId + SOURCE_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    }

    if(m_targetBuffer != nullptr)
    {
        texture_manager.remove(m_parentId + TARGET_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    }

    //Create the camera if this is the first call
    if(m_dummyCamera == nullptr)
    {
        m_dummyCamera = m_sceneManager->createCamera(m_parentId + "_summed_area_table_DummyCamera");
    }

    //Create the buffers with the update information
    {
        m_sourceBuffer = texture_manager.createManual(
            m_parentId + SOURCE_BUFFER_NAME,
            viz::scene3d::RESOURCE_GROUP,
            Ogre::TEX_TYPE_3D,
            width,
            height,
            depth,
            0, //Mipmap
            Ogre::PF_FLOAT32_RGBA,
            Ogre::TU_RENDERTARGET
        );

        m_targetBuffer = texture_manager.createManual(
            m_parentId + TARGET_BUFFER_NAME,
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
        Ogre::CompositorManager& compositor_manager = Ogre::CompositorManager::getSingleton();

        //Listeners updated with the current parameters
        auto* const new_initlistener =
            new summed_area_tableInitCompositorListener(m_currentSliceDepth);

        auto* const new_tablelistener =
            new summed_area_tableCompositorListener(
                m_readOffset,
                m_passOrientation,
                m_sliceIndex
            );

        for(std::size_t slice_index = 0 ; slice_index < depth ; ++slice_index)
        {
            //Source buffer
            {
                Ogre::RenderTarget* const render_target = m_sourceBuffer->getBuffer()->getRenderTarget(slice_index);
                Ogre::Viewport* const vp                = render_target->addViewport(m_dummyCamera);

                vp->setOverlaysEnabled(false);

                compositor_manager.addCompositor(vp, "summed_area_tableInit");
                compositor_manager.addCompositor(vp, "summed_area_table");

                //Init listener
                {
                    Ogre::CompositorInstance* const comp_instance =
                        compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_tableInit");

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
                Ogre::RenderTarget* const render_target = m_targetBuffer->getBuffer()->getRenderTarget(slice_index);
                Ogre::Viewport* const vp                = render_target->addViewport(m_dummyCamera);

                vp->setOverlaysEnabled(false);

                compositor_manager.addCompositor(vp, "summed_area_tableInit");
                compositor_manager.addCompositor(vp, "summed_area_table");

                //Init listener
                {
                    Ogre::CompositorInstance* const comp_instance =
                        compositor_manager.getCompositorChain(vp)->getCompositor("summed_area_tableInit");

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

void summed_area_table::computeSequential(data::image::sptr _image, data::transfer_function::sptr _tf)
{
    std::vector<glm::vec4> buffer(m_satSize[0] * m_satSize[1] * m_satSize[2]);

    //Convenience wrapper to get the value corresponding to a certain voxel
    static const auto value_at =
        [this](const std::vector<glm::vec4>& _sat_buffer, int _x, int _y, int _z)
        {
            if(_x < 0 || _y < 0 || _z < 0)
            {
                return glm::vec4(0.F);
            }

            const std::size_t index = static_cast<std::size_t>(_x) + m_satSize[0]
                                      * static_cast<std::size_t>(_y) + m_satSize[0]
                                      * m_satSize[1] * static_cast<std::size_t>(_z);

            //No range check here. It is assumed to be correct.
            return _sat_buffer[index];
        };

    //Convenience wrapper to set the value corresponding to a certain voxel
    static const auto set_value =
        [this](std::vector<glm::vec4>& _sat_buffer, const glm::vec4& _value, int _x, int _y, int _z)
        {
            const std::size_t index = static_cast<std::size_t>(_x) + m_satSize[0]
                                      * static_cast<std::size_t>(_y) + m_satSize[0]
                                      * m_satSize[1] * static_cast<std::size_t>(_z);

            //No range check here. It is assumed to be correct.
            _sat_buffer[index] = _value;
        };

    const auto dump_lock = _image->dump_lock();

    for(int z = 0 ; z < static_cast<int>(m_satSize[2]) ; ++z)
    {
        for(int y = 0 ; y < static_cast<int>(m_satSize[1]) ; ++y)
        {
            for(int x = 0 ; x < static_cast<int>(m_satSize[0]) ; ++x)
            {
                const std::int16_t img_value =
                    _image->at<std::int16_t>(
                        static_cast<std::size_t>(x),
                        static_cast<std::size_t>(y),
                        static_cast<std::size_t>(z)
                    );

                const glm::vec4 saturation = glm::vec4(_tf->sample(img_value))
                                             + value_at(buffer, x - 1, y - 1, z - 1)
                                             + value_at(buffer, x, y, z - 1)
                                             + value_at(buffer, x, y - 1, z)
                                             + value_at(buffer, x - 1, y, z)
                                             - value_at(buffer, x - 1, y - 1, z)
                                             - value_at(buffer, x, y - 1, z - 1)
                                             - value_at(buffer, x - 1, y, z - 1);

                set_value(buffer, saturation, x, y, z);
            }
        }
    }

    Ogre::HardwarePixelBufferSharedPtr pix_buffer = m_sourceBuffer->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pix_buffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    Ogre::PixelBox pix_box = pix_buffer->getCurrentLock();

    auto* const p_dest = static_cast<std::uint8_t*>(pix_box.data);

    std::memcpy(p_dest, buffer.data(), buffer.size() * sizeof(glm::vec4));

    pix_buffer->unlock();
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
