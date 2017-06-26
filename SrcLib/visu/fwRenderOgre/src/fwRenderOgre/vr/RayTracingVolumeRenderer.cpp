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

#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreDepthBuffer.h>
#include <OGRE/OgreException.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreHardwareBufferManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreHardwareVertexBuffer.h>
#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSubMesh.h>
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

class RayTracingVolumeRenderer::JFACompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    JFACompositorListener(float passIndex, float nbPasses, float& blurWeight) :
        m_passIndex(passIndex),
        m_nbPasses(nbPasses),
        m_blurWeight(blurWeight)
    {

    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialSetup(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        if(mtl->getName().find("JFA") != std::string::npos)
        {
            ::Ogre::Technique* tech = mtl->getBestTechnique();

            for(short unsigned int i = 0; i < tech->getNumPasses(); i++ )
            {
                ::Ogre::GpuProgramParametersSharedPtr vrParams = tech->getPass(i)->getFragmentProgramParameters();

                vrParams->setNamedConstant("u_passIndex", m_passIndex);
                vrParams->setNamedConstant("u_nbPasses", m_nbPasses);
            }
        }
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        if(mtl->getName().find("Blur") != std::string::npos)
        {
            ::Ogre::Technique* tech = mtl->getBestTechnique();

            for(short unsigned int i = 0; i < tech->getNumPasses(); i++ )
            {
                ::Ogre::GpuProgramParametersSharedPtr fParams = tech->getPass(i)->getFragmentProgramParameters();
                fParams->setNamedConstant("u_blurWeight", m_blurWeight);
            }
        }
    }

private:

    float m_passIndex;
    float m_nbPasses;

    float& m_blurWeight;
};

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

                    if(!closestLights.empty())
                    {
                        ::Ogre::Vector3 lightDir = m_renderer->m_volumeSceneNode->convertLocalToWorldDirection(
                            closestLights[0]->getDerivedDirection(), true);

                        ::Ogre::Pass* satIllumPass = ::Ogre::MaterialManager::getSingleton().getByName(
                            m_currentMtlName)->getTechnique(0)->getPass(0);
                        ::Ogre::GpuProgramParametersSharedPtr satIllumParams =
                            satIllumPass->getFragmentProgramParameters();

                        satIllumParams->setNamedConstant("u_lightDir", lightDir);

                        m_renderer->m_illumVolume->updateVolIllum();
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

const std::string s_NONE  = "None";
const std::string s_MIMP  = "MImP";
const std::string s_AIMC  = "AImC";
const std::string s_VPIMC = "VPImC";

const std::string s_MIMP_COMPOSITOR         = "IDVR_MImP_Comp";
const std::string s_AIMC_COMPOSITOR         = "IDVR_AImC_Comp";
const std::string s_VPIMC_COMPOSITOR        = "IDVR_VPImC_Comp";
const std::string s_JFAINIT_COMPOSITOR      = "JFAInit";
const std::string s_JFAPING_COMPOSITOR      = "JFAPingComp";
const std::string s_JFAPONG_COMPOSITOR      = "JFAPongComp";
const std::string s_JFAFINALPING_COMPOSITOR = "JFAFinalPingComp";
const std::string s_JFAFINALPONG_COMPOSITOR = "JFAFinalPongComp";
const std::string S_DEFAULTRT_COMPOSITOR    = "DefaultRT";

const std::string s_AO_DEFINE             = "AMBIENT_OCCLUSION=1";
const std::string s_COLOR_BLEEDING_DEFINE = "COLOR_BLEEDING=1";
const std::string s_SHADOWS_DEFINE        = "SHADOWS=1";
const std::string s_PREINTEGRATION_DEFINE = "PREINTEGRATION=1";

const std::string s_MIMP_DEFINE  = "IDVR=1";
const std::string s_AIMC_DEFINE  = "IDVR=2";
const std::string s_VPIMC_DEFINE = "IDVR=3";

const std::string s_CSG_DEFINE                  = "CSG=1";
const std::string s_CSG_BORDER_DEFINE           = "CSG_BORDER=1";
const std::string s_CSG_DISABLE_CONTEXT_DEFINE  = "CSG_DISABLE_CONTEXT=1";
const std::string s_CSG_OPACITY_DECREASE_DEFINE = "CSG_OPACITY_DECREASE=1";
const std::string s_CSG_DEPTH_LINES_DEFINE      = "CSG_DEPTH_LINES=1";

const std::string s_CSG_MOD_GRAYSCALE_AVERAGE_DEFINE    = "CSG_MODULATION=1";
const std::string s_CSG_MOD_GRAYSCALE_LIGHTNESS_DEFINE  = "CSG_MODULATION=2";
const std::string s_CSG_MOD_GRAYSCALE_LUMINOSITY_DEFINE = "CSG_MODULATION=3";
const std::string s_CSG_MOD_COLOR1_DEFINE               = "CSG_MODULATION=4";
const std::string s_CSG_MOD_COLOR2_DEFINE               = "CSG_MODULATION=5";
const std::string s_CSG_MOD_COLOR3_DEFINE               = "CSG_MODULATION=6";
const std::string s_CSG_MOD_COLOR4_DEFINE               = "CSG_MODULATION=7";

const std::string s_IMPORTANCE_COMPOSITING_TEXTURE = "IC";
const std::string s_JUMP_FLOOD_ALGORITHM_TEXTURE   = "JFA";

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::RayTracingVolumeRenderer(std::string parentId,
                                                   Layer::sptr layer,
                                                   ::Ogre::SceneNode* parentNode,
                                                   ::Ogre::TexturePtr imageTexture,
                                                   ::Ogre::TexturePtr maskTexture,
                                                   TransferFunction& gpuTF,
                                                   PreIntegrationTable& preintegrationTable,
                                                   ::fwRenderOgre::Layer::StereoModeType stereoMode,
                                                   bool ambientOcclusion,
                                                   bool colorBleeding,
                                                   bool shadows,
                                                   double aoFactor,
                                                   double colorBleedingFactor) :
    IVolumeRenderer(parentId, layer->getSceneManager(), parentNode, imageTexture, gpuTF, preintegrationTable),
    m_maskTexture(maskTexture),
    m_entryPointGeometry(nullptr),
    m_imageSize(::fwData::Image::SizeType({ 1, 1, 1 })),
    m_stereoMode(stereoMode),
    m_ambientOcclusion(ambientOcclusion),
    m_colorBleeding(colorBleeding),
    m_shadows(shadows),
    m_idvrCSG(false),
    m_idvrCSGSlope(0.3f),
    m_idvrCSGBlurWeight(0.01f),
    m_idvrCSGBorder(false),
    m_idvrCSGDisableContext(false),
    m_idvrCSGBorderThickness(0.05f),
    m_idvrCSGBorderColor(::Ogre::ColourValue(1.f, 0.f, 0.f)),
    m_idvrCSGModulation(false),
    m_idvrCSGModulationMethod(IDVRCSGModulationMethod::COLOR1),
    m_idvrCSGModulationFactor(1.f),
    m_idvrCSGOpacityDecrease(false),
    m_idvrCSGOpacityDecreaseFactor(1.f),
    m_idvrCSGDepthLines(false),
    m_idvrCSGDepthLinesThreshold(0.07f),
    m_idvrAImCAlphaCorrection(0.05f),
    m_idvrVPImCAlphaCorrection(0.3f),
    m_volIllumFactor(static_cast< ::Ogre::Real>(colorBleedingFactor),
                     static_cast< ::Ogre::Real>(colorBleedingFactor),
                     static_cast< ::Ogre::Real>(colorBleedingFactor),
                     static_cast< ::Ogre::Real>(aoFactor)),
    m_opacityCorrectionFactor(200.f),
    m_illumVolume(nullptr),
    m_idvrMethod(s_NONE),
    m_focalLength(0.f),
    m_lobeOffset(1.f),
    m_cameraListener(nullptr),
    m_layer(layer),
    m_autostereoListener(nullptr),
    m_fullScreenQuad(new ::Ogre::Rectangle2D())
{
    m_fullScreenQuad->setCorners(-1, 1, 1, -1);

    m_gridSize  = {{ 2, 2, 2 }};
    m_brickSize = {{ 8, 8, 8 }};

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
        m_RTVSharedParameters->addConstantDefinition("u_countersinkSlope", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_csgBorderThickness", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_colorModulationFactor", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_opacityDecreaseFactor", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_vpimcAlphaCorrection", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_aimcAlphaCorrection", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_depthLinesThreshold", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_sampleDistance", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_lobeOffset", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_opacityCorrectionFactor", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_csgBorderColor", ::Ogre::GCT_FLOAT3);
        m_RTVSharedParameters->addConstantDefinition("u_volIllumFactor", ::Ogre::GCT_FLOAT4);
        m_RTVSharedParameters->addConstantDefinition("u_min", ::Ogre::GCT_INT1);
        m_RTVSharedParameters->addConstantDefinition("u_max", ::Ogre::GCT_INT1);
        m_RTVSharedParameters->setNamedConstant("u_countersinkSlope", m_idvrCSGSlope);
        m_RTVSharedParameters->setNamedConstant("u_csgBorderThickness", m_idvrCSGBorderThickness);
        m_RTVSharedParameters->setNamedConstant("u_colorModulationFactor", m_idvrCSGModulationFactor);
        m_RTVSharedParameters->setNamedConstant("u_opacityDecreaseFactor", m_idvrCSGOpacityDecreaseFactor);
        m_RTVSharedParameters->setNamedConstant("u_csgBorderColor", m_idvrCSGBorderColor);
        m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
        m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
        m_RTVSharedParameters->setNamedConstant("u_depthLinesThreshold", m_idvrCSGDepthLinesThreshold);
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

    this->createRayTracingMaterial();

    this->initEntryPoints();

    this->setSampling(m_nbSlices);
}

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::~RayTracingVolumeRenderer()
{
    delete m_fullScreenQuad;
    m_camera->removeListener(m_cameraListener);

    this->cleanCompositorChain();

    if(m_autostereoListener)
    {
        ::Ogre::MaterialManager::getSingleton().removeListener(m_autostereoListener);
        delete m_autostereoListener;
        m_autostereoListener = nullptr;
    }

    m_camera->removeListener(m_cameraListener);
    delete m_cameraListener;
    m_cameraListener = nullptr;

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
    m_entryPointsTextures.clear();

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
                static_cast<int>(m_imageSize[i]) / m_brickSize[i] +
                (static_cast<int>(m_imageSize[i]) % m_brickSize[i] != 0);
        }

        if(!m_gridTexture.isNull())
        {
            ::Ogre::TextureManager::getSingleton().remove(m_gridTexture->getHandle());
            m_gridTexture.setNull();
        }

        this->createGridTexture();
        this->tfUpdate(tf);
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

void RayTracingVolumeRenderer::setIlluminationVolume(SATVolumeIllumination* illuminationVolume)
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

void RayTracingVolumeRenderer::setIDVRMethod(std::string method)
{
    bool isSupported(false);

    if(method == s_NONE ||
       method == s_MIMP ||
       method == s_AIMC ||
       method == s_VPIMC)
    {
        isSupported = true;
    }

    SLM_ASSERT("IDVR method '" + method + "' isn't supported by the ray tracing volume renderer.", isSupported);
    m_idvrMethod = method;

    this->createRayTracingMaterial();
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

void RayTracingVolumeRenderer::createRayTracingMaterial()
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;
    std::tie(vpPPDefines, fpPPDefines, hash) = this->computeRayTracingDefines();

    ::Ogre::String matName("RTV_Mat_" + std::to_string(hash));
    m_currentMtlName = matName;

    ::Ogre::MaterialManager& mm = ::Ogre::MaterialManager::getSingleton();

    // The material needs to be created only if it doesn't exist
    if(mm.resourceExists(matName))
    {
        this->initCompositors();
        return;
    }

    ::Ogre::HighLevelGpuProgramManager& gpm = ::Ogre::HighLevelGpuProgramManager::getSingleton();

    ///////////////////////////////////////////////////////////////////////////
    /// Compile vertex shader
    ::Ogre::String vpName("RTV_VP_" + std::to_string(hash));

    ::Ogre::HighLevelGpuProgramPtr vsp = gpm.createProgram(vpName, "Materials", "glsl", ::Ogre::GPT_VERTEX_PROGRAM);
    vsp->setSourceFile("RayTracedVolume_VP.glsl");

    if(vpPPDefines.size() > 0)
    {
        vsp->setParameter("preprocessor_defines", vpPPDefines);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// Compile fragment shader
    ::Ogre::String fpName("RTV_FP_" + std::to_string(hash));
    ::Ogre::HighLevelGpuProgramPtr fsp = gpm.createProgram(fpName, "Materials", "glsl", ::Ogre::GPT_FRAGMENT_PROGRAM);
    fsp->setSourceFile("RayTracedVolume_FP.glsl");

    fsp->setParameter("attach", "TransferFunction_FP");
    if(this->m_idvrCSGModulation)
    {
        fsp->setParameter("attach", "ColorFormats_FP");
    }
    if(fpPPDefines.size() > 0)
    {
        fsp->setParameter("preprocessor_defines", fpPPDefines);
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
    for(int j = 0; j < 10; ++j)
    {
        auto number = "[" + std::to_string(j) + "]";
        fpParams->setNamedAutoConstant("u_lightDir" + number, ::Ogre::GpuProgramParameters::ACT_LIGHT_POSITION, j);
        fpParams->setNamedAutoConstant("u_lightDiffuse" + number,
                                       ::Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR, j);
        fpParams->setNamedAutoConstant("u_lightSpecular" + number,
                                       ::Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR, j);
    }
    fpParams->addSharedParameters("RTVParams");

    // We use this map to keep track of the texture units IDs
    std::map<std::string, int> textureUnits;

    ///////////////////////////////////////////////////////////////////////////
    /// Setup texture unit states
    int numTexUnit = 0;
    ::Ogre::TextureUnitState* texUnitState;
    // Volume data
    texUnitState = pass->createTextureUnitState();
    texUnitState->setTextureName(m_3DOgreTexture->getName(), ::Ogre::TEX_TYPE_3D);
    texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
    texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
    textureUnits["u_image"] = numTexUnit;

    fpParams->setNamedConstant("u_image", numTexUnit++);

    // Transfer function
    if(fpPPDefines.find(s_PREINTEGRATION_DEFINE) != std::string::npos)
    {
        texUnitState = pass->createTextureUnitState(m_preIntegrationTable.getTexture()->getName());
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
    }
    else
    {
        texUnitState = pass->createTextureUnitState(m_gpuTF.getTexture()->getName());
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
    }
    textureUnits["u_tfTexture"] = numTexUnit;

    fpParams->setNamedConstant("u_tfTexture", numTexUnit++);

    if(fpPPDefines.find(s_AO_DEFINE) != std::string::npos)
    {
        texUnitState = pass->createTextureUnitState(m_illumVolume->getIlluminationVolume()->getName());
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

        textureUnits["u_illuminationVolume"] = numTexUnit;

        fpParams->setNamedConstant("u_illuminationVolume", numTexUnit++);
        // Update the shader parameter
        m_RTVSharedParameters->setNamedConstant("u_volIllumFactor", m_volIllumFactor);
    }

    // Importance Compositing texture: MImP | AImC | VPImC
    if(m_idvrMethod != s_NONE)
    {
        const std::string compositorRef = m_idvrMethod == s_MIMP ? s_MIMP_COMPOSITOR :
                                          m_idvrMethod == s_AIMC ? s_AIMC_COMPOSITOR : s_VPIMC_COMPOSITOR;
        texUnitState = pass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        texUnitState->setCompositorReference(compositorRef, "IC", 0);

        textureUnits[s_IMPORTANCE_COMPOSITING_TEXTURE] = numTexUnit;

        fpParams->setNamedConstant("u_" + s_IMPORTANCE_COMPOSITING_TEXTURE, numTexUnit++);
    }

    // JFA texture: MImP
    if(m_idvrMethod == s_MIMP)
    {
        texUnitState = pass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        texUnitState->setCompositorReference(s_JFAINIT_COMPOSITOR, "JFAFinal", 0);

        textureUnits[s_JUMP_FLOOD_ALGORITHM_TEXTURE] = numTexUnit;

        fpParams->setNamedConstant("u_" + s_JUMP_FLOOD_ALGORITHM_TEXTURE, numTexUnit++);
    }

    // Alpha Correction: AImC | VPImC
    if(m_idvrMethod == s_AIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
    }
    else if(m_idvrMethod == s_VPIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
    }

    // Entry points texture
    texUnitState = pass->createTextureUnitState();
    texUnitState->setName("entryPoints");
    texUnitState->setTexture(m_entryPointsTextures[0]);
    texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
    texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

    textureUnits["u_entryPoints"] = numTexUnit;

    fpParams->setNamedConstant("u_entryPoints", numTexUnit++);

    ///////////////////////////////////////////////////////////////////////////
    /// Create the technique that is used in ImportanceCompositing.compositor
    if(m_idvrMethod != s_NONE )
    {
        auto tech = mat->createTechnique();
        SLM_ASSERT("Can't create a new technique", tech);

        auto pass = tech->createPass();
        SLM_ASSERT("Can't create a new pass", pass);

        pass->setCullingMode(::Ogre::CullingMode::CULL_ANTICLOCKWISE);
        pass->setSceneBlending(::Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
        pass->setVertexProgram(vpName);

        ::Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
        vpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

        if(m_idvrMethod == s_MIMP)
        {
            tech->setSchemeName("IDVR_MImP_Mat");
            pass->setFragmentProgram("IDVR_MImP_FP");
        }
        else if(m_idvrMethod == s_AIMC)
        {
            tech->setSchemeName("IDVR_AImC_Mat");
            pass->setFragmentProgram("IDVR_AImC_FP");
        }
        else if(m_idvrMethod == s_VPIMC)
        {
            tech->setSchemeName("IDVR_VPImC_Mat");
            pass->setFragmentProgram("IDVR_VPImC_FP");
        }

        texUnitState = pass->createTextureUnitState();
        texUnitState->setTextureName(m_maskTexture->getName(), ::Ogre::TEX_TYPE_3D);
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

        texUnitState = pass->createTextureUnitState();
        texUnitState->setName("entryPoints");
        texUnitState->setTexture(m_entryPointsTextures[0]);
        texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
    }

    this->initCompositors();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::initCompositors()
{
    // Start from an empty compositor chain
    this->cleanCompositorChain();

    // Setup the Default compositor, that notably handles the Widget modifying the entryPoints
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = m_layer.lock()->getViewport();

    auto compositorInstance = compositorManager.addCompositor(viewport, S_DEFAULTRT_COMPOSITOR, 0);
    SLM_ASSERT("Compositor could not be initialized", compositorInstance);
    compositorInstance->setEnabled(true);

    this->buildICCompositors();
    this->getLayer()->requestRender();
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

        this->createGridTexture();
    }

    // Render geometry.
    m_proxyGeometryGenerator->manualUpdate();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::computeEntryPointsTexture()
{
    m_proxyGeometryGenerator->setVisible(false);

    ::Ogre::RenderOperation renderOp;
    m_proxyGeometryGenerator->getRenderOperation(renderOp);
    m_entryPointGeometry->setVisible(true);
    m_entryPointGeometry->setMaterialName(0, m_currentMtlName);

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

        ::Ogre::Pass* pass = m_proxyGeometryGenerator->getMaterial()->getTechnique(0)->getPass("BackFacesMax");
        m_sceneManager->manualRender(&renderOp, pass, entryPtsVp, worldMat, m_camera->getViewMatrix(), projMat);

        // SECOND STEP - Back Faces: find the closest back faces and initialize the entry points with their depths.
        rs->setStencilCheckEnabled(false);
        pass = m_proxyGeometryGenerator->getMaterial()->getTechnique(0)->getPass("BackFaces");
        m_sceneManager->manualRender(&renderOp, pass, entryPtsVp, worldMat, m_camera->getViewMatrix(), projMat);

        // THIRD STEP - Front Faces: update the entry points depths with the closest front faces occluding closest back
        // faces.
        rs->setStencilCheckEnabled(true);

        // Set the stencil buffer to 2 where front faces are rasterized.
        rs->setStencilBufferParams(::Ogre::CMPF_ALWAYS_PASS, 0x2, 0xFFFFFFFF, 0xFFFFFFFF, ::Ogre::SOP_KEEP,
                                   ::Ogre::SOP_KEEP,
                                   ::Ogre::SOP_REPLACE);

        pass = m_proxyGeometryGenerator->getMaterial()->getTechnique(0)->getPass("FrontFaces");
        m_sceneManager->manualRender(&renderOp, pass, entryPtsVp, worldMat, m_camera->getViewMatrix(), projMat);

        // FOURTH STEP - Near Plane: update the entry points depths with the near plane depth when when it is inside a
        // brick.
        // The stencil test passes if the current stencil buffer value is equal to 1.
        // That is, where no front faces have been rasterized.
        rs->setStencilBufferParams(::Ogre::CMPF_EQUAL, 0x1, 0xFFFFFFFF);

        ::Ogre::Matrix4 identity;
        pass = m_proxyGeometryGenerator->getMaterial()->getTechnique(0)->getPass("NearPlane");

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

::Ogre::Matrix4 RayTracingVolumeRenderer::frustumShearTransform(float angle) const
{
    ::Ogre::Matrix4 shearTransform = ::Ogre::Matrix4::IDENTITY;

    const float focalLength  = m_camera->getFocalLength();
    const float xshearFactor = std::tan(angle);

    shearTransform[0][2] = -xshearFactor;
    shearTransform[0][3] = -focalLength * xshearFactor;

    return shearTransform;
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

    if(m_idvrMethod != s_NONE)
    {
        if(m_idvrMethod == s_MIMP)
        {
            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_MIMP_DEFINE;
            if(m_idvrCSG)
            {
                fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_DEFINE;

                if(m_idvrCSGBorder)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_BORDER_DEFINE;
                }
                if(m_idvrCSGDisableContext)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_DISABLE_CONTEXT_DEFINE;
                }
                if(m_idvrCSGOpacityDecrease)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_OPACITY_DECREASE_DEFINE;
                }
                if(m_idvrCSGDepthLines)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_DEPTH_LINES_DEFINE;
                }

                if(m_idvrCSGModulation)
                {
                    switch(m_idvrCSGModulationMethod)
                    {
                        case IDVRCSGModulationMethod::AVERAGE_GRAYSCALE:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_GRAYSCALE_AVERAGE_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::LIGHTNESS_GRAYSCALE:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_GRAYSCALE_LIGHTNESS_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::LUMINOSITY_GRAYSCALE:
                            fpPPDefs <<
                            (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_GRAYSCALE_LUMINOSITY_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR1:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR1_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR2:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR2_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR3:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR3_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR4:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR4_DEFINE;
                            break;
                    }

                }
            }
        }
        else if(m_idvrMethod == s_AIMC)
        {
            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_AIMC_DEFINE;
        }
        else if(m_idvrMethod == s_VPIMC)
        {
            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_VPIMC_DEFINE;
        }
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

void RayTracingVolumeRenderer::buildICCompositors()
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;
    std::tie(vpPPDefines, fpPPDefines, hash) = this->computeRayTracingDefines();

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = m_layer.lock()->getViewport();

    ::Ogre::CompositorInstance* compositorInstance = nullptr;

    if(m_idvrMethod == s_MIMP)
    {
        compositorInstance = compositorManager.addCompositor(viewport, s_MIMP_COMPOSITOR, 0);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        const double numPasses =
            std::nearbyint(std::max(std::log(m_camera->getViewport()->getActualWidth()) / std::log(2.0),
                                    std::log(m_camera->getViewport()->getActualHeight()) / std::log(2.0)));

        int chainIndex = 1;
        compositorInstance = compositorManager.addCompositor(viewport, s_JFAINIT_COMPOSITOR, chainIndex++);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        m_compositorListeners.push_back(new RayTracingVolumeRenderer::JFACompositorListener(static_cast<float>(0),
                                                                                            static_cast<float>(numPasses),
                                                                                            m_idvrCSGBlurWeight));
        compositorInstance->addListener(m_compositorListeners.back());

        int i = 0;
        for(i = 0; i < numPasses - 2; i++)
        {
            if(i % 2 == 0)
            {
                compositorInstance = compositorManager.addCompositor(viewport, s_JFAPING_COMPOSITOR, chainIndex++);
                SLM_ASSERT("Compositor could not be initialized", compositorInstance);
                compositorInstance->setEnabled(true);
            }
            else
            {
                compositorInstance = compositorManager.addCompositor(viewport, s_JFAPONG_COMPOSITOR, chainIndex++);
                SLM_ASSERT("Compositor could not be initialized", compositorInstance);
                compositorInstance->setEnabled(true);
            }

            m_compositorListeners.push_back(new RayTracingVolumeRenderer::JFACompositorListener(static_cast<float>(i +
                                                                                                                   1),
                                                                                                static_cast<float>(
                                                                                                    numPasses),
                                                                                                m_idvrCSGBlurWeight));
            compositorInstance->addListener(m_compositorListeners.back());
        }

        if(i % 2 == 0)
        {
            compositorInstance = compositorManager.addCompositor(viewport, s_JFAFINALPING_COMPOSITOR, chainIndex++);
            SLM_ASSERT("Compositor could not be initialized", compositorInstance);
            compositorInstance->setEnabled(true);
        }
        else
        {
            compositorInstance = compositorManager.addCompositor(viewport, s_JFAFINALPONG_COMPOSITOR, chainIndex++);
            SLM_ASSERT("Compositor could not be initialized", compositorInstance);
            compositorInstance->setEnabled(true);
        }

        m_compositorListeners.push_back(new RayTracingVolumeRenderer::JFACompositorListener(static_cast<float>(i),
                                                                                            static_cast<float>(numPasses),
                                                                                            m_idvrCSGBlurWeight));
        compositorInstance->addListener(m_compositorListeners.back());
    }
    else if(m_idvrMethod == s_AIMC)
    {
        compositorInstance = compositorManager.addCompositor(viewport, s_AIMC_COMPOSITOR, 0);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);
    }
    else if(m_idvrMethod == s_VPIMC)
    {
        compositorInstance = compositorManager.addCompositor(viewport, s_VPIMC_COMPOSITOR, 0);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::cleanCompositorChain()
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = m_layer.lock()->getViewport();

    ::Ogre::CompositorChain* compChain = compositorManager.getCompositorChain(viewport);
    SLM_ASSERT("Can't find compositor chain", compChain);

    // Then clean the whole chain
    const size_t numCompositors = compChain->getNumCompositors();
    std::vector<size_t> removeCompositors;

    for(size_t i = 0; i < numCompositors; ++i)
    {
        ::Ogre::CompositorInstance* targetComp = compChain->getCompositor(i);
        SLM_ASSERT("Compositor instance is null", targetComp);
        if(targetComp->getCompositor()->getName() == s_MIMP_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_AIMC_COMPOSITOR||
           targetComp->getCompositor()->getName() == s_VPIMC_COMPOSITOR||
           targetComp->getCompositor()->getName() == s_JFAINIT_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_JFAPING_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_JFAPONG_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_JFAFINALPING_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_JFAFINALPONG_COMPOSITOR ||
           targetComp->getCompositor()->getName() == S_DEFAULTRT_COMPOSITOR
           )
        {
            removeCompositors.push_back(i);
        }
    }

    for(auto it = removeCompositors.rbegin(); it != removeCompositors.rend(); ++it )
    {
        compChain->removeCompositor(static_cast<size_t>(*it));
    }

    // Remove all associated listeners from the compositor chain
    std::for_each(m_compositorListeners.begin(), m_compositorListeners.end(),
                  [](::Ogre::CompositorInstance::Listener* listener) { delete listener; }
                  );
    m_compositorListeners.clear();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleIDVRCountersinkGeometry(bool CSG)
{
    m_idvrCSG = CSG;

    if(this->m_idvrMethod == s_MIMP)
    {
        this->createRayTracingMaterial();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRCountersinkSlope(double slope)
{
    m_idvrCSGSlope = static_cast<float>(slope);

    if(m_idvrMethod == s_MIMP && m_idvrCSG)
    {
        m_RTVSharedParameters->setNamedConstant("u_countersinkSlope", m_idvrCSGSlope);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRCSGBlurWeight(double blurWeight)
{
    m_idvrCSGBlurWeight = static_cast<float>(blurWeight);

    if(m_idvrMethod == s_MIMP && m_idvrCSG)
    {
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleIDVRCSGBorder(bool border)
{
    m_idvrCSGBorder = border;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createRayTracingMaterial();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleIDVRCSGDisableContext(bool discard)
{
    m_idvrCSGDisableContext = discard;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createRayTracingMaterial();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRCSGBorderThickness(double thickness)
{
    m_idvrCSGBorderThickness = static_cast<float>(thickness);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGBorder)
    {
        m_RTVSharedParameters->setNamedConstant("u_csgBorderThickness", m_idvrCSGBorderThickness);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRCSGBorderColor(std::array<std::uint8_t, 4> color)
{
    m_idvrCSGBorderColor.r = color[0] / 256.f;
    m_idvrCSGBorderColor.g = color[1] / 256.f;
    m_idvrCSGBorderColor.b = color[2] / 256.f;

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGBorder)
    {
        m_RTVSharedParameters->setNamedConstant("u_csgBorderColor", m_idvrCSGBorderColor);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleIDVRCSGModulation(bool modulation)
{
    m_idvrCSGModulation = modulation;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createRayTracingMaterial();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRCSModulationMethod(IDVRCSGModulationMethod method)
{
    m_idvrCSGModulationMethod = method;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGModulation)
    {
        this->createRayTracingMaterial();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRCSGModulationFactor(double modulationFactor)
{
    m_idvrCSGModulationFactor = static_cast<float>(modulationFactor);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        m_RTVSharedParameters->setNamedConstant("u_colorModulationFactor", m_idvrCSGModulationFactor);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleIDVRCSGOpacityDecrease(bool opacityDecrease)
{
    m_idvrCSGOpacityDecrease = opacityDecrease;

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createRayTracingMaterial();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRCSGOpacityDecreaseFactor(double opacityDecrease)
{
    m_idvrCSGOpacityDecreaseFactor = static_cast<float>(opacityDecrease);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        m_RTVSharedParameters->setNamedConstant("u_opacityDecreaseFactor", m_idvrCSGOpacityDecreaseFactor);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleIDVRDepthLines(bool depthLines)
{
    m_idvrCSGDepthLines = depthLines;

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGBorder)
    {
        this->createRayTracingMaterial();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRCSGDepthLinesThreshold(double threshold)
{
    m_idvrCSGDepthLinesThreshold = static_cast<float>(threshold);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGBorder && this->m_idvrCSGDepthLines)
    {
        m_RTVSharedParameters->setNamedConstant("u_depthLinesThreshold", m_idvrCSGDepthLinesThreshold);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRAImCAlphaCorrection(double alphaCorrection)
{
    m_idvrAImCAlphaCorrection = static_cast<float>(alphaCorrection);

    if(m_idvrMethod == s_AIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRVPImCAlphaCorrection(double alphaCorrection)
{
    m_idvrVPImCAlphaCorrection = static_cast<float>(alphaCorrection);

    if(m_idvrMethod == s_VPIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
        this->getLayer()->requestRender();
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

        gridGeneratorParams->setNamedConstant("u_brickSize", m_brickSize.data(), 3, 1);
        gridGeneratorParams->setNamedConstant("u_sampleDistance", m_sampleDistance);

        ::Ogre::MaterialPtr geomGeneratorMtl = ::Ogre::MaterialManager::getSingleton().getByName("VolumeBricks");
        ::Ogre::Pass* geomGenerationPass     = geomGeneratorMtl->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorVtxParams = geomGenerationPass->getVertexProgramParameters();
        geomGeneratorVtxParams->setNamedConstant("u_gridResolution", m_gridSize.data(), 3, 1);

        ::Ogre::GpuProgramParametersSharedPtr geomGeneratorGeomParams =
            geomGenerationPass->getGeometryProgramParameters();

        // Cast size_t to int.
        const std::vector<int> imageSize(m_imageSize.begin(), m_imageSize.end());

        geomGeneratorGeomParams->setNamedConstant("u_imageResolution", imageSize.data(), 3, 1);
        geomGeneratorGeomParams->setNamedConstant("u_brickSize", m_brickSize.data(), 3, 1);

        ::Ogre::TextureUnitState* gridTexState = geomGenerationPass->getTextureUnitState("gridVolume");

        SLM_ASSERT("'gridVolume' texture unit is not found", gridTexState);
        gridTexState->setTexture(m_gridTexture);
    }
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace fwRenderOgre
