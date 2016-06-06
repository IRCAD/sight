#include "fwRenderOgre/RayTracingVolumeRenderer.hpp"

#include "fwRenderOgre/helper/Shading.hpp"

#include <OGRE/OgreDepthBuffer.h>
#include <OGRE/OgreHardwareBufferManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreHardwareVertexBuffer.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreSubMesh.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <fwCore/Profiling.hpp>

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
    m_entryPointGeometry(nullptr),
    m_imageSize { 1, 1, 1 },
    m_gridSize  { 2, 2, 2 },
    m_bricksSize{ 8, 8, 8 }
{
    const std::vector<std::string> vrMaterials {
        "RayTracedVolume",
        "PreIntegratedRayTracedVolume"
    };

    m_entryPointsTexture = ::Ogre::TextureManager::getSingletonPtr()->createManual(
                m_parentId + "_entryPointsTexture",
                ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                ::Ogre::TEX_TYPE_2D,
                m_camera->getViewport()->getActualWidth(),
                m_camera->getViewport()->getActualHeight(),
                1,
                0,
                ::Ogre::PF_FLOAT32_RGB,
                ::Ogre::TU_RENDERTARGET );

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

                ::Ogre::TextureUnitState *tex3DState          = pass->getTextureUnitState("image");
                ::Ogre::TextureUnitState *texTFState          = pass->getTextureUnitState("transferFunction");
                ::Ogre::TextureUnitState *texEntryPointsState = pass->getTextureUnitState("entryPoints");

                SLM_ASSERT("'image' texture unit is not found", tex3DState);
                SLM_ASSERT("'transferFunction' texture unit is not found", texTFState);
                SLM_ASSERT("'entryPoints' texture unit is not found", texEntryPointsState);

                tex3DState->setTexture(m_3DOgreTexture);
                texEntryPointsState->setTexture(m_entryPointsTexture);

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

    ::Ogre::RenderTexture *renderTexture = m_entryPointsTexture->getBuffer()->getRenderTarget();
    renderTexture->addViewport(m_camera);

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

void RayTracingVolumeRenderer::imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf)
{
    scaleCube(image->getSpacing());

    const ::fwData::Image::SizeType& newSize = image->getSize();

    // Create new grid texture + proxy geometry if image size changed.
    if(m_imageSize != newSize)
    {
        m_imageSize = newSize;

        for(int i = 0; i < 3; ++ i)
        {
            m_gridSize[i] = m_imageSize[i] / m_bricksSize[i] + (m_imageSize[i] % m_bricksSize[i] != 0);
        }

        if(!m_gridTexture.isNull())
        {
            ::Ogre::TextureManager::getSingletonPtr()->remove(m_gridTexture->getHandle());
        }

        createGridTexture();

        tfUpdate(tf);

        m_proxyGeometryGenerator->manualUpdate();
    }

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable->imageUpdate(image, tf, m_sampleDistance);

        auto minMax = m_preIntegrationTable->getMinMax();

        m_preIntegrationShaderParameters->setNamedConstant("u_min", minMax.first);
        m_preIntegrationShaderParameters->setNamedConstant("u_max", minMax.second);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::tfUpdate(fwData::TransferFunction::sptr tf)
{
    FW_PROFILE("TF Update")
    {
        // Compute grid
        {
            size_t count = m_gridRenderOp.vertexData->vertexCount;
            m_gridRenderOp.vertexData->vertexCount = 4;
            m_gridRenderOp.operationType = ::Ogre::RenderOperation::OT_TRIANGLE_STRIP;

            for(unsigned i = 0; i < m_gridSize[2]; ++ i)
            {
                ::Ogre::RenderTexture *rt = m_gridTexture->getBuffer()->getRenderTarget(i);

                rt->getViewport(0)->clear();

                ::Ogre::MaterialPtr gridMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("VolumeBricksGrid");

                ::Ogre::Pass *gridPass = gridMtl->getTechnique(0)->getPass(0);
                ::Ogre::GpuProgramParametersSharedPtr params = gridPass->getFragmentProgramParameters();

                params->setNamedConstant("u_slice", static_cast<int>(i));

                m_sceneManager->manualRender(
                            &m_gridRenderOp,
                            gridPass,
                            rt->getViewport(0),
                            ::Ogre::Matrix4::IDENTITY,
                            ::Ogre::Matrix4::IDENTITY,
                            ::Ogre::Matrix4::IDENTITY);
            }

            m_gridRenderOp.vertexData->vertexCount = count;
            m_gridRenderOp.operationType = ::Ogre::RenderOperation::OT_POINT_LIST;
        }

        m_proxyGeometryGenerator->manualUpdate();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setSampling(uint16_t nbSamples)
{
    m_nbSlices = nbSamples;

    computeSampleDistance(getCameraPlane());

    m_currentShaderParameters->setNamedConstant("u_sampleDistance", m_sampleDistance);
    m_preIntegrationShaderParameters->setNamedConstant("u_sampleDistance", m_sampleDistance);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setPreIntegratedRendering(bool preIntegratedRendering)
{
    m_preIntegratedRendering = preIntegratedRendering;

    m_currentShaderParameters = m_preIntegratedRendering ? m_preIntegrationShaderParameters : m_defaultShaderParameters;

    m_entryPointGeometry->setMaterialName(0, m_preIntegratedRendering ? "PreIntegratedRayTracedVolume" : "RayTracedVolume");
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::clipImage(const ::Ogre::AxisAlignedBox& clippingBox)
{
    IVolumeRenderer::clipImage(clippingBox);

    const ::Ogre::AxisAlignedBox maxBoxSize(::Ogre::Vector3::ZERO, ::Ogre::Vector3(1.f, 1.f, 1.f));
    const ::Ogre::AxisAlignedBox realClippingBox = maxBoxSize.intersection(clippingBox);

    ::Ogre::MaterialPtr geomMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("VolumeBricks");
    ::Ogre::GpuProgramParametersSharedPtr geomParams = geomMtl->getTechnique(0)->getPass(0)->getGeometryProgramParameters();

    if(realClippingBox.isFinite())
    {
        geomParams->setNamedConstant("u_boundingBoxMin", realClippingBox.getMinimum());
        geomParams->setNamedConstant("u_boundingBoxMax", realClippingBox.getMaximum());
    }
    else if(realClippingBox.isNull())
    {
        geomParams->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3(NAN));
        geomParams->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3(NAN));
    }
    else // Infinite box
    {
        geomParams->setNamedConstant("u_boundingBoxMin", ::Ogre::Vector3::ZERO);
        geomParams->setNamedConstant("u_boundingBoxMax", ::Ogre::Vector3(1.f, 1.f, 1.f));
    }

    m_proxyGeometryGenerator->manualUpdate();
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

    // Create R2VB object used to generate proxy geometry
    {
        ::Ogre::MeshPtr gridMesh = ::Ogre::MeshManager::getSingletonPtr()->createManual(
                    m_parentId + "_gridMesh",
                    ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
        );

        ::Ogre::SubMesh *subMesh = gridMesh->createSubMesh();

        const int nbVtx = m_gridSize[0] * m_gridSize[1] * m_gridSize[2];

        subMesh->useSharedVertices = false;
        subMesh->operationType = ::Ogre::RenderOperation::OT_POINT_LIST;

        subMesh->vertexData = new ::Ogre::VertexData();
        subMesh->vertexData->vertexStart = 0;
        subMesh->vertexData->vertexCount = nbVtx;

        ::Ogre::VertexDeclaration *decl = subMesh->vertexData->vertexDeclaration;

        decl->addElement(0, 0, ::Ogre::VET_INT1, ::Ogre::VES_POSITION);

//        ::Ogre::HardwareVertexBufferSharedPtr vertexBuffer = ::Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
//                    ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
//                    subMesh->vertexData->vertexCount,
//                    ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

//        for(int i = 0; i < subMesh->vertexData->vertexCount; ++ i)
//        {
//            vertexBuffer->writeData(
//                        i * ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
//                        ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
//                        &i,
//                        false);
//        }

//        subMesh->vertexData->vertexBufferBinding->setBinding(0, vertexBuffer);

        gridMesh->_setBounds(::Ogre::AxisAlignedBox::BOX_INFINITE);
        gridMesh->_setBoundingSphereRadius(1000);
        gridMesh->load();

        m_r2vbSource = m_sceneManager->createEntity(gridMesh);

        while(!m_r2vbSource->isInitialised())
        {
            m_r2vbSource->_initialise();
        }

        m_r2vbSource->setVisible(true);


        m_proxyGeometryGenerator = R2VBRenderable::New(
                    m_parentId + "_proxyBricks",
                    m_r2vbSource->getSubEntity(0),
                    m_sceneManager,
                    ::fwData::Mesh::POINT,
                    "RayEntryPoints");

        m_camera->addListener(new CameraListener(this));

        m_r2vbSource->getSubEntity(0)->getRenderOperation(m_gridRenderOp);

        m_volumeSceneNode->attachObject(m_proxyGeometryGenerator);
    }

    // Create grid texture and set parameters in it's associated shader.
    {
        ::Ogre::MaterialPtr gridMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("VolumeBricksGrid");

        ::Ogre::Pass *gridPass = gridMtl->getTechnique(0)->getPass(0);

        ::Ogre::TextureUnitState *tex3DState = gridPass->getTextureUnitState("image");
        ::Ogre::TextureUnitState *texTFState = gridPass->getTextureUnitState("transferFunction");

        SLM_ASSERT("'image' texture unit is not found", tex3DState);
        SLM_ASSERT("'transferFunction' texture unit is not found", texTFState);

        tex3DState->setTexture(m_3DOgreTexture);
        texTFState->setTexture(m_gpuTF->getTexture());

        createGridTexture();
    }

    // Render geometry.
    m_proxyGeometryGenerator->manualUpdate();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::computeEntryPointsTexture()
{
//    m_proxyGeometryGenerator->setMaterial("RayEntryPoints");
    m_proxyGeometryGenerator->setVisible(false);

    ::Ogre::RenderTexture *renderTexture = m_entryPointsTexture->getBuffer()->getRenderTarget();
    renderTexture->getViewport(0)->clear(::Ogre::FBT_COLOUR | ::Ogre::FBT_DEPTH, ::Ogre::ColourValue::White);

    ::Ogre::RenderOperation renderOp;
    m_proxyGeometryGenerator->getRenderOperation(renderOp);
//    m_entryPointGeometry->getSection(0)->getRenderOperation(renderOp);

    ::Ogre::Matrix4 worldMat;
    m_proxyGeometryGenerator->getWorldTransforms(&worldMat);

    ::Ogre::Pass *pass = m_proxyGeometryGenerator->getMaterial()->getTechnique(0)->getPass(0);
    m_sceneManager->manualRender(&renderOp, pass, renderTexture->getViewport(0), worldMat, m_camera->getViewMatrix(), m_camera->getProjectionMatrix());

    // TEST
//    size_t nbPixels = m_entryPointsTexture->getHeight() * m_entryPointsTexture->getWidth();
//    size_t dataSize = nbPixels * 3 * sizeof(float);

//    float *optiText = new float[dataSize];

//    ::Ogre::HardwarePixelBufferSharedPtr textBuffer = m_entryPointsTexture->getBuffer();

//    void *pixBuff = textBuffer->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY);
//    {
//        std::memcpy(optiText, pixBuff, dataSize);
//    }
//    textBuffer->unlock();

//    m_entryPointGeometry->getSection(0)->getRenderOperation(renderOp);
//    m_sceneManager->manualRender(&renderOp, pass, renderTexture->getViewport(0), worldMat, m_camera->getViewMatrix(), m_camera->getProjectionMatrix());

//    float *cubeText = new float[dataSize];

//    textBuffer =  m_entryPointsTexture->getBuffer();
//    pixBuff = textBuffer->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY);
//    {
//        std::memcpy(cubeText, pixBuff, dataSize);
//    }
//    textBuffer->unlock();


//    double totalSkippedDistance = 0;

//    for(size_t i = 0; i < nbPixels * 3; i +=3)
//    {
////        std::cout << "r " << cubeText[i] << " g " << cubeText[i+1] << " b " << cubeText[i + 2] << std::endl;

//        double cubeDist = 0, optiDist = 0;
//        if(optiText[i+1] != 1)
//        {
//            optiDist = -optiText[i+1] - optiText[i];
//        }
//        if(cubeText[i+1] != 1)
//        {
//            cubeDist = -cubeText[i+1] - cubeText[i];
//        }

//        totalSkippedDistance += (cubeDist - optiDist);
//    }

//    std::cout << "Total skipped distance : " << totalSkippedDistance << std::endl;

//    delete[] optiText;
//    delete[] cubeText;
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::createGridTexture()
{
    // Create grid texture and set initialize render targets.
    {
        m_gridTexture = ::Ogre::TextureManager::getSingletonPtr()->createManual(
                    m_parentId + "_gridTexture",
                    ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                    ::Ogre::TEX_TYPE_3D,
                    m_gridSize[0],
                    m_gridSize[1],
                    m_gridSize[2],
                    1,
                    ::Ogre::PF_R8,
                    ::Ogre::TU_RENDERTARGET
        );

        for(unsigned i = 0; i < m_gridSize[2]; ++ i)
        {
            ::Ogre::RenderTexture *rt = m_gridTexture->getBuffer()->getRenderTarget(i);
            rt->addViewport(m_camera);
        }
    }

    // Update R2VB source geometry.
    {
        ::Ogre::MeshPtr r2vbSrcMesh = ::Ogre::MeshManager::getSingletonPtr()->getByName(m_parentId + "_gridMesh");

        ::Ogre::VertexData *meshVtxData = r2vbSrcMesh->getSubMesh(0)->vertexData;

        meshVtxData->vertexCount = m_gridSize[0] * m_gridSize[1] * m_gridSize[2];

        ::Ogre::HardwareVertexBufferSharedPtr vtxBuffer = ::Ogre::HardwareBufferManager::getSingletonPtr()->createVertexBuffer(
                    ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                    meshVtxData->vertexCount,
                    ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        for(int i = 0; i < meshVtxData->vertexCount; ++ i)
        {
            vtxBuffer->writeData(
                        i * ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                        ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                        &i,
                        false);
        }

        meshVtxData->vertexBufferBinding->setBinding(0, vtxBuffer);

        r2vbSrcMesh->load();

        m_r2vbSource->_initialise(true);
        m_r2vbSource->getSubEntity(0)->getRenderOperation(m_gridRenderOp);

        m_proxyGeometryGenerator->setOutputSettings(meshVtxData->vertexCount * 36, false, false, "VolumeBricks");
    }

    // Set shader parameters.
    {
        ::Ogre::MaterialPtr gridMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("VolumeBricksGrid");

        ::Ogre::Pass *gridPass = gridMtl->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr gridGeneratorParams = gridPass->getFragmentProgramParameters();

        gridGeneratorParams->setNamedConstant("u_gridResolution", m_gridSize, 3, 1);
        gridGeneratorParams->setNamedConstant("u_brickSize", m_bricksSize, 3, 1);

        ::Ogre::MaterialPtr geomGeneratorMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("VolumeBricks");

        ::Ogre::Pass *geomGenerationPass = geomGeneratorMtl->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorVtxParams = geomGenerationPass->getVertexProgramParameters();

        geomGeneratorVtxParams->setNamedConstant("u_gridResolution", m_gridSize, 3, 1);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorGeomParams = geomGenerationPass->getGeometryProgramParameters();

        // Cast size_t to int.
        const std::vector<int> imageSize(m_imageSize.begin(), m_imageSize.end());

        geomGeneratorGeomParams->setNamedConstant("u_imageResolution", imageSize.data(), 3, 1);
        geomGeneratorGeomParams->setNamedConstant("u_gridResolution", m_gridSize, 3, 1);
        geomGeneratorGeomParams->setNamedConstant("u_brickSize", m_bricksSize, 3, 1);

        ::Ogre::TextureUnitState *gridTexState = geomGenerationPass->getTextureUnitState("gridVolume");

        SLM_ASSERT("'grid' texture unit is not found", gridTexState);

        gridTexState->setTexture(m_gridTexture);
    }

}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre
