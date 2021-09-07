/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

//-----------------------------------------------------------------------------

class SummedAreaTableInitCompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    SummedAreaTableInitCompositorListener(float& currentSliceIndex) :
        m_currentSliceDepth(currentSliceIndex)
    {
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(::Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass* satInitPass = mat->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr initPassParams = satInitPass->getFragmentProgramParameters();

        initPassParams->setNamedConstant("u_sliceDepth", m_currentSliceDepth);
    }

private:

    float& m_currentSliceDepth;
};

//-----------------------------------------------------------------------------

class SummedAreaTableCompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    SummedAreaTableCompositorListener(int& readOffset, int& passOrientation, size_t& currentSliceIndex) :
        m_readOffset(readOffset),
        m_passOrientation(passOrientation),
        m_currentSliceIndex(currentSliceIndex)
    {
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(::Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass* satPass = mat->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr satPassParams = satPass->getFragmentProgramParameters();

        satPassParams->setNamedConstant("u_readOffset", m_readOffset);
        satPassParams->setNamedConstant("u_passOrientation", m_passOrientation);
        satPassParams->setNamedConstant("u_sliceIndex", static_cast<int>(m_currentSliceIndex));
    }

private:

    int& m_readOffset;

    int& m_passOrientation;

    size_t& m_currentSliceIndex;
};

//-----------------------------------------------------------------------------

