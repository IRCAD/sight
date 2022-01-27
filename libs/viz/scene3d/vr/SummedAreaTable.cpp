/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "viz/scene3d/vr/SummedAreaTable.hpp"

#include "viz/scene3d/Utils.hpp"

#include <viz/scene3d/ogre.hpp>

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

#include <math.h>

namespace sight::viz::scene3d
{

namespace vr
{

/// Listener dedicated to watching the initialisation.
class SummedAreaTable::SummedAreaTableInitCompositorListener : public Ogre::CompositorInstance::Listener
{
public:

    SummedAreaTableInitCompositorListener(float& currentSliceIndex) :
        m_currentSliceDepth(currentSliceIndex)
    {
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& mat)
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
class SummedAreaTable::SummedAreaTableCompositorListener : public Ogre::CompositorInstance::Listener
{
public:

    SummedAreaTableCompositorListener(int& readOffset, int& passOrientation, std::size_t& currentSliceIndex) :
        m_readOffset(readOffset),
        m_passOrientation(passOrientation),
        m_currentSliceIndex(currentSliceIndex)
    {
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& mat)
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

SummedAreaTable::SummedAreaTable(std::string _parentId, Ogre::SceneManager* _sceneManager, float _sizeRatio) :
    m_satSizeRatio(_sizeRatio),
    m_satSize(
        {
            0, 0, 0
        }),
    m_currentImageSize({0, 0, 0}),
    m_parentId(_parentId),
    m_sceneManager(_sceneManager),
    m_dummyCamera(nullptr)
{
}

//-----------------------------------------------------------------------------

SummedAreaTable::~SummedAreaTable()
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

    //Members of m_listeners are freed by the manager upon desctruction. Freeing them here causes a double-free.
}

//-----------------------------------------------------------------------------

void SummedAreaTable::computeParallel(
    Ogre::TexturePtr _imgTexture,
    const TransferFunction::sptr& _gpuTf,
    float _sampleDistance
)
{
    SIGHT_ASSERT("imgTexture cannot be nullptr", _imgTexture != nullptr);
    SIGHT_ASSERT("tf cannot be nullptr", _gpuTf != nullptr);

    this->updateSatFromTexture(_imgTexture);

    //Material (init)
    {
        Ogre::MaterialPtr initPassMtl =
            Ogre::MaterialManager::getSingleton().getByName("SummedAreaTableInit", RESOURCE_GROUP);

        if(initPassMtl->getNumTechniques() > 0)
        {
            Ogre::Technique* const technique = initPassMtl->getTechnique(0);

            if(technique->getNumPasses() > 0)
            {
                Ogre::Pass* const satInitPass            = technique->getPass(0);
                Ogre::TextureUnitState* const tex3DState = satInitPass->getTextureUnitState("image");
                SIGHT_ASSERT("'image' texture unit is not found", tex3DState);
                tex3DState->setTexture(_imgTexture);

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

            compositorManager.setCompositorEnabled(vp, "SummedAreaTableInit", true);
            target->update(false);
            compositorManager.setCompositorEnabled(vp, "SummedAreaTableInit", false);
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

                compositorManager.setCompositorEnabled(vp, "SummedAreaTable", true);
            }
        }

        //Target
        {
            Ogre::RenderTexture* const target = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);
                compositorManager.setCompositorEnabled(vp, "SummedAreaTable", true);
            }
        }
    }

    //Material (table)
    {
        Ogre::MaterialPtr satMtl =
            Ogre::MaterialManager::getSingleton().getByName("SummedAreaTable", RESOURCE_GROUP);

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

                compositorManager.setCompositorEnabled(vp, "SummedAreaTable", false);
            }
        }

        //Target
        {
            Ogre::RenderTexture* const target = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex);

