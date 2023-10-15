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

#include "viz/scene3d/Utils.hpp"

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

    explicit summed_area_tableInitCompositorListener(float& currentSliceIndex) :
        m_currentSliceDepth(currentSliceIndex)
    {
    }

    //------------------------------------------------------------------------------

    void notifyMaterialRender(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& mat) override
    {
        if(mat->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = mat->getTechnique(0);

            if(technique->getNumPasses() > 0)
            {
                Ogre::Pass* const satInitPass                      = technique->getPass(0);
                Ogre::GpuProgramParametersSharedPtr initPassParams = satInitPass->getFragmentProgramParameters();

                initPassParams->setNamedConstant("u_sliceDepth", m_currentSliceDepth);
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

    summed_area_tableCompositorListener(int& readOffset, int& passOrientation, std::size_t& currentSliceIndex) :
        m_readOffset(readOffset),
        m_passOrientation(passOrientation),
        m_currentSliceIndex(currentSliceIndex)
    {
    }

    //------------------------------------------------------------------------------

    void notifyMaterialRender(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& mat) override
    {
        if(mat->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = mat->getTechnique(0);

            if(technique != nullptr)
            {
                if(technique->getNumPasses() > 0)
                {
                    Ogre::Pass* const satPass = technique->getPass(0);

                    if(satPass != nullptr)
                    {
                        Ogre::GpuProgramParametersSharedPtr satPassParams = satPass->getFragmentProgramParameters();

                        satPassParams->setNamedConstant("u_readOffset", m_readOffset);
                        satPassParams->setNamedConstant("u_passOrientation", m_passOrientation);
                        satPassParams->setNamedConstant("u_sliceIndex", static_cast<int>(m_currentSliceIndex));
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

summed_area_table::summed_area_table(std::string _parentId, Ogre::SceneManager* _sceneManager, float _sizeRatio) :
    m_satSizeRatio(_sizeRatio),
    m_satSize(
    {
        0, 0, 0
    }),
    m_currentImageSize({0, 0, 0}),
    m_parentId(std::move(_parentId)),
    m_sceneManager(_sceneManager)
{
}

//-----------------------------------------------------------------------------

summed_area_table::~summed_area_table()
{
    Ogre::TextureManager& textureManager = Ogre::TextureManager::getSingleton();

    if(m_sourceBuffer != nullptr)
    {
        textureManager.remove(m_parentId + SOURCE_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    }

    if(m_targetBuffer != nullptr)
    {
        textureManager.remove(m_parentId + TARGET_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    }

    if(m_dummyCamera != nullptr)
    {
        m_sceneManager->destroyCamera(m_dummyCamera);
    }

    //Members of m_listeners are freed by the manager upon destruction. Freeing them here causes a double-free.
}

//-----------------------------------------------------------------------------

void summed_area_table::computeParallel(
    const Texture::sptr& _imgTexture,
    const transfer_function::sptr& _gpuTf,
    float _sampleDistance
)
{
    SIGHT_ASSERT("imgTexture cannot be nullptr", _imgTexture != nullptr);
    SIGHT_ASSERT("tf cannot be nullptr", _gpuTf != nullptr);

    this->updateSatFromTexture(_imgTexture);

    //Material (init)
    {
        Ogre::MaterialPtr initPassMtl =
            Ogre::MaterialManager::getSingleton().getByName("summed_area_tableInit", RESOURCE_GROUP);

        if(initPassMtl->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = initPassMtl->getTechnique(0);

            if(technique->getNumPasses() > 0)
            {
                Ogre::Pass* const satInitPass            = technique->getPass(0);
                Ogre::TextureUnitState* const tex3DState = satInitPass->getTextureUnitState("image");
                SIGHT_ASSERT("'image' texture unit is not found", tex3DState);
                tex3DState->setTexture(_imgTexture->get());

                auto fpParams = satInitPass->getFragmentProgramParameters();
                fpParams->setNamedConstant("u_sampleDistance", _sampleDistance);

                _gpuTf->bind(satInitPass, "transferFunction", fpParams);
            }
        }
    }

    Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();
    const std::size_t depth                    = m_satSize[2];

    // Copy our original image to the source buffer.
    for(std::size_t sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
    {
        Ogre::RenderTexture* const target = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex);
        m_currentSliceDepth = static_cast<float>(sliceIndex) / static_cast<float>(depth);

        if(target->getNumViewports() > 0)
        {
            Ogre::Viewport* const vp = target->getViewport(0);

            compositorManager.setCompositorEnabled(vp, "summed_area_tableInit", true);
            target->update(false);
            compositorManager.setCompositorEnabled(vp, "summed_area_tableInit", false);
        }
    }

    // Enable SAT compositor.
    for(std::size_t sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
    {
        //Source
        {
            Ogre::RenderTexture* const target = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);

                compositorManager.setCompositorEnabled(vp, "summed_area_table", true);
            }
        }

        //Target
        {
            Ogre::RenderTexture* const target = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);
                compositorManager.setCompositorEnabled(vp, "summed_area_table", true);
            }
        }
    }

    //Material (table)
    {
        Ogre::MaterialPtr satMtl =
            Ogre::MaterialManager::getSingleton().getByName("summed_area_table", RESOURCE_GROUP);

        if(satMtl->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = satMtl->getTechnique(0);
            if(technique->getNumPasses() > 0)
            {
                Ogre::Pass* const satPass                 = technique->getPass(0);
                Ogre::TextureUnitState* const srcImgState = satPass->getTextureUnitState("source");

                if(srcImgState != nullptr)
                {
                    for(m_passOrientation = 0 ; m_passOrientation < 3 ; ++m_passOrientation)
                    {
                        const int dim      = static_cast<int>(m_satSize[static_cast<std::size_t>(m_passOrientation)]);
                        const int nbPasses = static_cast<int>(std::ceil(std::log(dim) / std::log(s_nbTextReads)));

                        m_readOffset = 1;

                        for(int passIndex = 0 ; passIndex < nbPasses ; ++passIndex)
                        {
                            srcImgState->setTexture(m_sourceBuffer);

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
    for(std::size_t sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
    {
        //Source
        {
            Ogre::RenderTexture* const target = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);

                compositorManager.setCompositorEnabled(vp, "summed_area_table", false);
            }
        }

        //Target
        {
            Ogre::RenderTexture* const target = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);
                compositorManager.setCompositorEnabled(vp, "summed_area_table", false);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void summed_area_table::updateSatFromTexture(const Texture::sptr& _imgTexture)
{
    SIGHT_ASSERT("texture cannot be nullptr", _imgTexture != nullptr);

    m_currentImageSize =
    {
        static_cast<std::size_t>(_imgTexture->width()),
        static_cast<std::size_t>(_imgTexture->height()),
        static_cast<std::size_t>(_imgTexture->depth())
    };

    const auto width  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[0]) * m_satSizeRatio);
    const auto height = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[1]) * m_satSizeRatio);
    const auto depth  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[2]) * m_satSizeRatio);

    m_satSize = {width, height, depth};

    this->updateBuffers();
}

//-----------------------------------------------------------------------------

void summed_area_table::updateSatFromRatio(float _sizeRatio)
{
    m_satSizeRatio = _sizeRatio;

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

    Ogre::TextureManager& textureManager = Ogre::TextureManager::getSingleton();

    // Removes the ping pong buffers if they have to be resized
    if(m_sourceBuffer != nullptr)
    {
        textureManager.remove(m_parentId + SOURCE_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    }

    if(m_targetBuffer != nullptr)
    {
        textureManager.remove(m_parentId + TARGET_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    }

    //Create the camera if this is the first call
    if(m_dummyCamera == nullptr)
    {
        m_dummyCamera = m_sceneManager->createCamera(m_parentId + "_summed_area_table_DummyCamera");
    }

    //Create the buffers with the update information
    {
        m_sourceBuffer = textureManager.createManual(
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

        m_targetBuffer = textureManager.createManual(
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
        Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();

        //Listeners updated with the current parameters
        auto* const new_initlistener =
            new summed_area_tableInitCompositorListener(m_currentSliceDepth);

        auto* const new_tablelistener =
            new summed_area_tableCompositorListener(
                m_readOffset,
                m_passOrientation,
                m_sliceIndex
            );

        for(std::size_t sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
        {
            //Source buffer
            {
                Ogre::RenderTarget* const renderTarget = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex);
                Ogre::Viewport* const vp               = renderTarget->addViewport(m_dummyCamera);

                vp->setOverlaysEnabled(false);

                compositorManager.addCompositor(vp, "summed_area_tableInit");
                compositorManager.addCompositor(vp, "summed_area_table");

                //Init listener
                {
                    Ogre::CompositorInstance* const compInstance =
                        compositorManager.getCompositorChain(vp)->getCompositor("summed_area_tableInit");

                    //Remove the old listener
                    if(m_listeners.init != nullptr)
                    {
                        compInstance->removeListener(m_listeners.init);
                    }

                    //Replace it by the new
                    compInstance->addListener(new_initlistener);
                }

                //Table listener
                {
                    Ogre::CompositorInstance* const compInstance =
                        compositorManager.getCompositorChain(vp)->getCompositor("summed_area_table");

                    //Remove the old listener
                    if(m_listeners.table != nullptr)
                    {
                        compInstance->removeListener(m_listeners.table);
                    }

                    //Replace it by the new
                    compInstance->addListener(new_tablelistener);
                }
            }

            //Target buffer
            {
                Ogre::RenderTarget* const renderTarget = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex);
                Ogre::Viewport* const vp               = renderTarget->addViewport(m_dummyCamera);

                vp->setOverlaysEnabled(false);

                compositorManager.addCompositor(vp, "summed_area_tableInit");
                compositorManager.addCompositor(vp, "summed_area_table");

                //Init listener
                {
                    Ogre::CompositorInstance* const compInstance =
                        compositorManager.getCompositorChain(vp)->getCompositor("summed_area_tableInit");

                    //Remove the old listener
                    if(m_listeners.init != nullptr)
                    {
                        compInstance->removeListener(m_listeners.init);
                    }

                    compInstance->addListener(new_initlistener);
                }

                //Table listener
                {
                    Ogre::CompositorInstance* const compInstance =
                        compositorManager.getCompositorChain(vp)->getCompositor("summed_area_table");

                    //Remove the old listener
                    if(m_listeners.table != nullptr)
                    {
                        compInstance->removeListener(m_listeners.table);
                    }

                    //Replace it by the new
                    compInstance->addListener(new_tablelistener);
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
        [this](const std::vector<glm::vec4>& satBuffer, int x, int y, int z)
        {
            if(x < 0 || y < 0 || z < 0)
            {
                return glm::vec4(0.F);
            }

            const std::size_t index = static_cast<std::size_t>(x) + m_satSize[0]
                                      * static_cast<std::size_t>(y) + m_satSize[0]
                                      * m_satSize[1] * static_cast<std::size_t>(z);

            //No range check here. It is assumed to be correct.
            return satBuffer[index];
        };

    //Convenience wrapper to set the value corresponding to a certain voxel
    static const auto set_value =
        [this](std::vector<glm::vec4>& satBuffer, const glm::vec4& value, int x, int y, int z)
        {
            const std::size_t index = static_cast<std::size_t>(x) + m_satSize[0]
                                      * static_cast<std::size_t>(y) + m_satSize[0]
                                      * m_satSize[1] * static_cast<std::size_t>(z);

            //No range check here. It is assumed to be correct.
            satBuffer[index] = value;
        };

    const auto dumpLock = _image->dump_lock();

    for(int z = 0 ; z < static_cast<int>(m_satSize[2]) ; ++z)
    {
        for(int y = 0 ; y < static_cast<int>(m_satSize[1]) ; ++y)
        {
            for(int x = 0 ; x < static_cast<int>(m_satSize[0]) ; ++x)
            {
                const std::int16_t imgValue =
                    _image->at<std::int16_t>(
                        static_cast<std::size_t>(x),
                        static_cast<std::size_t>(y),
                        static_cast<std::size_t>(z)
                    );

                const glm::vec4 saturation = glm::vec4(_tf->sample(imgValue))
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

    Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_sourceBuffer->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pixBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();

    auto* const pDest = static_cast<std::uint8_t*>(pixBox.data);

    std::memcpy(pDest, buffer.data(), buffer.size() * sizeof(glm::vec4));

    pixBuffer->unlock();
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
