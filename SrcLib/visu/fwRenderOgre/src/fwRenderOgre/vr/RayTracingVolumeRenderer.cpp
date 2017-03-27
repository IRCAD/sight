/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/vr/RayTracingVolumeRenderer.hpp"

#include "fwRenderOgre/helper/Shading.hpp"
#include "fwRenderOgre/SRender.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <fwCore/Profiling.hpp>

#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreDepthBuffer.h>
#include <OGRE/OgreHardwareBufferManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreHardwareVertexBuffer.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSubMesh.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <algorithm>
#include <string>

namespace fwRenderOgre
{

namespace vr
{

//-----------------------------------------------------------------------------

struct RayTracingVolumeRenderer::CameraListener : public ::Ogre::Camera::Listener
{
    RayTracingVolumeRenderer* m_renderer;
    std::string m_currentMtlName;
    int m_frameId;

    CameraListener(RayTracingVolumeRenderer* renderer) :
        m_renderer(renderer),
        m_currentMtlName("VolIllum"),
        m_frameId(0)
    {
    }

    //------------------------------------------------------------------------------

    virtual void cameraPreRenderScene(::Ogre::Camera*)
    {
        auto layer = m_renderer->getLayer();
        if(layer)
        {
            const int frameId = layer->getRenderService()->getInteractorManager()->getFrameId();
            if(frameId != m_frameId)
            {
                if(m_renderer->m_illumVolume && m_renderer->m_shadows)
                {
                    // Set light directions in shader.
                    ::Ogre::LightList closestLights =
                        m_renderer->m_volumeSceneNode->getAttachedObject(0)->queryLights();

                    ::Ogre::Vector3 lightDir = m_renderer->m_volumeSceneNode->convertLocalToWorldDirection(
                        closestLights[0]->getDerivedDirection(), true);

                    ::Ogre::Pass* satIllumPass = ::Ogre::MaterialManager::getSingleton().getByName(
                        m_currentMtlName)->getTechnique(0)->getPass(0);
                    ::Ogre::GpuProgramParametersSharedPtr satIllumParams = satIllumPass->getFragmentProgramParameters();

                    satIllumParams->setNamedConstant("u_lightDir", lightDir);

                    m_renderer->m_illumVolume->updateVolIllum();
                }
                // Recompute the focal length in case the camera moved.

                m_renderer->computeEntryPointsTexture();

                m_frameId = frameId;
            }
        }
    }

    //------------------------------------------------------------------------------

