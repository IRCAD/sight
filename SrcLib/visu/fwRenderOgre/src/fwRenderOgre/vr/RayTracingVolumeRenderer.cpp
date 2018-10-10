/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/vr/RayTracingVolumeRenderer.hpp"

#include "fwRenderOgre/compositor/Core.hpp"
#include "fwRenderOgre/compositor/listener/RayExitDepth.hpp"
#include "fwRenderOgre/helper/Camera.hpp"
#include "fwRenderOgre/helper/Shading.hpp"
#include "fwRenderOgre/SRender.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <fwCore/Profiling.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <boost/make_unique.hpp>

#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreMaterial.h>
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

static const std::string s_AUTOSTEREO_DEFINE = "AUTOSTEREO=1";

static const std::string s_AO_DEFINE             = "AMBIENT_OCCLUSION=1";
static const std::string s_COLOR_BLEEDING_DEFINE = "COLOR_BLEEDING=1";
static const std::string s_SHADOWS_DEFINE        = "SHADOWS=1";
static const std::string s_PREINTEGRATION_DEFINE = "PREINTEGRATION=1";

static const std::string s_VOLUME_TF_TEXUNIT_NAME = "volumeTransferFunction";

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::RayTracingVolumeRenderer(std::string parentId,
                                                   Layer::sptr layer,
                                                   ::Ogre::SceneNode* parentNode,
                                                   ::Ogre::TexturePtr imageTexture,
                                                   const TransferFunction::sptr& gpuVolumeTF,
                                                   PreIntegrationTable& preintegrationTable,
                                                   bool ambientOcclusion,
                                                   bool colorBleeding,
                                                   bool shadows,
                                                   double aoFactor,
                                                   double colorBleedingFactor) :
    IVolumeRenderer(parentId, layer->getSceneManager(), parentNode, imageTexture, gpuVolumeTF, preintegrationTable),
    m_entryPointGeometry(nullptr),
    m_proxyGeometry(nullptr),
    m_imageSize(::fwData::Image::SizeType({ 1, 1, 1 })),
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
    m_layer(layer)
{
    auto* exitDepthListener = new compositor::listener::RayExitDepthListener();
    ::Ogre::MaterialManager::getSingleton().addListener(exitDepthListener);

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = layer->getViewport();

    const std::uint8_t numViewPoints  = this->getLayer()->getNumberOfCameras();
    const auto stereoMode             = layer->getStereoMode();
    const auto rayEntryCompositorName = "VolumeEntries" + std::to_string(numViewPoints);
    m_rayEntryCompositor = ::boost::make_unique<RayEntryCompositor> (rayEntryCompositorName, s_PROXY_GEOMETRY_RQ_GROUP,
                                                                     stereoMode, true);

    auto compositorInstance = compositorManager.addCompositor(viewport, rayEntryCompositorName, 0);
    SLM_ERROR_IF("Compositor '" + rayEntryCompositorName + "' not found.", compositorInstance == nullptr);
    compositorInstance->setEnabled(true);

    // First check that we did not already instance Shared parameters
    // This can happen when reinstancing this class (e.g. switching 3D mode)
    ::Ogre::GpuProgramManager::SharedParametersMap spMap =
        ::Ogre::GpuProgramManager::getSingleton().getAvailableSharedParameters();
    if(!spMap["RTVParams"])
    {
        m_RTVSharedParameters = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters("RTVParams");
    }
    else
    {
        m_RTVSharedParameters = spMap["RTVParams"];
    }

    // define the shared param structure
    m_RTVSharedParameters->addConstantDefinition("u_sampleDistance", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_lobeOffset", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_opacityCorrectionFactor", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_volIllumFactor", ::Ogre::GCT_FLOAT4);
    m_RTVSharedParameters->addConstantDefinition("u_min", ::Ogre::GCT_INT1);
    m_RTVSharedParameters->addConstantDefinition("u_max", ::Ogre::GCT_INT1);
    m_RTVSharedParameters->addConstantDefinition("u_tfWindow", ::Ogre::GCT_FLOAT2);
    m_RTVSharedParameters->setNamedConstant("u_opacityCorrectionFactor", m_opacityCorrectionFactor);

    this->initEntryPoints();
    this->createRayTracingMaterial();

    this->setSampling(m_nbSlices);
}

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::~RayTracingVolumeRenderer()
{
    m_camera->removeListener(m_cameraListener);

    m_camera->removeListener(m_cameraListener);
    delete m_cameraListener;
    m_cameraListener = nullptr;

    m_volumeSceneNode->detachObject(m_entryPointGeometry);
    m_volumeSceneNode->detachObject(m_proxyGeometry);

    m_sceneManager->destroyManualObject(m_entryPointGeometry);
    m_sceneManager->destroyMovableObject(m_proxyGeometry);

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = this->getLayer()->getViewport();

    const auto& rayEntryCompositorName = m_rayEntryCompositor->getName();
    compositorManager.setCompositorEnabled(viewport, rayEntryCompositorName, false);
    compositorManager.removeCompositor(viewport, rayEntryCompositorName);

    m_RTVSharedParameters->removeAllConstantDefinitions();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::imageUpdate(::fwData::Image::sptr image, ::fwData::TransferFunction::sptr tf)
{
    if(!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        return;
    }

    this->scaleTranslateCube(image->getSpacing(), image->getOrigin());

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
    else
    {
        auto material  = ::Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName);
        auto technique = material->getTechnique(0);
        SLM_ASSERT("Technique not found", technique);
        auto pass = technique->getPass(0);
        m_gpuVolumeTF.lock()->bind(pass, s_VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateVolumeTF()
{
    FW_PROFILE("TF Update")
    if(!m_preIntegratedRendering)
    {
        auto material  = ::Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName);
        auto technique = material->getTechnique(0);
        SLM_ASSERT("Technique not found", technique);
        auto pass = technique->getPass(0);
        m_gpuVolumeTF.lock()->bind(pass, s_VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
    }
    m_proxyGeometry->computeGrid();
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

bool RayTracingVolumeRenderer::isVisible() const
{
    return m_entryPointGeometry->isVisible() && m_proxyGeometry->isVisible();
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
        auto gpuTF = m_gpuVolumeTF.lock();
        texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setName(s_VOLUME_TF_TEXUNIT_NAME);
        gpuTF->bind(_rayCastingPass, texUnitState->getName(), fpParams);
    }

    fpParams->setNamedConstant("u_tfTexture", numTexUnit++);

    if(_fpPPDefines.find(s_AO_DEFINE) != std::string::npos)
    {
        texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        texUnitState->setTexture(m_illumVolume.lock()->getIlluminationVolume());

        fpParams->setNamedConstant("u_illuminationVolume", numTexUnit++);
        // Update the shader parameter
        m_RTVSharedParameters->setNamedConstant("u_volIllumFactor", m_volIllumFactor);
    }

    // Entry points texture
    texUnitState = _rayCastingPass->createTextureUnitState();
    texUnitState->setName("entryPoints");

    if(this->getLayer()->getNumberOfCameras() == 1)
    {
        const auto& rayEntryCompositorName = m_rayEntryCompositor->getName();
        texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        texUnitState->setCompositorReference(rayEntryCompositorName, rayEntryCompositorName + "Texture");
    }

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
        if(matResource)
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

        if(fpPPDefines.find(s_PREINTEGRATION_DEFINE) == std::string::npos)
        {
            fsp->setParameter("attach", "TransferFunction_FP");
        }

        if(fpPPDefines.size() > 0)
        {
            fsp->setParameter("preprocessor_defines", fpPPDefines);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Create the material
    ::Ogre::MaterialPtr mat = mm.create(m_currentMtlName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    // Ensure that we have the color parameters set for the current material
    this->setMaterialLightParams(mat);
    // Get the already created pass through the already created technique
    ::Ogre::Technique* tech = mat->getTechnique(0);

    ::Ogre::Pass* pass = tech->getPass(0);
    pass->setCullingMode(::Ogre::CULL_ANTICLOCKWISE);
    pass->setSceneBlending(::Ogre::SBT_TRANSPARENT_ALPHA);
    pass->setDepthCheckEnabled(true);
    pass->setDepthWriteEnabled(true);

    // Vertex program
    pass->setVertexProgram(vpName);
    ::Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
    vpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

    // Fragment program
    pass->setFragmentProgram(fpName);
    ::Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
    fpParams->setNamedAutoConstant("u_viewport", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
    fpParams->setNamedAutoConstant("u_clippingNear", ::Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
    fpParams->setNamedAutoConstant("u_clippingFar", ::Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
    fpParams->setNamedAutoConstant("u_cameraPos", ::Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
    fpParams->setNamedAutoConstant("u_shininess", ::Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
    fpParams->setNamedAutoConstant("u_invWorldViewProj",
                                   ::Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEWPROJ_MATRIX);
    fpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    fpParams->setNamedAutoConstant("u_numLights", ::Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
    for(size_t i = 0; i < 10; ++i)
    {
        auto number = "[" + std::to_string(i) + "]";
        fpParams->setNamedAutoConstant("u_lightDir" + number,
                                       ::Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_OBJECT_SPACE, i);
        fpParams->setNamedAutoConstant("u_lightDiffuse" + number,
                                       ::Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR, i);
        fpParams->setNamedAutoConstant("u_lightSpecular" + number,
                                       ::Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR, i);
    }
    fpParams->addSharedParameters("RTVParams");

    ///////////////////////////////////////////////////////////////////////////
    /// Setup texture unit states
    this->setRayCastingPassTextureUnits(pass, fpPPDefines);

    m_entryPointGeometry->setMaterialName(0, m_currentMtlName);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::initEntryPoints()
{
    m_entryPointGeometry = m_sceneManager->createManualObject(m_parentId + "_RayTracingVREntryPoints");

    // Use the default material before the raytracing material is created otherwise we get an error.
    m_entryPointGeometry->begin("Default", ::Ogre::RenderOperation::OT_TRIANGLE_LIST);
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

    // Render volumes after surfaces.
    m_entryPointGeometry->setRenderQueueGroup(compositor::Core::s_VOLUME_RQ_GROUP_ID);

    m_volumeSceneNode->attachObject(m_entryPointGeometry);

    m_proxyGeometry = ::fwRenderOgre::vr::GridProxyGeometry::New(this->m_parentId + "_GridProxyGeometry",
                                                                 m_sceneManager, m_3DOgreTexture,
                                                                 m_gpuVolumeTF.lock(), "RayEntryPoints");

    m_proxyGeometry->setRenderQueueGroup(s_PROXY_GEOMETRY_RQ_GROUP);
    m_volumeSceneNode->attachObject(m_proxyGeometry);

    m_cameraListener = new CameraListener(this);
    m_camera->addListener(m_cameraListener);
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
    mtl->setShininess( 20 );
}

//-----------------------------------------------------------------------------

} // namespace vr
} // namespace fwRenderOgre