SummedAreaTable::SummedAreaTable(std::string _parentId, ::Ogre::SceneManager* _sceneManager, float _sizeRatio) :
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
    ::Ogre::TextureManager& textureManager = ::Ogre::TextureManager::getSingleton();

    textureManager.remove(m_parentId + SOURCE_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    textureManager.remove(m_parentId + TARGET_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
}

//-----------------------------------------------------------------------------

void SummedAreaTable::computeParallel(
    ::Ogre::TexturePtr _imgTexture,
    const viz::scene3d::TransferFunction::sptr& _gpuTf,
    float _sampleDistance
)
{
    if(!m_sourceBuffer)
    {
        this->updateSatFromTexture(_imgTexture);
    }

    ::Ogre::MaterialPtr initPassMtl = ::Ogre::MaterialManager::getSingleton().getByName(
        "SummedAreaTableInit",
        RESOURCE_GROUP
    );
    ::Ogre::Pass* satInitPass = initPassMtl->getTechnique(0)->getPass(0);

    ::Ogre::TextureUnitState* tex3DState = satInitPass->getTextureUnitState("image");
    SIGHT_ASSERT("'image' texture unit is not found", tex3DState);
    tex3DState->setTexture(_imgTexture);

    auto fpParams = satInitPass->getFragmentProgramParameters();
    fpParams->setNamedConstant("u_sampleDistance", _sampleDistance);

    _gpuTf->bind(satInitPass, "transferFunction", fpParams);

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    const size_t depth                           = m_satSize[2];

    // Copy our original image to the source buffer.
    for(size_t sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
    {
        ::Ogre::Viewport* vp = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTableInit", true);

        m_currentSliceDepth = static_cast<float>(sliceIndex) / static_cast<float>(depth);

        m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex)->update(false);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTableInit", false);
    }

    // Enable SAT compositor.
    for(size_t sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
    {
        ::Ogre::Viewport* vp = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTable", true);

        vp = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTable", true);
    }

    ::Ogre::MaterialPtr satMtl = ::Ogre::MaterialManager::getSingleton().getByName(
        "SummedAreaTable",
        RESOURCE_GROUP
    );
    ::Ogre::Pass* satPass                 = satMtl->getTechnique(0)->getPass(0);
    ::Ogre::TextureUnitState* srcImgState = satPass->getTextureUnitState("source");

    for(m_passOrientation = 0 ; m_passOrientation < 3 ; ++m_passOrientation)
    {
        const int dim      = static_cast<int>(m_satSize[static_cast<size_t>(m_passOrientation)]);
        const int nbPasses = static_cast<int>(std::ceil(std::log(dim) / std::log(m_nbTextReads)));

        m_readOffset = 1;

        for(int passIndex = 0 ; passIndex < nbPasses ; ++passIndex)
        {
            srcImgState->setTexture(m_sourceBuffer);

            for(m_sliceIndex = 0 ; m_sliceIndex < depth ; ++m_sliceIndex)
            {
                m_targetBuffer->getBuffer()->getRenderTarget(static_cast<size_t>(m_sliceIndex))->update(false);
            }

            m_readOffset *= m_nbTextReads;

            // Ping-pong swap.
            std::swap(m_sourceBuffer, m_targetBuffer);
        }
    }

    // Disable SAT compositor.
    for(size_t sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
    {
        ::Ogre::Viewport* vp = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTable", false);

        vp = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTable", false);
    }
}

//-----------------------------------------------------------------------------

void SummedAreaTable::updateSatFromTexture(::Ogre::TexturePtr _imgTexture)
{
    m_currentImageSize =
    {
        static_cast<size_t>(_imgTexture->getWidth()),
        static_cast<size_t>(_imgTexture->getHeight()),
        static_cast<size_t>(_imgTexture->getDepth())
    };

    const size_t width  = static_cast<size_t>(static_cast<float>(m_currentImageSize[0]) * m_satSizeRatio);
    const size_t height = static_cast<size_t>(static_cast<float>(m_currentImageSize[1]) * m_satSizeRatio);
    const size_t depth  = static_cast<size_t>(static_cast<float>(m_currentImageSize[2]) * m_satSizeRatio);

    m_satSize = {width, height, depth};

    this->initializeSAT();
}

//-----------------------------------------------------------------------------

void SummedAreaTable::updateSatFromRatio(float _sizeRatio)
{
    m_satSizeRatio = _sizeRatio;

    const size_t width  = static_cast<size_t>(static_cast<float>(m_currentImageSize[0]) * m_satSizeRatio);
    const size_t height = static_cast<size_t>(static_cast<float>(m_currentImageSize[1]) * m_satSizeRatio);
    const size_t depth  = static_cast<size_t>(static_cast<float>(m_currentImageSize[2]) * m_satSizeRatio);

    m_satSize = {width, height, depth};

    this->initializeSAT();
}

//-----------------------------------------------------------------------------

void SummedAreaTable::initializeSAT()
{
    ::Ogre::uint width  = static_cast< ::Ogre::uint>(m_satSize[0]);
    ::Ogre::uint height = static_cast< ::Ogre::uint>(m_satSize[1]);
    ::Ogre::uint depth  = static_cast< ::Ogre::uint>(m_satSize[2]);

    ::Ogre::TextureManager& textureManager = ::Ogre::TextureManager::getSingleton();

    // Removes the ping pong buffers if they have to be resized
    textureManager.remove(m_parentId + SOURCE_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);
    textureManager.remove(m_parentId + TARGET_BUFFER_NAME, viz::scene3d::RESOURCE_GROUP);

    m_sourceBuffer = textureManager.createManual(
        m_parentId + SOURCE_BUFFER_NAME,
        viz::scene3d::RESOURCE_GROUP,
        ::Ogre::TEX_TYPE_3D,
        width,
        height,
        depth,
        0,
        ::Ogre::PF_FLOAT32_RGBA,
        ::Ogre::TU_RENDERTARGET
    );

    m_targetBuffer = textureManager.createManual(
        m_parentId + TARGET_BUFFER_NAME,
        viz::scene3d::RESOURCE_GROUP,
        ::Ogre::TEX_TYPE_3D,
        width,
        height,
        depth,
        0,
        ::Ogre::PF_FLOAT32_RGBA,
        ::Ogre::TU_RENDERTARGET
    );

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    SummedAreaTableInitCompositorListener* satInitListener =
        new SummedAreaTableInitCompositorListener(m_currentSliceDepth);

    SummedAreaTableCompositorListener* satListener =
        new SummedAreaTableCompositorListener(m_readOffset, m_passOrientation, m_sliceIndex);

    if(!m_dummyCamera)
    {
        m_dummyCamera = m_sceneManager->createCamera(m_parentId + "_SummedAreaTable_DummyCamera");
    }

    for(size_t sliceIndex = 0 ; sliceIndex < depth ; ++sliceIndex)
    {
        // Init source buffer
        ::Ogre::RenderTarget* renderTarget = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex);
        ::Ogre::Viewport* vp               = renderTarget->addViewport(m_dummyCamera);

        vp->setOverlaysEnabled(false);

        compositorManager.addCompositor(vp, "SummedAreaTableInit");
        compositorManager.addCompositor(vp, "SummedAreaTable");

        ::Ogre::CompositorInstance* compInstance = compositorManager.getCompositorChain(vp)->getCompositor(
            "SummedAreaTableInit"
        );
        compInstance->addListener(satInitListener);

        compInstance = compositorManager.getCompositorChain(vp)->getCompositor("SummedAreaTable");
        compInstance->addListener(satListener);

        // Init target buffer
        renderTarget = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex);
        vp           = renderTarget->addViewport(m_dummyCamera);

        vp->setOverlaysEnabled(false);

        compositorManager.addCompositor(vp, "SummedAreaTableInit");
        compositorManager.addCompositor(vp, "SummedAreaTable");

        compInstance = compositorManager.getCompositorChain(vp)->getCompositor("SummedAreaTableInit");
        compInstance->addListener(satInitListener);

        compInstance = compositorManager.getCompositorChain(vp)->getCompositor("SummedAreaTable");
        compInstance->addListener(satListener);
    }
}

//-----------------------------------------------------------------------------

void SummedAreaTable::computeSequential(data::Image::sptr _image, data::TransferFunction::sptr _tf)
{
    ::glm::vec4* satBuffer = new ::glm::vec4[m_satSize[0] * m_satSize[1] * m_satSize[2]];

    const auto dumpLock = _image->lock();

    ::glm::vec4 satVal;

    for(int z = 0 ; z < static_cast<int>(m_satSize[2]) ; ++z)
    {
        for(int y = 0 ; y < static_cast<int>(m_satSize[1]) ; ++y)
        {
            for(int x = 0 ; x < static_cast<int>(m_satSize[0]) ; ++x)
            {
                std::int16_t imgValue =
                    _image->at<std::int16_t>(
                        static_cast<size_t>(x),
                        static_cast<size_t>(y),
                        static_cast<size_t>(z)
                    );

                satVal = applyTf(_tf, imgValue)
                         + getSatValue(satBuffer, x - 1, y - 1, z - 1)
                         + getSatValue(satBuffer, x, y, z - 1)
                         + getSatValue(satBuffer, x, y - 1, z)
                         + getSatValue(satBuffer, x - 1, y, z)
                         - getSatValue(satBuffer, x - 1, y - 1, z)
                         - getSatValue(satBuffer, x, y - 1, z - 1)
                         - getSatValue(satBuffer, x - 1, y, z - 1);

                setSatValue(satBuffer, satVal, x, y, z);
            }
        }
    }

    ::Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_sourceBuffer->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pixBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
    ::Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();
    std::uint8_t* pDest     = static_cast<std::uint8_t*>(pixBox.data);

    std::memcpy(pDest, satBuffer, m_satSize[0] * m_satSize[1] * m_satSize[2] * sizeof(::glm::vec4));

    pixBuffer->unlock();

    delete[] satBuffer;
}

//-----------------------------------------------------------------------------

::glm::vec4 SummedAreaTable::applyTf(data::TransferFunction::sptr _tf, int16_t imgValue)
{
    double invWindow = 1. / _tf->getWindow();

    const data::TransferFunction::TFValuePairType intensityMinMax = _tf->getWLMinMax();

    const data::TransferFunction::TFValuePairType tfMinMax = _tf->getMinMaxTFValues();

    double value = (imgValue - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow + tfMinMax.first;

    data::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

    return ::glm::vec4(interpolatedColor.r, interpolatedColor.g, interpolatedColor.b, interpolatedColor.a);
}

//-----------------------------------------------------------------------------

::glm::vec4 SummedAreaTable::getSatValue(::glm::vec4* satBuffer, int x, int y, int z)
{
    if(x < 0 || y < 0 || z < 0)
    {
        return ::glm::vec4(0.f);
    }

    return satBuffer[static_cast<size_t>(x) + m_satSize[0] * static_cast<size_t>(y) + m_satSize[0] * m_satSize[1]
                     * static_cast<size_t>(z)];
}

//-----------------------------------------------------------------------------

void SummedAreaTable::setSatValue(::glm::vec4* satBuffer, glm::vec4 value, int x, int y, int z)
{
    satBuffer[static_cast<size_t>(x) + m_satSize[0] * static_cast<size_t>(y) + m_satSize[0] * m_satSize[1]
              * static_cast<size_t>(z)] = value;
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d
