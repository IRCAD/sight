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
#include <regex>
#include <string>

namespace fwRenderOgre
{

namespace vr
{

class RayTracingVolumeRenderer::JFACompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    JFACompositorListener(float passIndex, float nbPasses) :
        m_passIndex(passIndex),
        m_nbPasses(nbPasses)
    {

    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialSetup(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        ::Ogre::Technique* tech = mtl->getBestTechnique();

        for(short unsigned int i = 0; i < tech->getNumPasses(); i++ )
        {
            ::Ogre::GpuProgramParametersSharedPtr vrParams = tech->getPass(i)->getFragmentProgramParameters();

            vrParams->setNamedConstant("u_passIndex", m_passIndex);
            vrParams->setNamedConstant("u_nbPasses", m_nbPasses);
        }
    }

private:

    float m_passIndex;
    float m_nbPasses;
};

//-----------------------------------------------------------------------------

class RayTracingVolumeRenderer::ICCompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    ICCompositorListener(std::vector< ::Ogre::Matrix4>& invWorldViewProj,
                         ::Ogre::TexturePtr maskTexture,
                         ::Ogre::TexturePtr entryPoints,
                         float& sampleDistance) :
        m_invWorldViewProj(invWorldViewProj),
        m_maskTexture(maskTexture),
        m_entryPoints(entryPoints),
        m_sampleDistance(sampleDistance)
    {

    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialSetup(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        ::Ogre::Pass* pass = mtl->getTechnique(0)->getPass(0);

        ::Ogre::TextureUnitState* maskTexUnitState        = pass->getTextureUnitState("mask");
        ::Ogre::TextureUnitState* entryPointsTexUnitState = pass->getTextureUnitState("entryPoints");

        maskTexUnitState->setTextureName(m_maskTexture->getName());
        entryPointsTexUnitState->setTextureName(m_entryPoints->getName());
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        ::Ogre::Pass* pass = mtl->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr vrParams = pass->getFragmentProgramParameters();
        vrParams->setNamedConstant("u_invWorldViewProj", m_invWorldViewProj.data(), m_invWorldViewProj.size());
        vrParams->setNamedConstant("u_sampleDistance", m_sampleDistance);
    }

private:

    std::vector< ::Ogre::Matrix4>& m_invWorldViewProj;

    ::Ogre::TexturePtr m_maskTexture;
    ::Ogre::TexturePtr m_entryPoints;

    float& m_sampleDistance;
};

//-----------------------------------------------------------------------------

class RayTracingVolumeRenderer::RayTracingCompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    RayTracingCompositorListener(std::vector< ::Ogre::Matrix4>& invWorldViewProj,
                                 bool is3DMode,
                                 ::Ogre::SceneNode* volumeSceneNode) :
        m_invWorldViewProj(invWorldViewProj),
        m_is3DMode(is3DMode),
        m_volumeSceneNode(volumeSceneNode)
    {

    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        ::Ogre::Pass* pass = mtl->getTechnique(0)->getPass(0);

        ::Ogre::GpuProgramParametersSharedPtr vrParams = pass->getFragmentProgramParameters();

        if(!m_is3DMode)
        {
            vrParams->setNamedConstant("u_invWorldViewProj", m_invWorldViewProj.data(), m_invWorldViewProj.size());
        }
        else
        {
            vrParams->setNamedConstant("u_invWorldViewProjs", m_invWorldViewProj.data(), m_invWorldViewProj.size());
        }

        // Set light directions in shader.
        ::Ogre::LightList closestLights = m_volumeSceneNode->getAttachedObject(0)->queryLights();

        const size_t numLights = closestLights.size();

        vrParams->setNamedConstant("u_numLights", static_cast<int>(numLights));

        for(unsigned i = 0; i < numLights; ++i)
        {
            ::Ogre::Vector3 lightDir = -closestLights[i]->getDerivedDirection();
            vrParams->setNamedConstant("u_lightDir[" + std::to_string(i) + "]", lightDir);

            ::Ogre::ColourValue colourDiffuse = closestLights[i]->getDiffuseColour();
            vrParams->setNamedConstant("u_lightDiffuse[" + std::to_string(i) + "]", colourDiffuse);

            ::Ogre::ColourValue colourSpecular = closestLights[i]->getSpecularColour();
            vrParams->setNamedConstant("u_lightSpecular[" + std::to_string(i) + "]", colourSpecular);
        }

        ::Ogre::Vector4 shininess(10.f, 0.f, 0.f, 0.f);
        vrParams->setNamedConstant("u_shininess", shininess);
    }

private:

    const std::vector< ::Ogre::Matrix4>& m_invWorldViewProj;

    const bool m_is3DMode;

    ::Ogre::SceneNode* m_volumeSceneNode;
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

                    ::Ogre::Vector3 lightDir = m_renderer->m_volumeSceneNode->convertLocalToWorldDirection(
                        closestLights[0]->getDerivedDirection(), true);

                    ::Ogre::Pass* satIllumPass = ::Ogre::MaterialManager::getSingleton().getByName(
                        m_currentMtlName)->getTechnique(0)->getPass(0);
                    ::Ogre::GpuProgramParametersSharedPtr satIllumParams = satIllumPass->getFragmentProgramParameters();

                    satIllumParams->setNamedConstant("u_lightDir", lightDir);

                    m_renderer->m_illumVolume->updateVolIllum();
                }
                // Recompute the focal length in case the camera moved.
                m_renderer->computeRealFocalLength();

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

const std::string fwRenderOgre::vr::RayTracingVolumeRenderer::s_MIMP_COMPOSITOR  = "IDVR_MImP_Comp";
const std::string fwRenderOgre::vr::RayTracingVolumeRenderer::s_AIMC_COMPOSITOR  = "IDVR_AImC_Comp";
const std::string fwRenderOgre::vr::RayTracingVolumeRenderer::s_VPIMC_COMPOSITOR = "IDVR_VPImC_Comp";

const std::string fwRenderOgre::vr::RayTracingVolumeRenderer::s_MIMP_DEFINE  = "IDVR=1";
const std::string fwRenderOgre::vr::RayTracingVolumeRenderer::s_AIMC_DEFINE  = "IDVR=2";
const std::string fwRenderOgre::vr::RayTracingVolumeRenderer::s_VPIMC_DEFINE = "IDVR=3";

const std::string fwRenderOgre::vr::RayTracingVolumeRenderer::s_IMPORTANCE_COMPOSITING_TEXTURE = "IC";
const std::string fwRenderOgre::vr::RayTracingVolumeRenderer::s_JUMP_FLOOD_ALGORITHM_TEXTURE   = "JFA";

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::RayTracingVolumeRenderer(std::string parentId,
                                                   Layer::sptr layer,
                                                   ::Ogre::SceneNode* parentNode,
                                                   ::Ogre::TexturePtr imageTexture,
                                                   ::Ogre::TexturePtr maskTexture,
                                                   TransferFunction& gpuTF,
                                                   PreIntegrationTable& preintegrationTable,
                                                   ::fwRenderOgre::Layer::StereoModeType mode3D,
                                                   bool ambientOcclusion,
                                                   bool colorBleeding,
                                                   bool shadows,
                                                   double aoFactor,
                                                   double colorBleedingFactor) :
    IVolumeRenderer(parentId, layer->getSceneManager(), parentNode, imageTexture, gpuTF, preintegrationTable),
    m_maskTexture(maskTexture),
    m_entryPointGeometry(nullptr),
    m_imageSize(::fwData::Image::SizeType({ 1, 1, 1 })),
    m_mode3D(mode3D),
    m_fpPPDefines(""),
    m_ambientOcclusion(ambientOcclusion),
    m_colorBleeding(colorBleeding),
    m_shadows(shadows),
    m_idvrCSG(false),
    m_idvrCSGSlope(0.3f),
    m_idvrAImCAlphaCorrection(0.05f),
    m_idvrVPImCAlphaCorrection(0.3f),
    m_volIllumFactor(static_cast< ::Ogre::Real>(colorBleedingFactor),
                     static_cast< ::Ogre::Real>(colorBleedingFactor),
                     static_cast< ::Ogre::Real>(colorBleedingFactor),
                     static_cast< ::Ogre::Real>(aoFactor)),
    m_illumVolume(nullptr),
    m_idvrMethod("None"),
    m_focalLength(0.f),
    m_lobeOffset(1.f),
    m_cameraListener(nullptr),
    m_layer(layer)
{
    m_gridSize   = { 2, 2, 2 };
    m_bricksSize = { 8, 8, 8 };

    const unsigned nbViewpoints = m_mode3D == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8 ? 8 :
                                  m_mode3D == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5 ? 5 : 1;

    for(unsigned i = 0; i < nbViewpoints; ++i)
    {
        m_entryPointsTextures.push_back(::Ogre::TextureManager::getSingleton().createManual(
                                            m_parentId + "_entryPointsTexture" + std::to_string(i),
                                            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                            ::Ogre::TEX_TYPE_2D,
                                            static_cast<unsigned int>(m_camera->getViewport()->getActualWidth()),
                                            static_cast<unsigned int>(m_camera->getViewport()->getActualHeight()),
                                            1,
                                            0,
                                            ::Ogre::PF_FLOAT32_RGB,
                                            ::Ogre::TU_RENDERTARGET ));
    }

    for(::Ogre::TexturePtr entryPtsText : m_entryPointsTextures)
    {
        ::Ogre::RenderTexture* renderTexture = entryPtsText->getBuffer()->getRenderTarget();
        renderTexture->addViewport(m_camera);
    }

    // First check that we did not already instanced Shared parameters
    // This can happen when reinstancing this class (e.g. switching 3D mode)
    try
    {
        m_RTVSharedParameters = ::Ogre::GpuProgramManager::getSingleton().getSharedParameters("RTVParams");
    }
    catch(::Ogre::InvalidParametersException e)
    {
        m_RTVSharedParameters = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters("RTVParams");

        // define the shared param structure
        m_RTVSharedParameters->addConstantDefinition("u_countersinkSlope", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_vpimcAlphaCorrection", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_aimcAlphaCorrection", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_sampleDistance", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_lobeOffset", ::Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_volIllumFactor", ::Ogre::GCT_FLOAT4);
        m_RTVSharedParameters->addConstantDefinition("u_min", ::Ogre::GCT_INT1);
        m_RTVSharedParameters->addConstantDefinition("u_max", ::Ogre::GCT_INT1);
        m_RTVSharedParameters->setNamedConstant("u_countersinkSlope", m_idvrCSGSlope);
        m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
        m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
    }

    initCompositors();
    initEntryPoints();

    setSampling(m_nbSlices);
}

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::~RayTracingVolumeRenderer()
{
    m_camera->removeListener(m_cameraListener);

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = m_layer.lock()->getViewport();
    ::Ogre::CompositorChain* compChain = compositorManager.getCompositorChain(viewport);

    this->cleanCompositorChain(compChain);

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

void RayTracingVolumeRenderer::addRayTracingCompositor()
{
    // Get the required managers
    // We need to use the GpuProgramManager and not the HighLevelGpuProgramManager
    // Because we must be able to clear the microcode cache of previous version of shaders
    // Otherwise we end up using the same version of the shader when recompiling with different preprocessor options
    ::Ogre::GpuProgramManager* gpm = ::Ogre::GpuProgramManager::getSingletonPtr();

    //gpm->setSaveMicrocodesToCache(false);

    ::Ogre::MaterialManager* mm   = ::Ogre::MaterialManager::getSingletonPtr();
    ::Ogre::CompositorManager& cm = ::Ogre::CompositorManager::getSingleton();
    ::Ogre::TextureManager& tm    = ::Ogre::TextureManager::getSingleton();

    // Remove resources and purge microcodes from cache to avoid cache effects when using preprocessor defines
    ::Ogre::ResourcePtr resource;
    resource = gpm->getResourceByName("RTV_VP", ::Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    if(!resource.isNull())
    {
        gpm->remove(resource);
        gpm->removeMicrocodeFromCache(resource->getName());
    }
    resource = gpm->getResourceByName("RTV_FP", ::Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    if(!resource.isNull())
    {
        gpm->remove(resource);
        gpm->removeMicrocodeFromCache(resource->getName());
    }
    resource = mm->getResourceByName("RTV_Mat", ::Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    if(!resource.isNull())
    {
        mm->remove(resource);
    }
    resource = cm.getResourceByName("RTV_Comp", ::Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    if(!resource.isNull())
    {
        cm.remove(resource);
    }

    // Vertex shader
    Ogre::GpuProgramPtr vsp = gpm->createProgram("RTV_VP", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                 "RenderQuad_VP.glsl", Ogre::GPT_VERTEX_PROGRAM, "glsl");
    vsp->escalateLoading();
    vsp->load();

    // Fragment shader
    ::Ogre::GpuProgramPtr fsp = gpm->createProgram("RTV_FP", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                   "RayTracedVolume_FP.glsl", ::Ogre::GPT_FRAGMENT_PROGRAM, "glsl");
    fsp->setParameter("attach", "TransferFunction_FP");
    if(m_fpPPDefines.size() > 0)
    {
        fsp->setParameter("preprocessor_defines", m_fpPPDefines);
    }
    fsp->escalateLoading();
    fsp->load();

    // Material
    ::Ogre::MaterialPtr mat = mm->create("RTV_Mat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    // Get the already created pass through the already created technique
    Ogre::Technique* tech = mat->getTechnique(0);
    Ogre::Pass* pass      = tech->getPass(0);
    pass->setCullingMode(::Ogre::CULL_NONE);
    pass->setDepthCheckEnabled(false);
    pass->setSceneBlending(::Ogre::SBF_SOURCE_ALPHA, ::Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);

    pass->setVertexProgram("RTV_VP");
    pass->getVertexProgramParameters()->setNamedAutoConstant("u_worldViewProj",
                                                             Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

    pass->setFragmentProgram("RTV_FP");
    pass->getFragmentProgramParameters()->setNamedAutoConstant("u_viewportWidth",
                                                               Ogre::GpuProgramParameters::ACT_VIEWPORT_WIDTH);
    pass->getFragmentProgramParameters()->setNamedAutoConstant("u_viewportHeight",
                                                               Ogre::GpuProgramParameters::ACT_VIEWPORT_HEIGHT);
    pass->getFragmentProgramParameters()->setNamedAutoConstant("u_clippingNear",
                                                               Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
    pass->getFragmentProgramParameters()->setNamedAutoConstant("u_clippingFar",
                                                               Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
    pass->getFragmentProgramParameters()->setNamedAutoConstant("u_cameraPos",
                                                               Ogre::GpuProgramParameters::ACT_LOD_CAMERA_POSITION);
    pass->getFragmentProgramParameters()->addSharedParameters("RTVParams");

    // We use this map to keep track of the texture units IDs
    std::map<std::string, int> textureUnits;

    // Setup textures
    int numTexUnit = 0;
    ::Ogre::TextureUnitState* texUnitState;
    // Volume data
    texUnitState = pass->createTextureUnitState();
    texUnitState->setTextureName(m_3DOgreTexture->getName(), ::Ogre::TEX_TYPE_3D);
    texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
    texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
    textureUnits["u_image"] = numTexUnit;
    pass->getFragmentProgramParameters()->setNamedConstant("u_image", numTexUnit++);

    // Transfer function
    if(m_fpPPDefines.find("PREINTEGRATION=1") != std::string::npos)
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
    pass->getFragmentProgramParameters()->setNamedConstant("u_tfTexture", numTexUnit++);

    if(m_fpPPDefines.find("AMBIENT_OCCLUSION=1") != std::string::npos)
    {
        texUnitState = pass->createTextureUnitState(m_illumVolume->getIlluminationVolume()->getName());
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

        textureUnits["u_illuminationVolume"] = numTexUnit;
        pass->getFragmentProgramParameters()->setNamedConstant("u_illuminationVolume", numTexUnit++);
        // Update the shader parameter
        m_RTVSharedParameters->setNamedConstant("u_volIllumFactor", m_volIllumFactor);
    }

    // Importance Compositing texture: MImP | AImC | VPImC
    if(m_fpPPDefines.find(this->s_MIMP_DEFINE) != std::string::npos ||
       m_fpPPDefines.find(this->s_AIMC_DEFINE) != std::string::npos ||
       m_fpPPDefines.find(this->s_VPIMC_DEFINE) != std::string::npos)
    {
        texUnitState = pass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

        textureUnits[this->s_IMPORTANCE_COMPOSITING_TEXTURE] = numTexUnit;
        pass->getFragmentProgramParameters()->setNamedConstant("u_" + this->s_IMPORTANCE_COMPOSITING_TEXTURE,
                                                               numTexUnit++);
    }

    // JFA texture: MImP
    if(m_fpPPDefines.find(this->s_MIMP_DEFINE) != std::string::npos)
    {
        texUnitState = pass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

        textureUnits[this->s_JUMP_FLOOD_ALGORITHM_TEXTURE] = numTexUnit;
        pass->getFragmentProgramParameters()->setNamedConstant("u_" + this->s_JUMP_FLOOD_ALGORITHM_TEXTURE,
                                                               numTexUnit++);
    }

    // Alpha Correction: AImC | VPImC
    if(m_fpPPDefines.find(this->s_AIMC_DEFINE) != std::string::npos ||
       m_fpPPDefines.find(this->s_VPIMC_DEFINE) != std::string::npos)
    {
        if(m_idvrMethod == "AImC")
        {
            m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
        }
        else
        {
            m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
        }
    }

    /* mono mode */
    if(m_mode3D == ::fwRenderOgre::Layer::StereoModeType::NONE)
    {
        texUnitState = pass->createTextureUnitState(m_entryPointsTextures[0]->getName());
        texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

        textureUnits["u_entryPoints"] = numTexUnit;
        pass->getFragmentProgramParameters()->setNamedConstant("u_entryPoints", numTexUnit++);
    }
    /* stereo mode */
    else
    {
        for(unsigned i = 0; i < m_entryPointsTextures.size(); ++i)
        {
            ::Ogre::TextureUnitState* texUnitState = pass->createTextureUnitState(m_entryPointsTextures[i]->getName());
            texUnitState->setTextureName(m_entryPointsTextures[i]->getName());

            std::ostringstream oss;
            oss << "u_entryPoints" << i;
            textureUnits[oss.str()] = numTexUnit;
            pass->getFragmentProgramParameters()->setNamedConstant(oss.str(), numTexUnit++);
        }

        m_RTVSharedParameters->setNamedConstant("u_lobeOffset", m_lobeOffset);
    }

    // Create compositor
    ::Ogre::CompositorPtr comp = cm.create("RTV_Comp", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    {
        ::Ogre::CompositionTechnique* ct = comp->createTechnique();
        {
            // Create references to textures defined in previous compositor with chain_scope
            // In order to locally access them (similar to the texture_ref keyword)
            if(m_fpPPDefines.find(this->s_MIMP_DEFINE) != std::string::npos)
            {
                ::Ogre::CompositionTechnique::TextureDefinition* def = ct->createTextureDefinition(
                    this->s_IMPORTANCE_COMPOSITING_TEXTURE);
                def->refCompName = this->s_MIMP_COMPOSITOR;
                def->refTexName  = this->s_IMPORTANCE_COMPOSITING_TEXTURE;

                def              = ct->createTextureDefinition(this->s_JUMP_FLOOD_ALGORITHM_TEXTURE);
                def->refCompName = "JFAInit";
                def->refTexName  = "JFAFinal";
            }
            else if(m_fpPPDefines.find(this->s_AIMC_DEFINE) != std::string::npos)
            {
                ::Ogre::CompositionTechnique::TextureDefinition* def = ct->createTextureDefinition(
                    this->s_IMPORTANCE_COMPOSITING_TEXTURE);
                def->refCompName = this->s_AIMC_COMPOSITOR;
                def->refTexName  = this->s_IMPORTANCE_COMPOSITING_TEXTURE;
            }
            else if(m_fpPPDefines.find(this->s_VPIMC_DEFINE) != std::string::npos)
            {
                ::Ogre::CompositionTechnique::TextureDefinition* def = ct->createTextureDefinition(
                    this->s_IMPORTANCE_COMPOSITING_TEXTURE);
                def->refCompName = this->s_VPIMC_COMPOSITOR;
                def->refTexName  = this->s_IMPORTANCE_COMPOSITING_TEXTURE;
            }

            // Modify the target_output pass
            ::Ogre::CompositionTargetPass* ctp = ct->getOutputTargetPass();
            {
                ctp->setInputMode(::Ogre::CompositionTargetPass::IM_PREVIOUS);

                {
                    ::Ogre::CompositionPass* cp = ctp->createPass();
                    cp->setType(::Ogre::CompositionPass::PT_CLEAR);
                    cp->setClearBuffers(::Ogre::FBT_DEPTH);
                }
                {
                    ::Ogre::CompositionPass* cp = ctp->createPass();
                    cp->setType(::Ogre::CompositionPass::PT_RENDERQUAD);
                    cp->setMaterialName("RTV_Mat");

                    // add locally defined texture_ref as input to the corresponding texture unit
                    if(m_fpPPDefines.find(this->s_MIMP_DEFINE) != std::string::npos ||
                       m_fpPPDefines.find(this->s_AIMC_DEFINE) != std::string::npos ||
                       m_fpPPDefines.find(this->s_VPIMC_DEFINE) != std::string::npos)
                    {
                        cp->setInput(textureUnits[this->s_IMPORTANCE_COMPOSITING_TEXTURE],
                                     this->s_IMPORTANCE_COMPOSITING_TEXTURE);
                    }

                    if(m_fpPPDefines.find(this->s_MIMP_DEFINE) != std::string::npos)
                    {
                        cp->setInput(textureUnits[this->s_JUMP_FLOOD_ALGORITHM_TEXTURE],
                                     this->s_JUMP_FLOOD_ALGORITHM_TEXTURE);
                    }
                }
            }
        }
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
                static_cast<int>(m_imageSize[i] / m_bricksSize[i] + (m_imageSize[i] % m_bricksSize[i] != 0));
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

    // Trigger compositor re-initialization when changing the illumination volume
    // to make sure the texture has been taken care of
    this->initCompositors();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setPreIntegratedRendering(bool preIntegratedRendering)
{
    OSLM_WARN_IF("Stereoscopic rendering doesn't implement pre-integration yet.",
                 m_mode3D != ::fwRenderOgre::Layer::StereoModeType::NONE && preIntegratedRendering);

    m_preIntegratedRendering = preIntegratedRendering;

    this->initCompositors();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setAmbientOcclusion(bool ambientOcclusion)
{
    m_ambientOcclusion = ambientOcclusion;

    this->updateVolIllumMat();

    this->initCompositors();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setColorBleeding(bool colorBleeding)
{
    m_colorBleeding = colorBleeding;

    this->updateVolIllumMat();

    this->initCompositors();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setShadows(bool shadows)
{
    m_shadows = shadows;

    this->updateVolIllumMat();

    this->initCompositors();
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
    m_idvrMethod = method;

    this->initCompositors();
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
    for(::Ogre::TexturePtr entryPtsTexture : m_entryPointsTextures)
    {
        entryPtsTexture->freeInternalResources();

        entryPtsTexture->setWidth(static_cast< ::Ogre::uint32>(w));
        entryPtsTexture->setHeight(static_cast< ::Ogre::uint32>(h));

        entryPtsTexture->createInternalResources();

        ::Ogre::RenderTexture* renderTexture = entryPtsTexture->getBuffer()->getRenderTarget();
        renderTexture->addViewport(m_camera);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::initCompositors()
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = m_layer.lock()->getViewport();
    ::Ogre::CompositorChain* compChain = compositorManager.getCompositorChain(viewport);
    ::Ogre::CompositorInstance* compositorInstance;

    // Start from an empty compositor chain
    this->cleanCompositorChain(compChain);

    // Setup the Default compositor, that notably handles the Widget modifying the entryPoints
    compositorInstance = compositorManager.addCompositor(viewport, "Default");
    SLM_ASSERT("Compositor could not be initialized", compositorInstance);
    compositorInstance->setEnabled(true);
    // Push back a dummy listener
    m_compositorListeners.push_back(nullptr);

    // Add the initial ray tracing compositor
    this->buildCompositorChain();

    // Setup the Final Chain Compositor
    compositorInstance = compositorManager.addCompositor(viewport, "FinalChainCompositor");
    SLM_ASSERT("Compositor could not be initialized", compositorInstance);
    compositorInstance->setEnabled(true);
    // Push back a dummy listener
    m_compositorListeners.push_back(nullptr);

    this->getLayer()->requestRender();
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

    m_entryPointGeometry->setVisible(false);

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
    m_entryPointGeometry->setVisible(false);

    ::Ogre::Matrix4 worldMat;
    m_proxyGeometryGenerator->getWorldTransforms(&worldMat);

    float eyeAngle = 0.f;
    float angle    = 0.f;
    if(m_mode3D == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5)
    {
        eyeAngle = 0.02321f;
        angle    = eyeAngle * -2.f;
    }
    else if(m_mode3D == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8)
    {
        eyeAngle = 0.01625f;
        angle    = eyeAngle * -3.5f;
    }

    m_viewPointMatrices.clear();

    for(::Ogre::TexturePtr entryPtsText : m_entryPointsTextures)
    {
        ::Ogre::Matrix4 projMat = m_camera->getProjectionMatrix();

        ::Ogre::RenderTexture* renderTexture = entryPtsText->getBuffer()->getRenderTarget();
        renderTexture->getViewport(0)->clear(::Ogre::FBT_COLOUR | ::Ogre::FBT_DEPTH, ::Ogre::ColourValue::White);

        if(m_mode3D == ::fwRenderOgre::Layer::StereoModeType::NONE)
        {
            ::Ogre::Matrix4 worldViewProj = projMat * m_camera->getViewMatrix() * worldMat;
            m_viewPointMatrices.push_back(worldViewProj.inverse());
        }
        else
        {
            const ::Ogre::Matrix4 shearTransform = frustumShearTransform(angle);

            angle  += eyeAngle;
            projMat = projMat * shearTransform;

            ::Ogre::Matrix4 worldViewProj = projMat * m_camera->getViewMatrix() * worldMat;
            m_viewPointMatrices.push_back(worldViewProj.inverse());
        }

        m_sceneManager->manualRender(&renderOp, pass, renderTexture->getViewport(0), worldMat,
                                     m_camera->getViewMatrix(), projMat);
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

void RayTracingVolumeRenderer::updateVolIllumMat()
{
    std::string volIllumMtl = "VolIllum";

    volIllumMtl += m_ambientOcclusion || m_colorBleeding ? "_AO" : "";
    volIllumMtl += m_shadows ? "_Shadows" : "";

    SLM_ASSERT("Camera listener not instantiated", m_cameraListener);
    m_cameraListener->setCurrentMtlName(volIllumMtl);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateCompositorName()
{
    std::ostringstream ppDefs;

    ppDefs.str("");

    if(m_mode3D == ::fwRenderOgre::Layer::StereoModeType::NONE)
    {
        if(m_ambientOcclusion)
        {
            ppDefs << (ppDefs.str() == "" ? "" : ",") << "AMBIENT_OCCLUSION=1";
        }

        if(m_colorBleeding)
        {
            ppDefs << (ppDefs.str() == "" ? "" : ",") << "COLOR_BLEEDING=1";
        }

        if(m_shadows)
        {
            ppDefs << (ppDefs.str() == "" ? "" : ",") << "SHADOWS=1";
        }

        if(m_preIntegratedRendering)
        {
            ppDefs << (ppDefs.str() == "" ? "" : ",") << "PREINTEGRATION=1";
        }

        if(m_idvrMethod != "None")
        {
            if(m_idvrMethod == "MImP")
            {
                ppDefs << (ppDefs.str() == "" ? "" : ",") << "IDVR=1";
                if(m_idvrCSG)
                {
                    ppDefs << (ppDefs.str() == "" ? "" : ",") << "CSG=1";
                }
            }
            else if(m_idvrMethod == "AImC")
            {
                ppDefs << (ppDefs.str() == "" ? "" : ",") << "IDVR=2";
            }
            else if(m_idvrMethod == "VPImC")
            {
                ppDefs << (ppDefs.str() == "" ? "" : ",") << "IDVR=3";
            }
        }
    }
    else
    {
        ppDefs << (ppDefs.str() == "" ? "" : ",") << "MODE3D=1";
        if(m_mode3D == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5)
        {
            ppDefs << (ppDefs.str() == "" ? "" : ",") << "VIEWPOINTS=5";
        }
        else if(m_mode3D == ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8)
        {
            ppDefs << (ppDefs.str() == "" ? "" : ",") << "VIEWPOINTS=8";
        }
        else
        {
            ppDefs << (ppDefs.str() == "" ? "" : ",") << "VIEWPOINTS=1";
        }
    }

    m_fpPPDefines = ppDefs.str();
}

//-----------------------------------------------------------------------------

::Ogre::GpuProgramParametersSharedPtr RayTracingVolumeRenderer::retrieveCurrentProgramParams()
{
    ::Ogre::MaterialPtr currentMtl = ::Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName,
                                                                                       ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    OSLM_ASSERT("Material '" + m_currentMtlName + "' not found", !currentMtl.isNull());

    return currentMtl->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
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

void RayTracingVolumeRenderer::buildCompositorChain()
{
    this->updateCompositorName();

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = m_layer.lock()->getViewport();

    ::Ogre::CompositorInstance* compositorInstance = nullptr;
    std::string idvrCompositorName;

    if(m_idvrMethod == "MImP")
    {
        idvrCompositorName = "IDVR_MImP_Comp";

        compositorInstance = compositorManager.addCompositor(viewport, idvrCompositorName);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        // Ensure that we have the color parameters set for the current material
        auto tech  = compositorInstance->getTechnique();
        auto tpass = tech->getOutputTargetPass();

        for( auto pass : tpass->getPassIterator() )
        {
            auto mtl = pass->getMaterial();

            if(!mtl.isNull())
            {
                this->setMaterialLightParams(mtl);
            }
        }

        m_compositorListeners.push_back(new RayTracingVolumeRenderer::ICCompositorListener(m_viewPointMatrices,
                                                                                           m_maskTexture,
                                                                                           m_entryPointsTextures[0],
                                                                                           m_sampleDistance));

        compositorInstance->addListener(m_compositorListeners.back());

        double nbPasses =
            std::nearbyint(std::max(std::log(m_camera->getViewport()->getActualWidth()) / std::log(2.0),
                                    std::log(m_camera->getViewport()->getActualHeight()) / std::log(2.0)));

        compositorInstance = compositorManager.addCompositor(viewport, "JFAInit");
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        m_compositorListeners.push_back(new RayTracingVolumeRenderer::JFACompositorListener(static_cast<float>(0),
                                                                                            static_cast<float>(nbPasses)));
        compositorInstance->addListener(m_compositorListeners.back());

        int i = 0;
        for(i = 0; i < nbPasses - 2; i++)
        {
            if(i % 2 == 0)
            {
                compositorInstance = compositorManager.addCompositor(viewport, "JFAPingComp");
                SLM_ASSERT("Compositor could not be initialized", compositorInstance);
                compositorInstance->setEnabled(true);
            }
            else
            {
                compositorInstance = compositorManager.addCompositor(viewport, "JFAPongComp");
                SLM_ASSERT("Compositor could not be initialized", compositorInstance);
                compositorInstance->setEnabled(true);
            }

            m_compositorListeners.push_back(new RayTracingVolumeRenderer::JFACompositorListener(static_cast<float>(i +
                                                                                                                   1),
                                                                                                static_cast<float>(
                                                                                                    nbPasses)));
            compositorInstance->addListener(m_compositorListeners.back());
        }

        if(i % 2 == 0)
        {
            compositorInstance = compositorManager.addCompositor(viewport, "JFAFinalPingComp");
            SLM_ASSERT("Compositor could not be initialized", compositorInstance);
            compositorInstance->setEnabled(true);
        }
        else
        {
            compositorInstance = compositorManager.addCompositor(viewport, "JFAFinalPongComp");
            SLM_ASSERT("Compositor could not be initialized", compositorInstance);
            compositorInstance->setEnabled(true);
        }

        m_compositorListeners.push_back(new RayTracingVolumeRenderer::JFACompositorListener(static_cast<float>(i),
                                                                                            static_cast<float>(nbPasses)));
        compositorInstance->addListener(m_compositorListeners.back());
    }
    else if(m_idvrMethod == "AImC")
    {
        idvrCompositorName = "IDVR_AImC_Comp";

        compositorInstance = compositorManager.addCompositor(viewport, idvrCompositorName);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        // Ensure that we have the color parameters set for the current material
        auto tech  = compositorInstance->getTechnique();
        auto tpass = tech->getOutputTargetPass();

        for( auto pass : tpass->getPassIterator() )
        {
            auto mtl = pass->getMaterial();

            if(!mtl.isNull())
            {
                this->setMaterialLightParams(mtl);
            }
        }

        m_compositorListeners.push_back(new RayTracingVolumeRenderer::ICCompositorListener(m_viewPointMatrices,
                                                                                           m_maskTexture,
                                                                                           m_entryPointsTextures[0],
                                                                                           m_sampleDistance));

        compositorInstance->addListener(m_compositorListeners.back());
    }
    else if(m_idvrMethod == "VPImC")
    {
        idvrCompositorName = "IDVR_VPImC_Comp";

        compositorInstance = compositorManager.addCompositor(viewport, idvrCompositorName);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        // Ensure that we have the color parameters set for the current material
        auto tech  = compositorInstance->getTechnique();
        auto tpass = tech->getOutputTargetPass();

        for( auto pass : tpass->getPassIterator() )
        {
            auto mtl = pass->getMaterial();

            if(!mtl.isNull())
            {
                this->setMaterialLightParams(mtl);
            }
        }

        m_compositorListeners.push_back(new RayTracingVolumeRenderer::ICCompositorListener(m_viewPointMatrices,
                                                                                           m_maskTexture,
                                                                                           m_entryPointsTextures[0],
                                                                                           m_sampleDistance));

        compositorInstance->addListener(m_compositorListeners.back());
    }

    this->addRayTracingCompositor();

    compositorInstance = compositorManager.addCompositor(viewport, "RTV_Comp");
    SLM_ASSERT("Compositor could not be initialized", compositorInstance);
    compositorInstance->setEnabled(true);

    m_compositorListeners.push_back(new RayTracingVolumeRenderer::RayTracingCompositorListener(m_viewPointMatrices,
                                                                                               (m_mode3D !=
                                                                                                ::fwRenderOgre::Layer::
                                                                                                StereoModeType::NONE),
                                                                                               m_volumeSceneNode));
    compositorInstance->addListener(m_compositorListeners.back());

    // Ensure that we have the color parameters set for the current material
    auto tech  = compositorInstance->getTechnique();
    auto tpass = tech->getOutputTargetPass();

    for( auto pass : tpass->getPassIterator() )
    {
        auto mtl = pass->getMaterial();

        if(!mtl.isNull())
        {
            this->setMaterialLightParams(mtl);
        }
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::cleanCompositorChain(::Ogre::CompositorChain* compChain)
{
    // Remove all associated listeners from the compositor chain
    for(size_t i(0); i < m_compositorListeners.size(); ++i)
    {
        ::Ogre::CompositorInstance::Listener* listener = m_compositorListeners[i];

        if(listener)
        {
            compChain->getCompositor(i)->removeListener(listener);
            delete listener;
        }
    }

    // Then clean the whole chain
    compChain->removeAllCompositors();
    m_compositorListeners.clear();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleIDVRCountersinkGeometry(bool CSG)
{
    m_idvrCSG = CSG;
    this->initCompositors();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRCountersinkSlope(double slope)
{
    m_idvrCSGSlope = static_cast<float>(slope);

    if(m_idvrMethod == "MImP")
    {
        m_RTVSharedParameters->setNamedConstant("u_countersinkSlope", m_idvrCSGSlope);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRAImCAlphaCorrection(double alphaCorrection)
{
    m_idvrAImCAlphaCorrection = static_cast<float>(alphaCorrection);

    if(m_idvrMethod == "AImC")
    {
        m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setIDVRVPImCAlphaCorrection(double alphaCorrection)
{
    m_idvrVPImCAlphaCorrection = static_cast<float>(alphaCorrection);

    if(m_idvrMethod == "VPImC")
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
