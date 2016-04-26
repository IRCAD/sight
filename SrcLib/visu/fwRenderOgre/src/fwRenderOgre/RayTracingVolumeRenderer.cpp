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
        "RayTracedVolume"
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

                if(mtlName == "PreIntegratedSliceVolume")
                {
//                    m_preIntegrationTableTexState = texTFState;
                    texTFState->setTexture(m_preIntegrationTable->getTexture());
                    m_preIntegrationParameters = pass->getFragmentProgramParameters();
                }
                else
                {
                    texTFState->setTexture(m_gpuTF->getTexture());
                    m_preIntegrationParameters = pass->getFragmentProgramParameters();
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

void RayTracingVolumeRenderer::updateGeometry()
{
    updateEntryPoints();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::imageUpdate(fwData::Image::sptr image, fwData::TransferFunction::sptr tf)
{
    scaleCube(image->getSpacing());
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setSampling(uint16_t nbSamples)
{
    m_nbSlices = nbSamples;

    const ::Ogre::Vector3 cameraPosition = m_volumeSceneNode->convertWorldToLocalPosition(m_camera->getRealPosition());
    ::Ogre::Vector3 planeNormal = m_volumeSceneNode->convertWorldToLocalDirection(m_camera->getRealDirection(), true);
    planeNormal.normalise();

    const ::Ogre::Plane cameraPlane(planeNormal, cameraPosition);

    // get the cube's closest and furthest vertex to the camera
    const auto comp = [&cameraPlane](const ::Ogre::Vector3& v1, const ::Ogre::Vector3& v2)
            { return cameraPlane.getDistance(v1) < cameraPlane.getDistance(v2); };

    const ::Ogre::Vector3 furthestVtx = *std::max_element(m_clippedImagePositions, m_clippedImagePositions + 8, comp);
    const ::Ogre::Vector3 closestVtx  = *std::min_element(m_clippedImagePositions, m_clippedImagePositions + 8, comp);

    // get distance between slices
    const float closestVtxDistance  = cameraPlane.getDistance(closestVtx);
    const float furthestVtxDistance = cameraPlane.getDistance(furthestVtx);

    const float firstToLastSliceDistance = std::abs(closestVtxDistance - furthestVtxDistance);

    const float sliceDistance =  firstToLastSliceDistance / m_nbSlices;

//    if(m_preIntegratedRendering)
//    {
        m_preIntegrationParameters->setNamedConstant("u_sampleDistance", sliceDistance);
//    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setPreIntegratedRendering(bool preIntegratedRendering)
{

}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::clipImage(const ::Ogre::AxisAlignedBox& clippingBox)
{
    IVolumeRenderer::clipImage(clippingBox);

    const ::Ogre::AxisAlignedBox maxBoxSize(::Ogre::Vector3::ZERO, ::Ogre::Vector3(1.f, 1.f, 1.f));
    const ::Ogre::AxisAlignedBox realClippingBox = maxBoxSize.intersection(clippingBox);

    if(realClippingBox.isFinite())
    {
        m_preIntegrationParameters->setNamedConstant("u_boundingBoxMin", realClippingBox.getMinimum());
        m_preIntegrationParameters->setNamedConstant("u_boundingBoxMax", realClippingBox.getMaximum());
    }
    else if(realClippingBox.isNull())
    {
        m_preIntegrationParameters->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3(NAN));
        m_preIntegrationParameters->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3(NAN));
    }
    else // Infinite extent
    {
        m_preIntegrationParameters->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3::ZERO);
        m_preIntegrationParameters->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3(1.f, 1.f, 1.f));
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::initEntryPoints()
{
    m_entryPointGeometry = m_sceneManager->createManualObject(m_parentId + "_RayTracingVREntryPoints");

    m_entryPointGeometry->begin("RayTracedVolume", ::Ogre::RenderOperation::OT_TRIANGLE_LIST);
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
