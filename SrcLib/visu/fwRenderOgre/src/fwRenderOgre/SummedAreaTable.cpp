#include "fwRenderOgre/SummedAreaTable.hpp"

#include "fwRenderOgre/Utils.hpp"

#include <math.h>

#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreViewport.h>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

class SummedAreaTableInitCompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    SummedAreaTableInitCompositorListener(float& currentSliceIndex) :
        m_currentSliceDepth(currentSliceIndex)
    {
    }

    virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass *satInitPass = mat->getTechnique(0)->getPass(0);

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

    SummedAreaTableCompositorListener(int& readOffset, int& passOrientation, int& currentSliceIndex) :
        m_readOffset(readOffset),
        m_passOrientation(passOrientation),
        m_currentSliceIndex(currentSliceIndex)
    {
    }

    virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass *satPass = mat->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr satPassParams = satPass->getFragmentProgramParameters();

        satPassParams->setNamedConstant("u_readOffset", m_readOffset);
        satPassParams->setNamedConstant("u_passOrientation", m_passOrientation);
        satPassParams->setNamedConstant("u_sliceIndex", m_currentSliceIndex);
    }

private:
    int& m_readOffset;

    int& m_passOrientation;

    int& m_currentSliceIndex;
};

//-----------------------------------------------------------------------------

SummedAreaTable::SummedAreaTable(::Ogre::SceneManager *_sceneManager, int width, int height, int depth) :
    m_sceneManager(_sceneManager),
    m_satSize     { width, height, depth }
{
    m_sourceBuffer = ::Ogre::TextureManager::getSingletonPtr()->createManual(
                "__GPU_SummedAreaTable_Ping",
                ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                ::Ogre::TEX_TYPE_3D,
                width,
                height,
                depth,
                0,
                ::Ogre::PF_FLOAT32_RGBA,
                ::Ogre::TU_RENDERTARGET );

    m_targetBuffer = ::Ogre::TextureManager::getSingletonPtr()->createManual(
                "__GPU_SummedAreaTable_Pong",
                ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                ::Ogre::TEX_TYPE_3D,
                width,
                height,
                depth,
                0,
                ::Ogre::PF_FLOAT32_RGBA,
                ::Ogre::TU_RENDERTARGET );

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    SummedAreaTableInitCompositorListener *satInitListener =
            new SummedAreaTableInitCompositorListener(m_currentSliceDepth);

    SummedAreaTableCompositorListener *satListener =
            new SummedAreaTableCompositorListener(m_readOffset, m_passOrientation, m_sliceIndex);

    ::Ogre::Camera *dummyCamera = m_sceneManager->createCamera("_SummedAreaTable_DummyCamera");

    for(int sliceIndex = 0; sliceIndex < depth; ++ sliceIndex)
    {
        // Init source buffer
        ::Ogre::RenderTarget *renderTarget = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex);
        ::Ogre::Viewport *vp = renderTarget->addViewport(dummyCamera);

        vp->setOverlaysEnabled(false);

        compositorManager.addCompositor(vp, "SummedAreaTableInit");
        compositorManager.addCompositor(vp, "SummedAreaTable");

        ::Ogre::CompositorInstance *compInstance = compositorManager.getCompositorChain(vp)->getCompositor("SummedAreaTableInit");
        compInstance->addListener(satInitListener);

        compInstance = compositorManager.getCompositorChain(vp)->getCompositor("SummedAreaTable");
        compInstance->addListener(satListener);

        // Init target buffer
        renderTarget = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex);
        vp = renderTarget->addViewport(dummyCamera);

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

SummedAreaTable::~SummedAreaTable()
{
}

//-----------------------------------------------------------------------------

void SummedAreaTable::computeParallel(::Ogre::TexturePtr _imgTexture, Ogre::TexturePtr _gpuTf)
{

    ::Ogre::MaterialPtr initPassMtl = ::Ogre::MaterialManager::getSingleton().getByName("SummedAreaTableInit");
    ::Ogre::Pass *satInitPass = initPassMtl->getTechnique(0)->getPass(0);

    ::Ogre::GpuProgramParametersSharedPtr initPassParams = satInitPass->getFragmentProgramParameters();

    ::Ogre::TextureUnitState *tex3DState = satInitPass->getTextureUnitState("image");
    ::Ogre::TextureUnitState *texTFState = satInitPass->getTextureUnitState("transferFunction");

    SLM_ASSERT("'image' texture unit is not found", tex3DState);
    SLM_ASSERT("'transferFunction' texture unit is not found", texTFState);

    tex3DState->setTexture(_imgTexture);
    texTFState->setTexture(_gpuTf);

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    const int depth = m_satSize[2];

    for(int sliceIndex = 0; sliceIndex < depth; ++ sliceIndex)
    {
        ::Ogre::Viewport *vp = m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTableInit", true);

        m_currentSliceDepth = static_cast<float>(sliceIndex) / depth;

        m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex)->update(false);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTableInit", false);
    }

    // Enable SAT compositor
    for(int sliceIndex = 0; sliceIndex < depth; ++ sliceIndex)
    {
        ::Ogre::Viewport *vp =  m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTable", true);

        vp =  m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTable", true);
    }

    ::Ogre::MaterialPtr satMtl = ::Ogre::MaterialManager::getSingleton().getByName("SummedAreaTable");
    ::Ogre::Pass *satPass = satMtl->getTechnique(0)->getPass(0);
    ::Ogre::TextureUnitState *srcImgState = satPass->getTextureUnitState("source");

    for(m_passOrientation = 0; m_passOrientation < 3; ++m_passOrientation)
    {
        const int dim = m_satSize[m_passOrientation];
        const int nbPass = static_cast<int>(std::ceil(std::log(dim) / std::log(m_nbTextReads)));

        m_readOffset = 1;

        for(int passIndex = 0; passIndex < nbPass; ++passIndex)
        {
            srcImgState->setTexture(m_sourceBuffer);

            for(m_sliceIndex = 0; m_sliceIndex < depth; ++ m_sliceIndex)
            {
                m_targetBuffer->getBuffer()->getRenderTarget(m_sliceIndex)->update(false);
            }

            m_readOffset *= m_nbTextReads;

            // Swap ping pong
            std::swap(m_sourceBuffer, m_targetBuffer);
        }
    }

    // Disable SAT compositor
    for(int sliceIndex = 0; sliceIndex < depth; ++ sliceIndex)
    {
        ::Ogre::Viewport *vp =  m_sourceBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTable", false);

        vp = m_targetBuffer->getBuffer()->getRenderTarget(sliceIndex)->getViewport(0);

        compositorManager.setCompositorEnabled(vp, "SummedAreaTable", false);
    }

}

//-----------------------------------------------------------------------------

void SummedAreaTable::computeSequential(::fwData::Image::sptr _image, fwData::TransferFunction::sptr _tf)
{
    m_satBuffer = new ::glm::vec4[m_satSize[0] * m_satSize[1] * m_satSize[2]];

    if(m_gpuSummedAreaTable.isNull())
    {
        m_gpuSummedAreaTable = ::Ogre::TextureManager::getSingletonPtr()->createManual(
                    "__CPU_SummedAreaTable",
                    ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                    ::Ogre::TEX_TYPE_3D,
                    m_satSize[0],
                    m_satSize[1],
                    m_satSize[2],
                    0,
                    ::Ogre::PF_FLOAT32_RGBA,
                    ::Ogre::TU_DYNAMIC_WRITE_ONLY );
    }

    ::fwComEd::helper::Image imageHelper(_image);

    ::glm::vec4 satVal;

    for(int z = 0; z < m_satSize[2]; ++ z)
    {
        for(int y = 0; y < m_satSize[1]; ++y)
        {
            for(int x = 0; x < m_satSize[0]; ++ x)
            {
                int16_t *imgValue = static_cast<int16_t*>(imageHelper.getPixelBuffer(x, y, z));

                satVal = applyTf(_tf, *imgValue) + getSatValue(x-1,y-1,z-1) +
                         getSatValue( x,y,z-1 ) + getSatValue( x, y-1, z ) + getSatValue( x-1, y, z )
                         - getSatValue( x-1, y-1, z ) - getSatValue( x, y-1, z-1 ) - getSatValue( x-1, y, z-1 );

                setSatValue(satVal, x, y, z);
            }
        }
    }

    std::cout << "img type: " << _image->getPixelType().string()<< std::endl;

    ::Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_gpuSummedAreaTable->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pixBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
    ::Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();
    std::uint8_t* pDest = static_cast<std::uint8_t*>(pixBox.data);

    std::memcpy(pDest, m_satBuffer, m_satSize[0] * m_satSize[1] * m_satSize[2] * sizeof(::glm::vec4));

    pixBuffer->unlock();

    delete[] m_satBuffer;
}

//-----------------------------------------------------------------------------

::glm::vec4 SummedAreaTable::applyTf(fwData::TransferFunction::sptr _tf, int16_t imgValue)
{
    double invWindow = 1. / _tf->getWindow();

    const ::fwData::TransferFunction::TFValuePairType intensityMinMax = _tf->getWLMinMax();

    const ::fwData::TransferFunction::TFValuePairType tfMinMax = _tf->getMinMaxTFValues();

    double value = (imgValue - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow + tfMinMax.first;

    ::fwData::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

    return ::glm::vec4(interpolatedColor.r, interpolatedColor.g, interpolatedColor.b, interpolatedColor.a);
}

//-----------------------------------------------------------------------------

::glm::vec4 SummedAreaTable::getSatValue(int x, int y, int z)
{
    if(x < 0 || y < 0 || z < 0)
    {
        return ::glm::vec4(0.f);
    }

    return m_satBuffer[x + m_satSize[0] * y + m_satSize[0] * m_satSize[1] * z];
}

//-----------------------------------------------------------------------------

void SummedAreaTable::setSatValue(glm::vec4 value, int x, int y, int z)
{
    m_satBuffer[x + m_satSize[0] * y + m_satSize[0] * m_satSize[1] * z] = value;
}

//-----------------------------------------------------------------------------

}
