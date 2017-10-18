/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/vr/RayTracingVolumeRenderer.hpp"

#include "fwRenderOgre/helper/Camera.hpp"
#include "fwRenderOgre/helper/Shading.hpp"
#include "fwRenderOgre/SRender.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <fwCore/Profiling.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <OGRE/OgreException.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <algorithm>
#include <map>
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
                auto illuVolume = m_renderer->m_illumVolume.lock();
                if(illuVolume && m_renderer->m_shadows)
                {
                    // Set light directions in shader.
                    ::Ogre::LightList closestLights =
                        m_renderer->m_volumeSceneNode->getAttachedObject(0)->queryLights();

                    if(!closestLights.empty())
                    {
                        ::Ogre::Vector3 lightDir = m_renderer->m_volumeSceneNode->convertLocalToWorldDirection(
                            closestLights[0]->getDerivedDirection(), true);

                        ::Ogre::Pass* satIllumPass = ::Ogre::MaterialManager::getSingleton().getByName(
                            m_currentMtlName)->getTechnique(0)->getPass(0);
                        ::Ogre::GpuProgramParametersSharedPtr satIllumParams =
                            satIllumPass->getFragmentProgramParameters();

                        satIllumParams->setNamedConstant("u_lightDir", lightDir);

                        illuVolume->updateVolIllum();
                    }
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

//--------------------------------------------------------------,---------------

const std::string s_AUTOSTEREO_DEFINE = "AUTOSTEREO=1";

const std::string s_AO_DEFINE             = "AMBIENT_OCCLUSION=1";
const std::string s_COLOR_BLEEDING_DEFINE = "COLOR_BLEEDING=1";
const std::string s_SHADOWS_DEFINE        = "SHADOWS=1";
const std::string s_PREINTEGRATION_DEFINE = "PREINTEGRATION=1";

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::RayTracingVolumeRenderer(std::string parentId,
                                                   Layer::sptr layer,
                                                   ::Ogre::SceneNode* parentNode,
                                                   ::Ogre::TexturePtr imageTexture,
                                                   TransferFunction& gpuTF,
                                                   PreIntegrationTable& preintegrationTable,
                                                   ::fwRenderOgre::Layer::StereoModeType stereoMode,
                                                   bool ambientOcclusion,
                                                   bool colorBleeding,
                                                   bool shadows,
                                                   double aoFactor,
                                                   double colorBleedingFactor) :
    IVolumeRenderer(parentId, layer->getSceneManager(), parentNode, imageTexture, gpuTF, preintegrationTable),
    m_entryPointGeometry(nullptr),
    m_proxyGeometry(nullptr),
    m_imageSize(::fwData::Image::SizeType({ 1, 1, 1 })),
    m_stereoMode(stereoMode),
    m_ambientOcclusion(ambientOcclusion),
    m_colorBleeding(colorBleeding),
    m_shadows(shadows),
    m_volIllumFactor(static_cast< ::Ogre::Real>(colorBleedingFactor),
                     static_cast< ::Ogre::Real>(colorBleedingFactor),
                     static_cast< ::Ogre::Real>(colorBleedingFactor),
                     static_cast< ::Ogre::Real>(aoFactor)),
    m_opacityCorrectionFactor(200.f),
    m_focalLength(0.f),
    m_cameraListener(nullptr),
    m_layer(layer),
    m_autostereoListener(nullptr),
    m_fullScreenQuad(new ::Ogre::Rectangle2D())
{
    m_fullScreenQuad->setCorners(-1, 1, 1, -1);

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
                                            ::Ogre::PF_FLOAT32_GR,
                                            ::Ogre::TU_RENDERTARGET ));
    }

    if(m_stereoMode != ::fwRenderOgre::Layer::StereoModeType::NONE)
    {
        m_autostereoListener = new compositor::listener::AutoStereoCompositorListener(&m_entryPointsTextures);
        ::Ogre::MaterialManager::getSingleton().addListener(m_autostereoListener);
    }

    // First check that we did not already instanced Shared parameters
    // This can happen when reinstancing this class (e.g. switching 3D mode)
    ::Ogre::GpuProgramManager::SharedParametersMap spMap =
        ::Ogre::GpuProgramManager::getSingleton().getAvailableSharedParameters();
    if(spMap["RTVParams"].isNull())
    {
        m_RTVSharedParameters = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters("RTVParams");

        // define the shared param structure
        m_RTVSharedParameters->addConstantDefinition("u_sampleDistance", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_lobeOffset", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_opacityCorrectionFactor", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_volIllumFactor", ::Ogre::GCT_FLOAT4);
        m_RTVSharedParameters->addConstantDefinition("u_min", ::Ogre::GCT_INT1);
        m_RTVSharedParameters->addConstantDefinition("u_max", ::Ogre::GCT_INT1);
        m_RTVSharedParameters->setNamedConstant("u_opacityCorrectionFactor", m_opacityCorrectionFactor);
    }
    else
    {
        m_RTVSharedParameters = spMap["RTVParams"];
    }

    for(::Ogre::TexturePtr entryPtsText : m_entryPointsTextures)
    {
        ::Ogre::RenderTexture* renderTexture = entryPtsText->getBuffer()->getRenderTarget();
        renderTexture->addViewport(m_camera);
    }

    m_fragmentShaderAttachements.push_back("TransferFunction_FP");
    this->createRayTracingMaterial();

    this->initEntryPoints();

    this->setSampling(m_nbSlices);
}

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::~RayTracingVolumeRenderer()
{
    delete m_fullScreenQuad;
    m_camera->removeListener(m_cameraListener);

    if(m_autostereoListener)
    {
        ::Ogre::MaterialManager::getSingleton().removeListener(m_autostereoListener);
        delete m_autostereoListener;
        m_autostereoListener = nullptr;
    }

    m_camera->removeListener(m_cameraListener);
    delete m_cameraListener;
    m_cameraListener = nullptr;

    m_volumeSceneNode->detachObject(m_entryPointGeometry);
    m_volumeSceneNode->detachObject(m_proxyGeometry);

    m_sceneManager->destroyManualObject(m_entryPointGeometry);
    m_sceneManager->destroyMovableObject(m_proxyGeometry);

    for(auto& texture : m_entryPointsTextures)
    {
        ::Ogre::TextureManager::getSingleton().remove(texture->getHandle());
    }
    m_entryPointsTextures.clear();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf)
{
    if(!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        return;
    }

    this->scaleCube(image->getSpacing());

    const ::fwData::Image::SizeType& newSize = image->getSize();

    // Create new grid texture + proxy geometry if image size changed.
    if(m_imageSize != newSize)
    {
        m_imageSize = newSize;

        m_proxyGeometry->updateGridSize();
    }
    else
    {
        m_proxyGeometry->computeGrid();
    }

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.imageUpdate(image, tf, m_sampleDistance);

        // After having updated the preintegrated transfer function
        // We update the corresponding shader parameters
        auto minMax = m_preIntegrationTable.getMinMax();

        m_RTVSharedParameters->setNamedConstant("u_min", minMax.first);
        m_RTVSharedParameters->setNamedConstant("u_max", minMax.second);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::tfUpdate(fwData::TransferFunction::sptr /*tf*/)
{
    FW_PROFILE("TF Update")
    {
        m_proxyGeometry->computeGrid();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setSampling(uint16_t nbSamples)
{
    m_nbSlices = nbSamples;

    computeSampleDistance(getCameraPlane());

    // Update the sample distance in the shaders
    m_RTVSharedParameters->setNamedConstant("u_sampleDistance", m_sampleDistance);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setOpacityCorrection(int opacityCorrection)
{
    m_opacityCorrectionFactor = static_cast<float>(opacityCorrection);

    // Update shader parameter
    m_RTVSharedParameters->setNamedConstant("u_opacityCorrectionFactor", m_opacityCorrectionFactor);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setAOFactor(double aoFactor)
{
    m_volIllumFactor.w = static_cast< ::Ogre::Real>(aoFactor);

    // Update the shader parameter
    m_RTVSharedParameters->setNamedConstant("u_volIllumFactor", m_volIllumFactor);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setColorBleedingFactor(double colorBleedingFactor)
{
    ::Ogre::Real cbFactor = static_cast< ::Ogre::Real>(colorBleedingFactor);
    m_volIllumFactor      = ::Ogre::Vector4(cbFactor, cbFactor, cbFactor, m_volIllumFactor.w);

    // Update the shader parameter
    m_RTVSharedParameters->setNamedConstant("u_volIllumFactor", m_volIllumFactor);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIlluminationVolume(SATVolumeIllumination::sptr illuminationVolume)
{
    m_illumVolume = illuminationVolume;

    this->createRayTracingMaterial();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setPreIntegratedRendering(bool preIntegratedRendering)
{
    OSLM_WARN_IF("Stereoscopic rendering doesn't implement pre-integration yet.",
                 m_stereoMode != ::fwRenderOgre::Layer::StereoModeType::NONE && preIntegratedRendering);

    m_preIntegratedRendering = preIntegratedRendering;

    this->createRayTracingMaterial();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setAmbientOcclusion(bool ambientOcclusion)
{
    m_ambientOcclusion = ambientOcclusion;

    this->createRayTracingMaterial();
    this->updateVolIllumMat();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setColorBleeding(bool colorBleeding)
{
    m_colorBleeding = colorBleeding;

    this->createRayTracingMaterial();
    this->updateVolIllumMat();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setShadows(bool shadows)
{
    m_shadows = shadows;

    this->createRayTracingMaterial();
    this->updateVolIllumMat();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setFocalLength(float focalLength)
{
    m_focalLength = focalLength;
    computeRealFocalLength();
}

//------------------------------------------------------------------------------

void RayTracingVolumeRenderer::clipImage(const ::Ogre::AxisAlignedBox& clippingBox)
{
    IVolumeRenderer::clipImage(clippingBox);

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

    m_proxyGeometry->clipGrid(clippingBox);
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

void RayTracingVolumeRenderer::setRayCastingPassTextureUnits(Ogre::Pass* _rayCastingPass,
                                                             const std::string& _fpPPDefines) const
{
    ::Ogre::GpuProgramParametersSharedPtr fpParams = _rayCastingPass->getFragmentProgramParameters();

    int numTexUnit = 0;
    ::Ogre::TextureUnitState* texUnitState;
    // Volume data
    texUnitState = _rayCastingPass->createTextureUnitState();
    texUnitState->setTextureName(m_3DOgreTexture->getName(), ::Ogre::TEX_TYPE_3D);
    texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
    texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

    fpParams->setNamedConstant("u_image", numTexUnit++);

    // Transfer function
    if(_fpPPDefines.find(s_PREINTEGRATION_DEFINE) != std::string::npos)
    {
        texUnitState = _rayCastingPass->createTextureUnitState(m_preIntegrationTable.getTexture()->getName());
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
    }
    else
    {
        texUnitState = _rayCastingPass->createTextureUnitState(m_gpuTF.getTexture()->getName());
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
    }

    fpParams->setNamedConstant("u_tfTexture", numTexUnit++);

    if(_fpPPDefines.find(s_AO_DEFINE) != std::string::npos)
    {
        texUnitState =
            _rayCastingPass->createTextureUnitState(m_illumVolume.lock()->getIlluminationVolume()->getName());
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

        fpParams->setNamedConstant("u_illuminationVolume", numTexUnit++);
        // Update the shader parameter
        m_RTVSharedParameters->setNamedConstant("u_volIllumFactor", m_volIllumFactor);
    }

    // Entry points texture
    texUnitState = _rayCastingPass->createTextureUnitState();
    texUnitState->setName("entryPoints");
    texUnitState->setTexture(m_entryPointsTextures[0]);
    texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
    texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

    fpParams->setNamedConstant("u_entryPoints", numTexUnit++);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::createRayTracingMaterial()
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;
    std::tie(vpPPDefines, fpPPDefines, hash) = this->computeRayTracingDefines();

    ::Ogre::String matName("RTV_Mat_" + m_parentId);
    m_currentMtlName = matName;

    ::Ogre::MaterialManager& mm = ::Ogre::MaterialManager::getSingleton();

    // The material needs to be destroyed only if it already exists
    {
        ::Ogre::ResourcePtr matResource =
            mm.getResourceByName(matName, ::Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
        if(!matResource.isNull())
        {
            mm.remove(matResource);
            // Our manual object still references the material and uses the material name to know if it should modify
            // its pointer (see ManualObject::ManualObjectSection::setMaterialName() in OgreManualObject.cpp)
            // So we just force it to release the pointer otherwise the material is not destroyed
            if(m_entryPointGeometry)
            {
                m_entryPointGeometry->setMaterialName(0, "");
            }
        }
    }

    ::Ogre::HighLevelGpuProgramManager& gpm = ::Ogre::HighLevelGpuProgramManager::getSingleton();

    ///////////////////////////////////////////////////////////////////////////
    /// Compile vertex shader
    ::Ogre::String vpName("RTV_VP_" + std::to_string(hash));
    ::Ogre::HighLevelGpuProgramPtr vsp;

    if(gpm.resourceExists(vpName))
    {
        vsp = gpm.getByName(vpName, "Materials");
    }
    else
    {
        vsp = gpm.createProgram(vpName, "Materials", "glsl", ::Ogre::GPT_VERTEX_PROGRAM);
        vsp->setSourceFile("RayTracedVolume_VP.glsl");

        if(vpPPDefines.size() > 0)
        {
            vsp->setParameter("preprocessor_defines", vpPPDefines);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Compile fragment shader
    ::Ogre::String fpName("RTV_FP_" + std::to_string(hash));
    ::Ogre::HighLevelGpuProgramPtr fsp;

    if(gpm.resourceExists(fpName))
    {
        fsp = gpm.getByName(fpName, "Materials");
    }
    else
    {
        fsp = gpm.createProgram(fpName, "Materials", "glsl", ::Ogre::GPT_FRAGMENT_PROGRAM);
        fsp->setSourceFile("RayTracedVolume_FP.glsl");

        for(const std::string& attachement: m_fragmentShaderAttachements)
        {
            fsp->setParameter("attach", attachement);
        }

        if(fpPPDefines.size() > 0)
        {
            fsp->setParameter("preprocessor_defines", fpPPDefines);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Create the material
    ::Ogre::MaterialPtr mat = mm.create(matName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    // Ensure that we have the color parameters set for the current material
    this->setMaterialLightParams(mat);
    // Get the already created pass through the already created technique
    ::Ogre::Technique* tech = mat->getTechnique(0);

    ::Ogre::Pass* pass = tech->getPass(0);
    pass->setCullingMode(::Ogre::CULL_ANTICLOCKWISE);
    pass->setSceneBlending(::Ogre::SBT_TRANSPARENT_ALPHA);

    // Vertex program
    pass->setVertexProgram(vpName);
    ::Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
    vpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

    // Fragment program
    pass->setFragmentProgram(fpName);
    ::Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
    fpParams->setNamedAutoConstant("u_viewportWidth", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_WIDTH);
    fpParams->setNamedAutoConstant("u_viewportHeight", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_HEIGHT);
    fpParams->setNamedAutoConstant("u_clippingNear", ::Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
    fpParams->setNamedAutoConstant("u_clippingFar", ::Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
    fpParams->setNamedAutoConstant("u_cameraPos", ::Ogre::GpuProgramParameters::ACT_LOD_CAMERA_POSITION);
    fpParams->setNamedAutoConstant("u_shininess", ::Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
    fpParams->setNamedAutoConstant("u_invWorldViewProj",
                                   ::Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEWPROJ_MATRIX);
    fpParams->setNamedAutoConstant("u_numLights", ::Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
    for(size_t i = 0; i < 10; ++i)
    {
        auto number = "[" + std::to_string(i) + "]";
        fpParams->setNamedAutoConstant("u_lightDir" + number, ::Ogre::GpuProgramParameters::ACT_LIGHT_POSITION, i);
        fpParams->setNamedAutoConstant("u_lightDiffuse" + number,
                                       ::Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR, i);
        fpParams->setNamedAutoConstant("u_lightSpecular" + number,
                                       ::Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR, i);
    }
    fpParams->addSharedParameters("RTVParams");

    ///////////////////////////////////////////////////////////////////////////
    /// Setup texture unit states
    this->setRayCastingPassTextureUnits(pass, fpPPDefines);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::initEntryPoints()
{
    m_entryPointGeometry = m_sceneManager->createManualObject(m_parentId + "_RayTracingVREntryPoints");

    m_entryPointGeometry->begin(m_currentMtlName, ::Ogre::RenderOperation::OT_TRIANGLE_LIST);
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

    m_proxyGeometry = ::fwRenderOgre::vr::GridProxyGeometry::New(this->m_parentId + "_GridProxyGeometry",
                                                                 m_sceneManager, m_3DOgreTexture,
                                                                 m_gpuTF, "RayEntryPoints");
    m_volumeSceneNode->attachObject(m_proxyGeometry);

    m_cameraListener = new CameraListener(this);
    m_camera->addListener(m_cameraListener);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::computeEntryPointsTexture()
{
    m_proxyGeometry->setVisible(false);

    ::Ogre::RenderOperation renderOp;
    m_proxyGeometry->getRenderOperation(renderOp);
    m_entryPointGeometry->setVisible(true);
    m_entryPointGeometry->setMaterialName(0, m_currentMtlName);

    ::Ogre::Matrix4 worldMat;
    m_proxyGeometry->getWorldTransforms(&worldMat);

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

    ::Ogre::RenderOperation fsRenderOp;
    m_fullScreenQuad->getRenderOperation(fsRenderOp);

    for(::Ogre::TexturePtr entryPtsText : m_entryPointsTextures)
    {
        ::Ogre::Matrix4 projMat = m_camera->getProjectionMatrix();

        // Move to the next view point if we're in 3D mode
        if(m_stereoMode != ::fwRenderOgre::Layer::StereoModeType::NONE)
        {
            const auto shearTransform = ::fwRenderOgre::helper::Camera::computeFrustumShearTransform(*m_camera, angle);

            angle  += eyeAngle;
            projMat = projMat * shearTransform;
        }

        ::Ogre::Viewport* entryPtsVp = entryPtsText->getBuffer()->getRenderTarget()->getViewport(0);
        entryPtsVp->clear(::Ogre::FBT_COLOUR | ::Ogre::FBT_DEPTH | ::Ogre::FBT_STENCIL, ::Ogre::ColourValue::White);

        // FIRST STEP - Back Faces max: find the exit points of the geometry.
        ::Ogre::RenderSystem* rs = ::Ogre::Root::getSingleton().getRenderSystem();
        rs->setStencilCheckEnabled(true);

        // Set the stencil buffer to 1 where back faces are rasterized.
        rs->setStencilBufferParams(::Ogre::CMPF_ALWAYS_PASS, 0x1, 0xFFFFFFFF, 0xFFFFFFFF, ::Ogre::SOP_KEEP,
                                   ::Ogre::SOP_KEEP,
                                   ::Ogre::SOP_REPLACE);

        ::Ogre::Pass* pass = m_proxyGeometry->getMaterial()->getTechnique(0)->getPass("BackFacesMax");
        m_sceneManager->manualRender(&renderOp, pass, entryPtsVp, worldMat, m_camera->getViewMatrix(), projMat);

        // SECOND STEP - Back Faces: find the closest back faces and initialize the entry points with their depths.
        rs->setStencilCheckEnabled(false);
        pass = m_proxyGeometry->getMaterial()->getTechnique(0)->getPass("BackFaces");
        m_sceneManager->manualRender(&renderOp, pass, entryPtsVp, worldMat, m_camera->getViewMatrix(), projMat);

        // THIRD STEP - Front Faces: update the entry points depths with the closest front faces occluding closest back
        // faces.
        rs->setStencilCheckEnabled(true);

        // Set the stencil buffer to 2 where front faces are rasterized.
        rs->setStencilBufferParams(::Ogre::CMPF_ALWAYS_PASS, 0x2, 0xFFFFFFFF, 0xFFFFFFFF, ::Ogre::SOP_KEEP,
                                   ::Ogre::SOP_KEEP,
                                   ::Ogre::SOP_REPLACE);

        pass = m_proxyGeometry->getMaterial()->getTechnique(0)->getPass("FrontFaces");
        m_sceneManager->manualRender(&renderOp, pass, entryPtsVp, worldMat, m_camera->getViewMatrix(), projMat);

        // FOURTH STEP - Near Plane: update the entry points depths with the near plane depth when when it is inside a
        // brick.
        // The stencil test passes if the current stencil buffer value is equal to 1.
        // That is, where no front faces have been rasterized.
        rs->setStencilBufferParams(::Ogre::CMPF_EQUAL, 0x1, 0xFFFFFFFF);

        ::Ogre::Matrix4 identity;
        pass = m_proxyGeometry->getMaterial()->getTechnique(0)->getPass("NearPlane");

        m_sceneManager->manualRender(&fsRenderOp, pass, entryPtsVp, identity, m_camera->getViewMatrix(), projMat);

        rs->setStencilCheckEnabled(false);
        rs->setStencilBufferParams();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::computeRealFocalLength()
{
    const ::Ogre::Plane cameraPlane(m_camera->getRealDirection(), m_camera->getRealPosition());
    const auto cameraDistComparator = [&cameraPlane](const ::Ogre::Vector3& v1, const ::Ogre::Vector3& v2)
                                      { return cameraPlane.getDistance(v1) < cameraPlane.getDistance(v2); };

    const auto closestFurthestImgPoints
        = std::minmax_element(m_clippedImagePositions, m_clippedImagePositions + 8, cameraDistComparator);

    const auto focusPoint = *closestFurthestImgPoints.first + m_focalLength *
                            (*closestFurthestImgPoints.second - *closestFurthestImgPoints.first);

    const float realFocalLength = m_camera->getRealPosition().distance(focusPoint);

    m_camera->setFocalLength(realFocalLength);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateVolIllumMat()
{
    std::string volIllumMtl = "VolIllum";

    volIllumMtl += m_ambientOcclusion || m_colorBleeding ? "_AO" : "";
    volIllumMtl += m_shadows ? "_Shadows" : "";

    SLM_ASSERT("Camera listener not instantiated", m_cameraListener);
    m_cameraListener->setCurrentMtlName(volIllumMtl);
}

//-----------------------------------------------------------------------------

std::tuple<std::string, std::string, size_t> RayTracingVolumeRenderer::computeRayTracingDefines() const
{
    std::ostringstream vpPPDefs, fpPPDefs;

    vpPPDefs.str("");
    fpPPDefs.str("");

    if(m_ambientOcclusion)
    {
        fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_AO_DEFINE;
    }

    if(m_colorBleeding)
    {
        fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_COLOR_BLEEDING_DEFINE;
    }

    if(m_shadows)
    {
        fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_SHADOWS_DEFINE;
    }

    if(m_preIntegratedRendering)
    {
        fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_PREINTEGRATION_DEFINE;
    }

    return std::make_tuple(vpPPDefs.str(), fpPPDefs.str(), std::hash<std::string>{} (vpPPDefs.str() + fpPPDefs.str()));
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setMaterialLightParams(::Ogre::MaterialPtr mtl)
{
    ::Ogre::ColourValue diffuse(1.2f, 1.2f, 1.2f, 1.f);
    mtl->setDiffuse(diffuse);

    ::Ogre::ColourValue specular(2.5f, 2.5f, 2.5f, 1.f);
    mtl->setSpecular( specular );
    mtl->setShininess( 10 );
}

//-----------------------------------------------------------------------------

} // namespace vr
} // namespace fwRenderOgre