            if(target->getNumViewports() > 0)
            {
                Ogre::Viewport* const vp = target->getViewport(0);
                compositorManager.setCompositorEnabled(vp, "SummedAreaTable", false);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SummedAreaTable::updateSatFromTexture(Ogre::TexturePtr _imgTexture)
{
    SIGHT_ASSERT("texture cannot be nullptr", _imgTexture != nullptr);

    m_currentImageSize =
    {
        static_cast<std::size_t>(_imgTexture->getWidth()),
        static_cast<std::size_t>(_imgTexture->getHeight()),
        static_cast<std::size_t>(_imgTexture->getDepth())
    };

    const std::size_t width  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[0]) * m_satSizeRatio);
    const std::size_t height = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[1]) * m_satSizeRatio);
    const std::size_t depth  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[2]) * m_satSizeRatio);

    m_satSize = {width, height, depth};

    this->updateBuffers();
}

//-----------------------------------------------------------------------------

void SummedAreaTable::updateSatFromRatio(float _sizeRatio)
{
    m_satSizeRatio = _sizeRatio;

    const std::size_t width  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[0]) * m_satSizeRatio);
    const std::size_t height = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[1]) * m_satSizeRatio);
    const std::size_t depth  = static_cast<std::size_t>(static_cast<float>(m_currentImageSize[2]) * m_satSizeRatio);

    m_satSize = {width, height, depth};

    this->updateBuffers();
}

//-----------------------------------------------------------------------------

void SummedAreaTable::updateBuffers()
{
    const Ogre::uint width  = static_cast<Ogre::uint>(m_satSize[0]);
    const Ogre::uint height = static_cast<Ogre::uint>(m_satSize[1]);
    const Ogre::uint depth  = static_cast<Ogre::uint>(m_satSize[2]);

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
        m_dummyCamera = m_sceneManager->createCamera(m_parentId + "_SummedAreaTable_DummyCamera");
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
        SummedAreaTableInitCompositorListener* const new_initlistener =
            new SummedAreaTableInitCompositorListener(m_currentSliceDepth);

        SummedAreaTableCompositorListener* const new_tablelistener =
            new SummedAreaTableCompositorListener(
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

                compositorManager.addCompositor(vp, "SummedAreaTableInit");
                compositorManager.addCompositor(vp, "SummedAreaTable");

                //Init listener
                {
                    Ogre::CompositorInstance* const compInstance =
                        compositorManager.getCompositorChain(vp)->getCompositor("SummedAreaTableInit");

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
                        compositorManager.getCompositorChain(vp)->getCompositor("SummedAreaTable");

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

                compositorManager.addCompositor(vp, "SummedAreaTableInit");
                compositorManager.addCompositor(vp, "SummedAreaTable");

                //Init listener
                {
                    Ogre::CompositorInstance* const compInstance =
                        compositorManager.getCompositorChain(vp)->getCompositor("SummedAreaTableInit");

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
                        compositorManager.getCompositorChain(vp)->getCompositor("SummedAreaTable");

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

void SummedAreaTable::computeSequential(data::Image::sptr _image, data::TransferFunction::sptr _tf)
{
    std::vector<glm::vec4> buffer(m_satSize[0] * m_satSize[1] * m_satSize[2]);

    //Convenience wrapper to get the value corresponding to a certain voxel
    static const auto value_at =
        [this](const std::vector<glm::vec4>& satBuffer, int x, int y, int z)
        {
            if(x < 0 || y < 0 || z < 0)
            {
                return glm::vec4(0.f);
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

    const auto dumpLock = _image->lock();

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

                const glm::vec4 saturation = applyTf(_tf, imgValue)
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

    std::uint8_t* const pDest = static_cast<std::uint8_t*>(pixBox.data);

    std::memcpy(pDest, buffer.data(), buffer.size() * sizeof(glm::vec4));

    pixBuffer->unlock();
}

//-----------------------------------------------------------------------------

glm::vec4 SummedAreaTable::applyTf(data::TransferFunction::sptr _tf, int16_t imgValue)
{
    //Window (extent)
    const double invWindow = 1. / _tf->getWindow();

    //Intensity min and max
    const auto [i_min, i_max] = _tf->getWLMinMax();

    //TF min and max
    const auto [tf_min, tf_max] = _tf->getMinMaxTFValues();

    //Scaled value, which needs to be interpolated
    const double value = (imgValue - i_min) * (tf_max - tf_min) * invWindow + tf_min;

    const data::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

    return glm::vec4(interpolatedColor.r, interpolatedColor.g, interpolatedColor.b, interpolatedColor.a);
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d
