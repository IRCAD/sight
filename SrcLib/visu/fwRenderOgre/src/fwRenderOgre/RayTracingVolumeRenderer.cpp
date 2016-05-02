#include "fwRenderOgre/RayTracingVolumeRenderer.hpp"

#include "fwRenderOgre/helper/Shading.hpp"

#include <OGRE/OgreMaterialManager.h>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::RayTracingVolumeRenderer(std::string parentId,
                                                   ::Ogre::SceneManager *sceneManager,
                                                   ::Ogre::SceneNode *parentNode,
                                                   ::Ogre::TexturePtr imageTexture,
                                                   TransferFunction *gpuTF,
                                                   PreIntegrationTable *preintegrationTable) :
    IVolumeRenderer(parentId, sceneManager, parentNode, imageTexture, gpuTF, preintegrationTable),
    m_entryPointGeometry(nullptr)
{
    const std::vector<std::string> vrMaterials {
        "RayTracedVolume",
        "PreIntegratedRayTracedVolume"
    };

    for(const std::string& mtlName : vrMaterials)
    {
        ::Ogre::MaterialPtr volumeMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName(mtlName);
        ::Ogre::Material::TechniqueIterator techIt = volumeMtl->getTechniqueIterator();

        while( techIt.hasMoreElements())
        {
            ::Ogre::Technique* tech = techIt.getNext();
            SLM_ASSERT("Technique is not set", tech);

            if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
            {
                ::Ogre::Pass *pass = tech->getPass(0);

                ::Ogre::TextureUnitState *tex3DState = pass->getTextureUnitState("image");
                ::Ogre::TextureUnitState *texTFState = pass->getTextureUnitState("transferFunction");

                SLM_ASSERT("'image' texture unit is not found", tex3DState);
                SLM_ASSERT("'transferFunction' texture unit is not found", texTFState);

                tex3DState->setTexture(m_3DOgreTexture);

                if(mtlName == "PreIntegratedRayTracedVolume")
                {
                    texTFState->setTexture(m_preIntegrationTable->getTexture());
                    m_preIntegrationShaderParameters = pass->getFragmentProgramParameters();
                }
                else
                {
                    texTFState->setTexture(m_gpuTF->getTexture());
                    m_defaultShaderParameters = pass->getFragmentProgramParameters();
                    m_currentShaderParameters = m_defaultShaderParameters;
                }
            }
        }
    }

    initEntryPoints();
    setSampling(m_nbSlices);
}

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::~RayTracingVolumeRenderer()
{
    m_volumeSceneNode->detachObject(m_entryPointGeometry);
    m_sceneManager->destroyManualObject(m_entryPointGeometry->getName());
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::imageUpdate(fwData::Image::sptr image, fwData::TransferFunction::sptr tf)
{
    scaleCube(image->getSpacing());

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable->imageUpdate(image, tf, m_sampleDistance);

        auto minMax = m_preIntegrationTable->getMinMax();

        m_preIntegrationShaderParameters->setNamedConstant("u_min", minMax.first);
        m_preIntegrationShaderParameters->setNamedConstant("u_max", minMax.second);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setSampling(uint16_t nbSamples)
{
    m_nbSlices = nbSamples;

    computeSampleDistance(getCameraPlane());

    m_currentShaderParameters->setNamedConstant("u_sampleDistance", m_sampleDistance);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setPreIntegratedRendering(bool preIntegratedRendering)
{
    m_preIntegratedRendering = preIntegratedRendering;

    m_currentShaderParameters = m_preIntegratedRendering ? m_preIntegrationShaderParameters : m_defaultShaderParameters;

    const std::string mtlName = m_preIntegratedRendering ? "PreIntegratedRayTracedVolume" : "RayTracedVolume";

    m_entryPointGeometry->setMaterialName(0, mtlName);

    setSampling(m_nbSlices);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::clipImage(const ::Ogre::AxisAlignedBox& clippingBox)
{
    IVolumeRenderer::clipImage(clippingBox);

    const ::Ogre::AxisAlignedBox maxBoxSize(::Ogre::Vector3::ZERO, ::Ogre::Vector3(1.f, 1.f, 1.f));
    const ::Ogre::AxisAlignedBox realClippingBox = maxBoxSize.intersection(clippingBox);

    if(realClippingBox.isFinite())
    {
        m_currentShaderParameters->setNamedConstant("u_boundingBoxMin", realClippingBox.getMinimum());
        m_currentShaderParameters->setNamedConstant("u_boundingBoxMax", realClippingBox.getMaximum());
    }
    else if(realClippingBox.isNull())
    {
        m_currentShaderParameters->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3(NAN));
        m_currentShaderParameters->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3(NAN));
    }
    else // Infinite box
    {
        m_currentShaderParameters->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3::ZERO);
        m_currentShaderParameters->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3(1.f, 1.f, 1.f));
    }

    updateEntryPoints();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::initEntryPoints()
{
    const std::string mtlName = m_preIntegratedRendering ? "PreIntegratedRayTracedVolume" : "RayTracedVolume";

    m_entryPointGeometry = m_sceneManager->createManualObject(m_parentId + "_RayTracingVREntryPoints");

    m_entryPointGeometry->begin(mtlName, ::Ogre::RenderOperation::OT_TRIANGLE_LIST);
    {
        for(const auto& face : s_cubeFaces)
        {
            const CubeFacePositionList& facePositionList = face.second;

            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[0]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[1]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[2]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[2]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[3]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[0]]);
        }
    }
    m_entryPointGeometry->end();

    m_volumeSceneNode->attachObject(m_entryPointGeometry);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateEntryPoints()
{
    m_entryPointGeometry->beginUpdate(0);
    {
        for(const auto& face : s_cubeFaces)
        {
            const CubeFacePositionList& facePositionList = face.second;

            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[0]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[1]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[2]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[2]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[3]]);
            m_entryPointGeometry->position(m_clippedImagePositions[facePositionList[0]]);
        }
    }
    m_entryPointGeometry->end();
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