    void setCurrentMtlName(const std::string& currentMtlName)
    {
        m_currentMtlName = currentMtlName;
    }
};

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::RayTracingVolumeRenderer(std::string parentId,
                                                   Layer::sptr layer,
                                                   ::Ogre::SceneNode* parentNode,
                                                   ::Ogre::TexturePtr imageTexture,
                                                   TransferFunction& gpuTF,
                                                   PreIntegrationTable& preintegrationTable,
                                                   ::fwRenderOgre::Layer::StereoModeType mode3D,
                                                   bool ambientOcclusion,
                                                   bool colorBleeding,
                                                   bool shadows,
                                                   double aoFactor,
                                                   double colorBleedingFactor) :
    IVolumeRenderer(parentId, layer->getSceneManager(), parentNode, imageTexture, gpuTF, preintegrationTable),
    m_entryPointGeometry(nullptr),
    m_imageSize(::fwData::Image::SizeType({ 1, 1, 1 })),
    m_stereoMode(mode3D),
    m_ambientOcclusion(ambientOcclusion),
    m_colorBleeding(colorBleeding),
    m_shadows(shadows),
    m_aoFactor(aoFactor),
    m_colorBleedingFactor(colorBleedingFactor),
    m_illumVolume(nullptr),
    m_cameraListener(nullptr),
    m_layer(layer)
{
    m_gridSize   = {{ 2, 2, 2 }};
    m_bricksSize = {{ 8, 8, 8 }};

    const std::string vrMaterials[16]
    {
        "RayTracedVolume",
        "RayTracedVolume_AmbientOcclusion",
        "RayTracedVolume_ColorBleeding",
        "RayTracedVolume_Shadows",
        "RayTracedVolume_AmbientOcclusion_ColorBleeding",
        "RayTracedVolume_AmbientOcclusion_Shadows",
        "RayTracedVolume_ColorBleeding_Shadows",
        "RayTracedVolume_VolumeIllumination",
        "RayTracedVolume_PreIntegrated",
        "RayTracedVolume_PreIntegrated_AmbientOcclusion",
        "RayTracedVolume_PreIntegrated_ColorBleeding",
        "RayTracedVolume_PreIntegrated_Shadows",
        "RayTracedVolume_PreIntegrated_AmbientOcclusion_ColorBleeding",
        "RayTracedVolume_PreIntegrated_AmbientOcclusion_Shadows",
        "RayTracedVolume_PreIntegrated_ColorBleeding_Shadows",
        "RayTracedVolume_PreIntegrated_VolumeIllumination"
    };

    const unsigned int numViewPoints = m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8 ? 8 :
                                       m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5 ? 5 : 1;

    const float wRatio = numViewPoints != 1 ? 3.f / numViewPoints : 1.f;
    const float hRatio = numViewPoints != 1 ? 0.5f : 1.f;

    for(unsigned int i = 0; i < numViewPoints; ++i)
    {
        m_entryPointsTextures.push_back(::Ogre::TextureManager::getSingleton().createManual(
                                            m_parentId + "_entryPointsTexture" + std::to_string(i),
                                            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                            ::Ogre::TEX_TYPE_2D,
                                            static_cast<unsigned int>(m_camera->getViewport()->getActualWidth() *
                                                                      wRatio),
                                            static_cast<unsigned int>(m_camera->getViewport()->getActualHeight() *
                                                                      hRatio),
                                            1,
                                            0,
                                            ::Ogre::PF_FLOAT32_RGB,
                                            ::Ogre::TU_RENDERTARGET ));
    }

    if(m_stereoMode != ::fwRenderOgre::Layer::StereoModeType::NONE)
    {
        m_autostereoListener = new compositor::listener::AutoStereoCompositorListener(numViewPoints, *m_camera,
                                                                                      &m_entryPointsTextures);
        ::Ogre::MaterialManager::getSingleton().addListener(m_autostereoListener);
    }

    for(const std::string& mtlName : vrMaterials)
    {
        ::Ogre::MaterialPtr volumeMtl              = ::Ogre::MaterialManager::getSingleton().getByName(mtlName);
        ::Ogre::Material::TechniqueIterator techIt = volumeMtl->getTechniqueIterator();

        while( techIt.hasMoreElements())
        {
            ::Ogre::Technique* tech = techIt.getNext();
            SLM_ASSERT("Technique is not set", tech);

            if(::fwRenderOgre::helper::Shading::isColorTechnique(*tech))
            {
                ::Ogre::Pass* pass = tech->getPass(0);

                ::Ogre::TextureUnitState* tex3DState          = pass->getTextureUnitState("image");
                ::Ogre::TextureUnitState* texTFState          = pass->getTextureUnitState("transferFunction");
                ::Ogre::TextureUnitState* texEntryPointsState = pass->getTextureUnitState("entryPoints");
                ::Ogre::TextureUnitState* texIlluminationVol  = pass->getTextureUnitState("illuminationVolume");

                // Keep the texture unit states for materials "VolumeIlluminationRayTracedVolume" and
                // "PreIntegratedVolumeIlluminationRayTracedVolume" to reuse them later when the illuminationVolume
                // texture will be computed
                if(texIlluminationVol)
                {
                    m_rayTracedTexUnitStates.push_back(texIlluminationVol);
                }

                SLM_ASSERT("'image' texture unit is not found", tex3DState);
                SLM_ASSERT("'transferFunction' texture unit is not found", texTFState);
                SLM_ASSERT("'entryPoints' texture unit is not found", texEntryPointsState);

                tex3DState->setTexture(m_3DOgreTexture);
                texEntryPointsState->setTexture(m_entryPointsTextures[0]);

                if(mtlName.find("PreIntegrated") != std::string::npos)
                {
                    texTFState->setTexture(m_preIntegrationTable.getTexture());
                }
                else
                {
                    texTFState->setTexture(m_gpuTF.getTexture());
                }
            }
        }
    }

    for(::Ogre::TexturePtr entryPtsText : m_entryPointsTextures)
    {
        ::Ogre::RenderTexture* renderTexture = entryPtsText->getBuffer()->getRenderTarget();
        renderTexture->addViewport(m_camera);
    }

    initEntryPoints();

    updateMatNames();

    setSampling(m_nbSlices);
}

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::~RayTracingVolumeRenderer()
{
    if(m_autostereoListener)
    {
        ::Ogre::MaterialManager::getSingleton().removeListener(m_autostereoListener);
        delete m_autostereoListener;
    }

    m_camera->removeListener(m_cameraListener);

    if(m_r2vbSource)
    {
        auto mesh = m_r2vbSource->getMesh();
        m_sceneManager->destroyEntity(m_r2vbSource);
        m_r2vbSource = nullptr;
        ::Ogre::MeshManager::getSingleton().remove(mesh->getHandle());
    }
    m_volumeSceneNode->detachObject(m_entryPointGeometry);
    m_volumeSceneNode->detachObject(m_proxyGeometryGenerator);
    m_sceneManager->destroyManualObject(m_entryPointGeometry);
    m_sceneManager->destroyMovableObject(m_proxyGeometryGenerator);

    for(auto& texture : m_entryPointsTextures)
    {
        ::Ogre::TextureManager::getSingleton().remove(texture->getHandle());
    }

    if(!m_gridTexture.isNull())
    {
        ::Ogre::TextureManager::getSingleton().remove(m_gridTexture->getHandle());
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf)
{
    this->scaleCube(image->getSpacing());

    const ::fwData::Image::SizeType& newSize = image->getSize();

    // Create new grid texture + proxy geometry if image size changed.
    if(m_imageSize != newSize)
    {
        m_imageSize = newSize;

        for(size_t i = 0; i < 3; ++i)
        {
            m_gridSize[i] =
                static_cast<int>(m_imageSize[i]) / m_bricksSize[i] +
                (static_cast<int>(m_imageSize[i]) % m_bricksSize[i] != 0);
        }

        if(!m_gridTexture.isNull())
        {
            ::Ogre::TextureManager::getSingleton().remove(m_gridTexture->getHandle());
            m_gridTexture.setNull();
        }

        this->createGridTexture();

        tfUpdate(tf);

        m_proxyGeometryGenerator->manualUpdate();
    }

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.imageUpdate(image, tf, m_sampleDistance);

        auto minMax = m_preIntegrationTable.getMinMax();

        this->setFpNamedConstant("u_min", minMax.first);
        this->setFpNamedConstant("u_max", minMax.second);
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
            m_gridRenderOp.operationType           = ::Ogre::RenderOperation::OT_TRIANGLE_STRIP;

            for(unsigned i = 0; i < static_cast<unsigned>(m_gridSize[2]); ++i)
            {
                ::Ogre::RenderTexture* rt = m_gridTexture->getBuffer()->getRenderTarget(i);

                rt->getViewport(0)->clear();

                ::Ogre::MaterialPtr gridMtl = ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricksGrid");

                ::Ogre::Pass* gridPass                       = gridMtl->getTechnique(0)->getPass(0);
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
            m_gridRenderOp.operationType           = ::Ogre::RenderOperation::OT_POINT_LIST;
        }

        m_proxyGeometryGenerator->manualUpdate();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setSampling(uint16_t nbSamples)
{
    m_nbSlices = nbSamples;

    computeSampleDistance(getCameraPlane());

    this->setFpNamedConstant("u_sampleDistance", m_sampleDistance);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setAOFactor(double aoFactor)
{
    m_aoFactor = aoFactor;

    ::Ogre::Real cbFactor = static_cast< ::Ogre::Real>(m_colorBleedingFactor);
    ::Ogre::Vector4 volIllumFactor(cbFactor, cbFactor, cbFactor, static_cast< ::Ogre::Real>(m_aoFactor));

    this->setFpNamedConstant("u_volIllumFactor", volIllumFactor);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setColorBleedingFactor(double colorBleedingFactor)
{
    m_colorBleedingFactor = colorBleedingFactor;

    ::Ogre::Real cbFactor = static_cast< ::Ogre::Real>(m_colorBleedingFactor);
    ::Ogre::Vector4 volIllumFactor(cbFactor, cbFactor, cbFactor, static_cast< ::Ogre::Real>(m_aoFactor));

    this->setFpNamedConstant("u_volIllumFactor", volIllumFactor);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIlluminationVolume(SATVolumeIllumination* illuminationVolume)
{
    m_illumVolume = illuminationVolume;

    std::for_each(m_rayTracedTexUnitStates.begin(), m_rayTracedTexUnitStates.end(),
                  [illuminationVolume](::Ogre::TextureUnitState* tus)
            {
                tus->setTexture(illuminationVolume->getIlluminationVolume());
            });
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setPreIntegratedRendering(bool preIntegratedRendering)
{
    OSLM_WARN_IF("Stereoscopic rendering doesn't implement pre-integration yet.",
                 m_stereoMode != ::fwRenderOgre::Layer::StereoModeType::NONE && preIntegratedRendering);

    m_preIntegratedRendering = preIntegratedRendering;

    this->updateMatNames();
    m_entryPointGeometry->setMaterialName(0, m_currentMtlName);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setAmbientOcclusion(bool ambientOcclusion)
{
    m_ambientOcclusion = ambientOcclusion;

    this->updateMatNames();
    m_entryPointGeometry->setMaterialName(0, m_currentMtlName);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setColorBleeding(bool colorBleeding)
{
    m_colorBleeding = colorBleeding;

    this->updateMatNames();
    m_entryPointGeometry->setMaterialName(0, m_currentMtlName);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setShadows(bool shadows)
{
    m_shadows = shadows;

    this->updateMatNames();
    m_entryPointGeometry->setMaterialName(0, m_currentMtlName);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::clipImage(const ::Ogre::AxisAlignedBox& clippingBox)
{
    IVolumeRenderer::clipImage(clippingBox);

    const ::Ogre::AxisAlignedBox maxBoxSize(::Ogre::Vector3::ZERO, ::Ogre::Vector3(1.f, 1.f, 1.f));
    const ::Ogre::AxisAlignedBox realClippingBox = maxBoxSize.intersection(clippingBox);

    ::Ogre::MaterialPtr geomMtl =
        ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricks");
    ::Ogre::GpuProgramParametersSharedPtr geomParams =
        geomMtl->getTechnique(0)->getPass(0)->getGeometryProgramParameters();

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
    const auto numViewPoints = m_entryPointsTextures.size();
    const float wRatio       = numViewPoints != 1 ? 3.f / numViewPoints : 1.f;
    const float hRatio       = numViewPoints != 1 ? 0.5f : 1.f;

    for(::Ogre::TexturePtr entryPtsTexture : m_entryPointsTextures)
    {
        entryPtsTexture->freeInternalResources();

        entryPtsTexture->setWidth(static_cast< ::Ogre::uint32>(w * wRatio));
        entryPtsTexture->setHeight(static_cast< ::Ogre::uint32>(h * hRatio));

        entryPtsTexture->createInternalResources();

        ::Ogre::RenderTexture* renderTexture = entryPtsTexture->getBuffer()->getRenderTarget();
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

    m_entryPointGeometry->setVisible(true);

    m_volumeSceneNode->attachObject(m_entryPointGeometry);

    // Create R2VB object used to generate proxy geometry
    {
        ::Ogre::MeshPtr gridMesh = ::Ogre::MeshManager::getSingleton().createManual(
            m_parentId + "_gridMesh",
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
            );

        ::Ogre::SubMesh* subMesh = gridMesh->createSubMesh();

        const int nbVtx = m_gridSize[0] * m_gridSize[1] * m_gridSize[2];

        subMesh->useSharedVertices = false;
        subMesh->operationType     = ::Ogre::RenderOperation::OT_POINT_LIST;

        subMesh->vertexData              = new ::Ogre::VertexData();
        subMesh->vertexData->vertexStart = 0;
        subMesh->vertexData->vertexCount = static_cast<size_t>(nbVtx);

        ::Ogre::VertexDeclaration* decl = subMesh->vertexData->vertexDeclaration;

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

        m_cameraListener = new CameraListener(this);
        m_camera->addListener(m_cameraListener);

        m_r2vbSource->getSubEntity(0)->getRenderOperation(m_gridRenderOp);

        m_volumeSceneNode->attachObject(m_proxyGeometryGenerator);
    }

    // Create grid texture and set parameters in it's associated shader.
    {
        ::Ogre::MaterialPtr gridMtl = ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricksGrid");

        ::Ogre::Pass* gridPass = gridMtl->getTechnique(0)->getPass(0);

        ::Ogre::TextureUnitState* tex3DState = gridPass->getTextureUnitState("image");
        ::Ogre::TextureUnitState* texTFState = gridPass->getTextureUnitState("transferFunction");

        SLM_ASSERT("'image' texture unit is not found", tex3DState);
        SLM_ASSERT("'transferFunction' texture unit is not found", texTFState);

        tex3DState->setTexture(m_3DOgreTexture);
        texTFState->setTexture(m_gpuTF.getTexture());

        createGridTexture();
    }

    // Render geometry.
    m_proxyGeometryGenerator->manualUpdate();
}

//-----------------------------------------------------------------------------.

void RayTracingVolumeRenderer::computeEntryPointsTexture()
{
    m_proxyGeometryGenerator->setVisible(false);

    ::Ogre::Pass* pass = m_proxyGeometryGenerator->getMaterial()->getTechnique(0)->getPass(0);

    ::Ogre::RenderOperation renderOp;
    m_proxyGeometryGenerator->getRenderOperation(renderOp);
    m_entryPointGeometry->setVisible(true);

    ::Ogre::Matrix4 worldMat;
    m_proxyGeometryGenerator->getWorldTransforms(&worldMat);

    float eyeAngle = 0.f;
    float angle    = 0.f;
    if(m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5)
    {
        eyeAngle = 0.02321f;
        angle    = eyeAngle * -2.f;
    }
    else if(m_stereoMode == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8)
    {
        eyeAngle = 0.01625f;
        angle    = eyeAngle * -3.5f;
    }

    for(::Ogre::TexturePtr entryPtsText : m_entryPointsTextures)
    {
        ::Ogre::Matrix4 projMat = m_camera->getProjectionMatrix();

        ::Ogre::RenderTexture* renderTexture = entryPtsText->getBuffer()->getRenderTarget();
        renderTexture->getViewport(0)->clear(::Ogre::FBT_COLOUR | ::Ogre::FBT_DEPTH, ::Ogre::ColourValue::White);

        if(m_stereoMode != ::fwRenderOgre::Layer::StereoModeType::NONE)
        {
            const ::Ogre::Matrix4 shearTransform = this->frustumShearTransform(angle);

            angle  += eyeAngle;
            projMat = projMat * shearTransform;
        }

        m_sceneManager->manualRender(&renderOp, pass, renderTexture->getViewport(0), worldMat,
                                     m_camera->getViewMatrix(), projMat);
    }
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

void RayTracingVolumeRenderer::updateMatNames()
{
    bool ao = false;
    m_currentMtlName = "RayTracedVolume";

    if(m_preIntegratedRendering)
    {
        m_currentMtlName += "_PreIntegrated";
    }

    if(m_ambientOcclusion)
    {
        ao = true;
        if(m_colorBleeding)
        {
            if (m_shadows)
            {
                m_currentMtlName += "_VolumeIllumination";
            }
            else
            {
                m_currentMtlName += "_AmbientOcclusion_ColorBleeding";
            }
        }
        else
        {
            if(m_shadows)
            {
                m_currentMtlName += "_AmbientOcclusion_Shadows";
            }
            else
            {
                m_currentMtlName += "_AmbientOcclusion";
            }
        }
    }
    else
    {
        if(m_colorBleeding)
        {
            ao = true;
            if(m_shadows)
            {
                m_currentMtlName += "_ColorBleeding_Shadows";
            }
            else
            {
                m_currentMtlName += "_ColorBleeding";
            }
        }
        else
        {
            if(m_shadows)
            {
                ao                = true;
                m_currentMtlName += "_Shadows";
            }
        }
    }

    std::string volIllumMtl = "VolIllum";

    volIllumMtl += m_ambientOcclusion || m_colorBleeding ? "_AO" : "";
    volIllumMtl += m_shadows ? "_Shadows" : "";

    SLM_ASSERT("Camera listener not instantiated", m_cameraListener);
    m_cameraListener->setCurrentMtlName(volIllumMtl);

    // Set colors
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName);
    ::Ogre::ColourValue diffuse(1.2f, 1.2f, 1.2f, 1.f);
    material->setDiffuse(diffuse);

    ::Ogre::ColourValue specular(2.5f, 2.5f, 2.5f, 1.f);
    material->setSpecular( specular );
    material->setShininess( 10 );

    // Set all parameters
    if(ao)
    {
        this->setAOFactor(m_aoFactor);
        this->setColorBleedingFactor(m_colorBleedingFactor);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::createGridTexture()
{
    // Create grid texture and set initialize render targets.
    {
        m_gridTexture = ::Ogre::TextureManager::getSingleton().createManual(
            m_parentId + "_gridTexture",
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            ::Ogre::TEX_TYPE_3D,
            static_cast<unsigned int>(m_gridSize[0]),
            static_cast<unsigned int>(m_gridSize[1]),
            static_cast<unsigned int>(m_gridSize[2]),
            1,
            ::Ogre::PF_R8,
            ::Ogre::TU_RENDERTARGET
            );

        for(unsigned i = 0; i < static_cast<unsigned>(m_gridSize[2]); ++i)
        {
            ::Ogre::RenderTexture* rt = m_gridTexture->getBuffer()->getRenderTarget(i);
            rt->addViewport(m_camera);
        }
    }

    // Update R2VB source geometry.
    {
        ::Ogre::MeshPtr r2vbSrcMesh = ::Ogre::MeshManager::getSingleton().getByName(m_parentId + "_gridMesh");

        ::Ogre::VertexData* meshVtxData = r2vbSrcMesh->getSubMesh(0)->vertexData;

        meshVtxData->vertexCount = static_cast<size_t>(m_gridSize[0] * m_gridSize[1] * m_gridSize[2]);

        ::Ogre::HardwareVertexBufferSharedPtr vtxBuffer =
            ::Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
                ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                meshVtxData->vertexCount,
                ::Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        for(size_t i = 0; i < meshVtxData->vertexCount; ++i)
        {
            vtxBuffer->writeData(
                i * ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                ::Ogre::VertexElement::getTypeSize(::Ogre::VET_INT1),
                &i,
                false);
        }

        meshVtxData->vertexBufferBinding->setBinding(0, vtxBuffer);

        r2vbSrcMesh->load();

        m_r2vbSource->getSubEntity(0)->getRenderOperation(m_gridRenderOp);

        m_proxyGeometryGenerator->setOutputSettings(meshVtxData->vertexCount * 36, false, false, "VolumeBricks");
    }

    // Set shader parameters.
    {
        ::Ogre::MaterialPtr gridMtl = ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricksGrid");

        ::Ogre::Pass* gridPass = gridMtl->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr gridGeneratorParams = gridPass->getFragmentProgramParameters();

        gridGeneratorParams->setNamedConstant("u_gridResolution", m_gridSize.data(), 3, 1);
        gridGeneratorParams->setNamedConstant("u_brickSize", m_bricksSize.data(), 3, 1);

        ::Ogre::MaterialPtr geomGeneratorMtl = ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricks");

        ::Ogre::Pass* geomGenerationPass = geomGeneratorMtl->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorVtxParams = geomGenerationPass->getVertexProgramParameters();

        geomGeneratorVtxParams->setNamedConstant("u_gridResolution", m_gridSize.data(), 3, 1);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorGeomParams =
            geomGenerationPass->getGeometryProgramParameters();

        // Cast size_t to int.
        const std::vector<int> imageSize(m_imageSize.begin(), m_imageSize.end());

        geomGeneratorGeomParams->setNamedConstant("u_imageResolution", imageSize.data(), 3, 1);
        geomGeneratorGeomParams->setNamedConstant("u_gridResolution", m_gridSize.data(), 3, 1);
        geomGeneratorGeomParams->setNamedConstant("u_brickSize", m_bricksSize.data(), 3, 1);

        ::Ogre::TextureUnitState* gridTexState = geomGenerationPass->getTextureUnitState("gridVolume");

        SLM_ASSERT("'grid' texture unit is not found", gridTexState);

        gridTexState->setTexture(m_gridTexture);
    }

}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre
