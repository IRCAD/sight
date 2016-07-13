#include "fwRenderOgre/RayTracingVolumeRenderer.hpp"

#include "fwRenderOgre/helper/Shading.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
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

class AutoStereoCompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    AutoStereoCompositorListener(std::vector< ::Ogre::TexturePtr>& renderTargets,
                                std::vector< ::Ogre::Matrix4>& invWorldViewProj,
                                ::Ogre::TexturePtr image3DTexture,
                                ::Ogre::TexturePtr tfTexture,
                                float& sampleDistance) :
        m_renderTargets   (renderTargets),
        m_invWorldViewProj(invWorldViewProj),
        m_image3DTexture  (image3DTexture),
        m_tfTexture       (tfTexture),
        m_sampleDistance  (sampleDistance)
    {

    }

    virtual void notifyMaterialRender(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        ::Ogre::Pass *pass = mtl->getTechnique(0)->getPass(0);

        ::Ogre::TextureUnitState *imageTexUnitState = pass->getTextureUnitState("image");
        ::Ogre::TextureUnitState *tfTexUnitState = pass->getTextureUnitState("transferFunction");

        imageTexUnitState->setTextureName(m_image3DTexture->getName());
        tfTexUnitState->setTextureName(m_tfTexture->getName());

        ::Ogre::GpuProgramParametersSharedPtr vr3DParams = pass->getFragmentProgramParameters();

        vr3DParams->setNamedConstant("u_invWorldViewProjs", m_invWorldViewProj.data(), m_invWorldViewProj.size());
        vr3DParams->setNamedConstant("u_sampleDistance", m_sampleDistance);

        for(unsigned i = 0; i < m_renderTargets.size(); ++ i)
        {
            ::Ogre::TextureUnitState *texUnitState = pass->getTextureUnitState("entryPoints" + std::to_string(i));

            OSLM_ASSERT("No texture named " << i, texUnitState);
            texUnitState->setTextureName(m_renderTargets[i]->getName());
        }
    }

private:

    std::vector< ::Ogre::TexturePtr>& m_renderTargets;

    std::vector< ::Ogre::Matrix4>& m_invWorldViewProj;

    ::Ogre::TexturePtr m_image3DTexture;

    ::Ogre::TexturePtr m_tfTexture;

    float& m_sampleDistance;

};

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::RayTracingVolumeRenderer(std::string parentId,
                                                   ::Ogre::SceneManager *sceneManager,
                                                   ::Ogre::SceneNode *parentNode,
                                                   ::Ogre::TexturePtr imageTexture,
                                                   TransferFunction *gpuTF,
                                                   PreIntegrationTable *preintegrationTable,
                                                   bool mode3D) :
    IVolumeRenderer(parentId, sceneManager, parentNode, imageTexture, gpuTF, preintegrationTable),
    m_entryPointGeometry(nullptr),
    m_imageSize { 1, 1, 1 },
    m_gridSize  { 2, 2, 2 },
    m_bricksSize{ 8, 8, 8 },
    m_mode3D    (mode3D)
{
    const std::vector<std::string> vrMaterials {
        "RayTracedVolume",
        "PreIntegratedRayTracedVolume"
    };

    unsigned nbViewpoints = m_mode3D ? 8 : 1;

    for(unsigned i = 0; i < nbViewpoints; ++ i)
    {
        m_entryPointsTextures.push_back( ::Ogre::TextureManager::getSingletonPtr()->createManual(
                    m_parentId + "_entryPointsTexture" + std::to_string(i),
                    ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                    ::Ogre::TEX_TYPE_2D,
                    m_camera->getViewport()->getActualWidth(),
                    m_camera->getViewport()->getActualHeight(),
                    1,
                    0,
                    ::Ogre::PF_FLOAT32_RGB,
                    ::Ogre::TU_RENDERTARGET ));
    }

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
                texEntryPointsState->setTexture(m_entryPointsTextures[0]);

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

    for(::Ogre::TexturePtr entryPtsText : m_entryPointsTextures)
    {
        ::Ogre::RenderTexture *renderTexture = entryPtsText->getBuffer()->getRenderTarget();
        renderTexture->addViewport(m_camera);
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
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setPreIntegratedRendering(bool preIntegratedRendering)
{
    OSLM_WARN_IF("Stereoscopic rendering doesn't implement pre-integration", m_mode3D && preIntegratedRendering);

    m_preIntegratedRendering = preIntegratedRendering;

    m_currentShaderParameters = m_preIntegratedRendering ? m_preIntegrationShaderParameters : m_defaultShaderParameters;

    m_currentShaderParameters->setNamedConstant("u_sampleDistance", m_sampleDistance);

    m_entryPointGeometry->setMaterialName(0, m_preIntegratedRendering ? "PreIntegratedRayTracedVolume" : "RayTracedVolume");
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::configure3DViewport(Layer::sptr layer)
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    compositorManager.addCompositor(layer->getViewport(), "RayTracedVolume3D");
    compositorManager.setCompositorEnabled(layer->getViewport(), "RayTracedVolume3D", true);

    ::Ogre::CompositorChain *compChain = ::Ogre::CompositorManager::getSingleton().getCompositorChain(layer->getViewport());

    ::Ogre::CompositorInstance *compInstance = compChain->getCompositor("RayTracedVolume3D");

    compInstance->addListener(new AutoStereoCompositorListener(m_entryPointsTextures,
                                                              m_viewPointMatrices,
                                                              m_3DOgreTexture,
                                                              m_gpuTF->getTexture(),
                                                              m_sampleDistance));
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

    m_proxyGeometryGenerator->manualUpdate();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::resizeViewport(int w, int h)
{
    for(::Ogre::TexturePtr entryPtsTexture : m_entryPointsTextures)
    {
        entryPtsTexture->freeInternalResources();

        entryPtsTexture->setWidth(static_cast< ::Ogre::uint32>(w));
        entryPtsTexture->setHeight(static_cast< ::Ogre::uint32>(h));

        entryPtsTexture->createInternalResources();

        ::Ogre::RenderTexture *renderTexture = entryPtsTexture->getBuffer()->getRenderTarget();
        renderTexture->addViewport(m_camera);
    }
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

    m_entryPointGeometry->setVisible(!m_mode3D);

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

    ::Ogre::Pass *pass = m_proxyGeometryGenerator->getMaterial()->getTechnique(0)->getPass(0);

    ::Ogre::RenderOperation renderOp;
//    m_proxyGeometryGenerator->getRenderOperation(renderOp);
    m_entryPointGeometry->getSection(0)->getRenderOperation(renderOp);

    ::Ogre::Matrix4 worldMat;
    m_proxyGeometryGenerator->getWorldTransforms(&worldMat);

    const float eyeAngle = 0.01625;
    float angle = eyeAngle * -3.5;

    m_viewPointMatrices.clear();

    for(::Ogre::TexturePtr entryPtsText : m_entryPointsTextures)
    {
        ::Ogre::Matrix4 projMat = m_camera->getProjectionMatrix();

        ::Ogre::RenderTexture *renderTexture = entryPtsText->getBuffer()->getRenderTarget();
        renderTexture->getViewport(0)->clear(::Ogre::FBT_COLOUR | ::Ogre::FBT_DEPTH, ::Ogre::ColourValue::White);

        if(m_mode3D)
        {
            const ::Ogre::Matrix4 shearTransform = frustumShearTransform(angle);

            angle += eyeAngle;
            projMat = projMat * shearTransform;

            ::Ogre::Matrix4 worldViewProj = projMat * m_camera->getViewMatrix() * worldMat;
            m_viewPointMatrices.push_back(worldViewProj.inverse());
        }

        m_sceneManager->manualRender(&renderOp, pass, renderTexture->getViewport(0), worldMat, m_camera->getViewMatrix(), projMat);
    }

//    m_proxyGeometryGenerator->setMaterial("Default");

    // TEST
//    size_t nbPixels = m_entryPointsTextures[0]->getHeight() * m_entryPointsTextures[0]->getWidth();
//    size_t dataSize = nbPixels * 3 * sizeof(float);

//    float *optiText = new float[dataSize];

//    ::Ogre::HardwarePixelBufferSharedPtr textBuffer = m_entryPointsTextures[0]->getBuffer();

//    void *pixBuff = textBuffer->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY);
//    {
//        std::memcpy(optiText, pixBuff, dataSize);
//    }
//    textBuffer->unlock();

//    ::Ogre::RenderTexture *renderTexture = m_entryPointsTextures[0]->getBuffer()->getRenderTarget();

//    m_entryPointGeometry->getSection(0)->getRenderOperation(renderOp);
//    m_sceneManager->manualRender(&renderOp, pass, renderTexture->getViewport(0), worldMat, m_camera->getViewMatrix(), m_camera->getProjectionMatrix());

//    float *cubeText = new float[dataSize];

//    textBuffer =  m_entryPointsTextures[0]->getBuffer();
//    pixBuff = textBuffer->lock(::Ogre::HardwareBuffer::HBL_READ_ONLY);
//    {
//        std::memcpy(cubeText, pixBuff, dataSize);
//    }
//    textBuffer->unlock();


//    double totalSkippedDistance = 0;

//    double cubeDist = 0, optiDist = 0;

//    for(size_t i = 0; i < nbPixels * 3; i +=3)
//    {
////        std::cout << "r " << cubeText[i] << " g " << cubeText[i+1] << " b " << cubeText[i + 2] << std::endl;

//        if(optiText[i+1] != 1)
//        {
//            optiDist += -optiText[i+1] - optiText[i];
//        }
//        if(cubeText[i+1] != 1)
//        {
//            cubeDist += -cubeText[i+1] - cubeText[i];
//        }

////        totalSkippedDistance += (cubeDist - optiDist);
//    }

//    std::cout << "Total skipped distance : " << optiDist / cubeDist << std::endl;

//    delete[] optiText;
//        delete[] cubeText;
}

//-----------------------------------------------------------------------------

Ogre::Matrix4 RayTracingVolumeRenderer::frustumShearTransform(float angle) const
{
    ::Ogre::Matrix4 shearTransform = ::Ogre::Matrix4::IDENTITY;

    const float focalLength  = m_camera->getFocalLength();
    const float xshearFactor = std::tan(angle);

    shearTransform[0][2] = -xshearFactor;
    shearTransform[0][3] = -focalLength * xshearFactor;

    return shearTransform;
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
