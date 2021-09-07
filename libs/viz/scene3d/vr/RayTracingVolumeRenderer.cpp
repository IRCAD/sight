/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "viz/scene3d/vr/RayTracingVolumeRenderer.hpp"

#include "viz/scene3d/compositor/Core.hpp"
#include "viz/scene3d/compositor/listener/RayExitDepth.hpp"
#include "viz/scene3d/helper/Camera.hpp"
#include "viz/scene3d/helper/Shading.hpp"
#include "viz/scene3d/ogre.hpp"
#include "viz/scene3d/SRender.hpp"
#include "viz/scene3d/Utils.hpp"

#include <core/Profiling.hpp>

#include <data/fieldHelper/MedicalImageHelpers.hpp>

#include <OGRE/OgreCompositorInstance.h>
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

namespace sight::viz::scene3d
{

namespace vr
{

//-----------------------------------------------------------------------------

struct RayTracingVolumeRenderer::CameraListener : public ::Ogre::Camera::Listener
{
    RayTracingVolumeRenderer* const m_renderer;
    std::string m_currentMtlName;
    int m_frameId;

    CameraListener(RayTracingVolumeRenderer* const renderer) :
        m_renderer(renderer),
        m_currentMtlName("VolIllum"),
        m_frameId(0)
    {
    }

    //------------------------------------------------------------------------------

    virtual void cameraPreRenderScene(::Ogre::Camera*)
    {
        const auto layer = m_renderer->getLayer();
        if(layer)
        {
            const int frameId = layer->getRenderService()->getInteractorManager()->getFrameId();
            if(frameId != m_frameId)
            {
                auto ambientOcclusionSAT = m_renderer->m_ambientOcclusionSAT.lock();
                if(ambientOcclusionSAT && m_renderer->m_shadows)
                {
                    // Set light directions in shader.
                    const ::Ogre::LightList closestLights =
                        m_renderer->m_volumeSceneNode->getAttachedObject(0)->queryLights();

                    if(!closestLights.empty())
                    {
                        ::Ogre::Vector3 lightDir = m_renderer->m_volumeSceneNode->convertLocalToWorldDirection(
                            closestLights[0]->getDerivedDirection(),
                            true
                        );

                        const ::Ogre::Pass* const satIllumPass = ::Ogre::MaterialManager::getSingleton().getByName(
                            m_currentMtlName,
                            RESOURCE_GROUP
                        )->getTechnique(0)->getPass(0);
                        ::Ogre::GpuProgramParametersSharedPtr satIllumParams =
                            satIllumPass->getFragmentProgramParameters();

                        satIllumParams->setNamedConstant("u_lightDir", lightDir);

                        ambientOcclusionSAT->updateVolIllum();
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

RayTracingVolumeRenderer::RayTracingVolumeRenderer(
    std::string parentId,
    Layer::sptr layer,
    ::Ogre::SceneNode* const parentNode,
    ::Ogre::TexturePtr imageTexture,
    const TransferFunction::sptr& gpuVolumeTF,
    PreIntegrationTable& preintegrationTable,
    bool ambientOcclusion,
    bool colorBleeding,
    bool shadows,
    double aoFactor,
    double colorBleedingFactor
) :
    IVolumeRenderer(parentId, layer->getSceneManager(), parentNode, imageTexture, preintegrationTable),
    m_entryPointGeometry(nullptr),
    m_proxyGeometry(nullptr),
    m_imageSize(data::Image::Size({1, 1, 1})),
    m_ambientOcclusion(ambientOcclusion),
    m_colorBleeding(colorBleeding),
    m_shadows(shadows),
    m_gpuVolumeTF(gpuVolumeTF),
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
    auto* const viewport                         = layer->getViewport();

    const std::uint8_t numViewPoints  = this->getLayer()->getNumberOfCameras();
    const auto stereoMode             = layer->getStereoMode();
    const auto rayEntryCompositorName = "VolumeEntries" + std::to_string(numViewPoints);
    m_rayEntryCompositor = std::make_unique<RayEntryCompositor>(
        rayEntryCompositorName,
        s_PROXY_GEOMETRY_RQ_GROUP,
        stereoMode,
        true
    );

    auto* const compositorInstance = compositorManager.addCompositor(viewport, rayEntryCompositorName, 0);
    SIGHT_ERROR_IF("Compositor '" + rayEntryCompositorName + "' not found.", compositorInstance == nullptr);
    compositorInstance->setEnabled(true);

    const std::string rtvSharedParamsName = parentId + "_RTVParams";

    // First check that we did not already instance Shared parameters
    // This can happen when reinstancing this class (e.g. switching 3D mode)
    ::Ogre::GpuProgramManager::SharedParametersMap spMap =
        ::Ogre::GpuProgramManager::getSingleton().getAvailableSharedParameters();
    if(!spMap[rtvSharedParamsName])
    {
        m_RTVSharedParameters = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters(rtvSharedParamsName);
    }
    else
    {
        m_RTVSharedParameters = spMap[rtvSharedParamsName];
    }

    // define the shared param structure
    m_RTVSharedParameters->addConstantDefinition("u_f2TFWindow", ::Ogre::GCT_FLOAT2);
    m_RTVSharedParameters->addConstantDefinition("u_fSampleDis_Ms", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_f4VolIllumFactor", ::Ogre::GCT_FLOAT4);
    m_RTVSharedParameters->addConstantDefinition("u_f3VolumeClippingBoxMinPos_Ms", ::Ogre::GCT_FLOAT3);
    m_RTVSharedParameters->addConstantDefinition("u_f3VolumeClippingBoxMaxPos_Ms", ::Ogre::GCT_FLOAT3);
    m_RTVSharedParameters->addConstantDefinition("u_iMinImageValue", ::Ogre::GCT_INT1);
    m_RTVSharedParameters->addConstantDefinition("u_iMaxImageValue", ::Ogre::GCT_INT1);
    m_RTVSharedParameters->addConstantDefinition("u_fOpacityCorrectionFactor", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->setNamedConstant("u_fOpacityCorrectionFactor", m_opacityCorrectionFactor);

    this->initEntryPoints();
    this->createRayTracingMaterial();

    this->setSampling(m_nbSlices);
}

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::~RayTracingVolumeRenderer()
{
    m_camera->removeListener(m_cameraListener);
    delete m_cameraListener;
    m_cameraListener = nullptr;

    m_volumeSceneNode->detachObject(m_entryPointGeometry);
    m_volumeSceneNode->detachObject(m_proxyGeometry);

    m_sceneManager->destroyManualObject(m_entryPointGeometry);
    m_sceneManager->destroyMovableObject(m_proxyGeometry);

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto* const viewport                         = this->getLayer()->getViewport();

    const auto& rayEntryCompositorName = m_rayEntryCompositor->getName();
    compositorManager.setCompositorEnabled(viewport, rayEntryCompositorName, false);
    compositorManager.removeCompositor(viewport, rayEntryCompositorName);

    m_RTVSharedParameters->removeAllConstantDefinitions();

    // FIXME_DW: Doesn't seem to be a resource any longer
    //::Ogre::GpuProgramManager::getSingleton().remove(m_RTVSharedParameters->getName(), RESOURCE_GROUP);

    ::Ogre::MaterialManager::getSingleton().remove(m_currentMtlName, RESOURCE_GROUP);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::imageUpdate(const data::Image::sptr image, const data::TransferFunction::sptr tf)
{
    if(!data::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        return;
    }

    this->scaleTranslateCube(image->getSpacing2(), image->getOrigin2());

    const data::Image::Size& newSize = image->getSize2();

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

    const auto material = ::Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName, RESOURCE_GROUP);
    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.imageUpdate(image, tf, m_sampleDistance);

        // After having updated the preintegrated transfer function
        // We update the corresponding shader parameters
        const auto minMax = m_preIntegrationTable.getMinMax();

        m_RTVSharedParameters->setNamedConstant("u_iMinImageValue", minMax.first);
        m_RTVSharedParameters->setNamedConstant("u_iMaxImageValue", minMax.second);
    }
    else
    {
        const auto* const technique = material->getTechnique(0);
        SIGHT_ASSERT("Technique not found", technique);
        const auto* const pass = technique->getPass(0);
        SIGHT_ASSERT("Pass not found", pass);
        m_gpuVolumeTF.lock()->bind(pass, s_VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
    }

    // The depth technique always used the transfer function
    const auto* const technique = material->getTechnique(1);
    SIGHT_ASSERT("Technique not found", technique);
    const auto* const pass = technique->getPass(0);
    m_gpuVolumeTF.lock()->bind(pass, s_VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::set3DTexture(const ::Ogre::TexturePtr& _texture)
{
    if(m_3DOgreTexture != _texture)
    {
        m_3DOgreTexture = _texture;

        ::Ogre::MaterialManager& mm = ::Ogre::MaterialManager::getSingleton();
        ::Ogre::MaterialPtr mat     = mm.getByName(m_currentMtlName, RESOURCE_GROUP);
        SIGHT_ASSERT("Missing material '" + m_currentMtlName + "'.", mat);
        const ::Ogre::Technique* const tech = mat->getTechnique(0);
        SIGHT_ASSERT("Material '" + m_currentMtlName + "' has no techniques.", tech);
        ::Ogre::Pass* const pass = tech->getPass(0);
        SIGHT_ASSERT("Material '" + m_currentMtlName + "' has no passes.", pass);

        ::Ogre::TextureUnitState* const texUnitState = pass->getTextureUnitState(0);
        SIGHT_ASSERT("Material '" + m_currentMtlName + "' has no texture units.", texUnitState);
        texUnitState->setTextureName(m_3DOgreTexture->getName(), ::Ogre::TEX_TYPE_3D);

        m_proxyGeometry->set3DImageTexture(m_3DOgreTexture);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateVolumeTF()
{
    const auto material = ::Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName, RESOURCE_GROUP);

    if(!m_preIntegratedRendering)
    {
        const auto* const technique = material->getTechnique(0);
        SIGHT_ASSERT("Technique not found", technique);
        const auto* const pass = technique->getPass(0);
        SIGHT_ASSERT("Pass not found", pass);
        m_gpuVolumeTF.lock()->bind(pass, s_VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
    }

    // The depth technique always used the transfer function
    const auto* const technique = material->getTechnique(1);
    SIGHT_ASSERT("Technique not found", technique);
    const auto* const pass = technique->getPass(0);
    SIGHT_ASSERT("Pass not found", pass);
    m_gpuVolumeTF.lock()->bind(pass, s_VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);

    m_proxyGeometry->computeGrid();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setSampling(uint16_t nbSamples)
{
    m_nbSlices = nbSamples;

    computeSampleDistance(getCameraPlane());

    // Update the sample distance in the shaders
    m_RTVSharedParameters->setNamedConstant("u_fSampleDis_Ms", m_sampleDistance);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setOpacityCorrection(int opacityCorrection)
{
    m_opacityCorrectionFactor = static_cast<float>(opacityCorrection);

    // Update shader parameter
    m_RTVSharedParameters->setNamedConstant("u_fOpacityCorrectionFactor", m_opacityCorrectionFactor);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setAOFactor(double aoFactor)
{
    m_volIllumFactor.w = static_cast< ::Ogre::Real>(aoFactor);

    // Update the shader parameter
    m_RTVSharedParameters->setNamedConstant("u_f4VolIllumFactor", m_volIllumFactor);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setColorBleedingFactor(double colorBleedingFactor)
{
    ::Ogre::Real cbFactor = static_cast< ::Ogre::Real>(colorBleedingFactor);
    m_volIllumFactor = ::Ogre::Vector4(cbFactor, cbFactor, cbFactor, m_volIllumFactor.w);

    // Update the shader parameter
    m_RTVSharedParameters->setNamedConstant("u_f4VolIllumFactor", m_volIllumFactor);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setAmbientOcclusionSAT(IllumAmbientOcclusionSAT::sptr _ambientOcclusionSAT)
{
    m_ambientOcclusionSAT = _ambientOcclusionSAT;

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
    const ::Ogre::AxisAlignedBox maxBoxSize(::Ogre::Vector3::ZERO, ::Ogre::Vector3(1.f, 1.f, 1.f));
    const ::Ogre::AxisAlignedBox clampedClippingBox = maxBoxSize.intersection(clippingBox);

    IVolumeRenderer::clipImage(clampedClippingBox);

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

    m_proxyGeometry->clipGrid(clampedClippingBox);

    m_RTVSharedParameters->setNamedConstant("u_f3VolumeClippingBoxMinPos_Ms", clampedClippingBox.getMinimum());
    m_RTVSharedParameters->setNamedConstant("u_f3VolumeClippingBoxMaxPos_Ms", clampedClippingBox.getMaximum());
}

//-----------------------------------------------------------------------------

bool RayTracingVolumeRenderer::isVisible() const
{
    return m_entryPointGeometry->isVisible() && m_proxyGeometry->isVisible();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setRayCastingPassTextureUnits(
    Ogre::Pass* const _rayCastingPass,
    const std::string& _fpPPDefines
) const
{
    ::Ogre::GpuProgramParametersSharedPtr fpParams = _rayCastingPass->getFragmentProgramParameters();

    int numTexUnit = 0;
    ::Ogre::TextureUnitState* texUnitState;
    // Volume data
    texUnitState = _rayCastingPass->createTextureUnitState();
    texUnitState->setTextureName(m_3DOgreTexture->getName(), ::Ogre::TEX_TYPE_3D);
    texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
    texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

    fpParams->setNamedConstant("u_s3Image", numTexUnit++);

    // Transfer function
    if(_fpPPDefines.find(s_PREINTEGRATION_DEFINE) != std::string::npos)
    {
        texUnitState = _rayCastingPass->createTextureUnitState(m_preIntegrationTable.getTexture()->getName());
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        fpParams->setNamedConstant("u_s2PreintegratedTFTexture", numTexUnit++);
    }
    else
    {
        const auto gpuTF = m_gpuVolumeTF.lock();
        texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setName(s_VOLUME_TF_TEXUNIT_NAME);
        gpuTF->bind(_rayCastingPass, texUnitState->getName(), fpParams);
        fpParams->setNamedConstant("u_s1TFTexture", numTexUnit++);
    }

    if(_fpPPDefines.find(s_AO_DEFINE) != std::string::npos)
    {
        texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        texUnitState->setTexture(m_ambientOcclusionSAT.lock()->getIlluminationVolume());

        fpParams->setNamedConstant("u_s3IlluminationVolume", numTexUnit++);
        // Update the shader parameter
        m_RTVSharedParameters->setNamedConstant("u_f4VolIllumFactor", m_volIllumFactor);
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

    fpParams->setNamedConstant("u_s2EntryPoints", numTexUnit++);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::createRayTracingMaterial(const std::string& _sourceFile)
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;
    std::tie(vpPPDefines, fpPPDefines, hash) = this->computeRayTracingDefines();

    const ::Ogre::String matName("RTV_Mat_" + m_parentId);
    m_currentMtlName = matName;

    ::Ogre::MaterialManager& mm = ::Ogre::MaterialManager::getSingleton();

    // The material needs to be destroyed only if it already exists
    {
        const ::Ogre::ResourcePtr matResource = mm.getResourceByName(matName, RESOURCE_GROUP);
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

    // Compile the commun vertex shader
    ::Ogre::HighLevelGpuProgramManager& gpm = ::Ogre::HighLevelGpuProgramManager::getSingleton();
    const ::Ogre::String vpName("RTV_VP_" + std::to_string(hash));
    if(!gpm.resourceExists(vpName, RESOURCE_GROUP))
    {
        ::Ogre::HighLevelGpuProgramPtr vsp = gpm.createProgram(
            vpName,
            RESOURCE_GROUP,
            "glsl",
            ::Ogre::GPT_VERTEX_PROGRAM
        );
        vsp->setSourceFile("RayTracedVolume_VP.glsl");

        if(vpPPDefines.size() > 0)
        {
            vsp->setParameter("preprocessor_defines", vpPPDefines);
        }
    }

    // Compile fragment shader
    ::Ogre::String fpName("RTV_FP_" + std::to_string(hash));
    if(!gpm.resourceExists(fpName, RESOURCE_GROUP))
    {
        ::Ogre::HighLevelGpuProgramPtr fsp =
            gpm.createProgram(fpName, RESOURCE_GROUP, "glsl", ::Ogre::GPT_FRAGMENT_PROGRAM);
        fsp->setSourceFile(_sourceFile);

        if(fpPPDefines.size() > 0)
        {
            fsp->setParameter("preprocessor_defines", fpPPDefines);
        }
    }

    // Create the material
    ::Ogre::MaterialPtr mat = mm.create(m_currentMtlName, viz::scene3d::RESOURCE_GROUP);

    // Create the technique
    {
        // Ensure that we have the color parameters set for the current material
        this->setMaterialLightParams(mat);
        // Get the already created pass through the already created technique
        const ::Ogre::Technique* const tech = mat->getTechnique(0);

        ::Ogre::Pass* const pass = tech->getPass(0);
        pass->setCullingMode(::Ogre::CULL_ANTICLOCKWISE);
        pass->setSeparateSceneBlendingOperation(::Ogre::SBO_ADD, ::Ogre::SBO_ADD);
        pass->setSeparateSceneBlending(
            ::Ogre::SBF_SOURCE_ALPHA,
            ::Ogre::SBF_ONE_MINUS_SOURCE_ALPHA,
            ::Ogre::SBF_ONE,
            ::Ogre::SBF_ONE_MINUS_SOURCE_ALPHA
        );
        pass->setDepthCheckEnabled(true);
        pass->setDepthWriteEnabled(true);

        // Vertex program
        pass->setVertexProgram(vpName);
        ::Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
        vpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

        // Fragment program
        pass->setFragmentProgram(fpName);
        ::Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
        fpParams->setNamedAutoConstant("u_viewportSize", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
        fpParams->setNamedAutoConstant("u_clippingNearDis", ::Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
        fpParams->setNamedAutoConstant("u_clippingFarDis", ::Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
        fpParams->setNamedAutoConstant("u_f3CameraPos", ::Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
        fpParams->setNamedAutoConstant("u_fShininess", ::Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
        fpParams->setNamedAutoConstant("u_fNumLights", ::Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
        fpParams->setNamedAutoConstant(
            "u_f4LightPos",
            ::Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY,
            10
        );
        fpParams->setNamedAutoConstant(
            "u_f3LightDiffuseCol",
            ::Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_ARRAY,
            10
        );
        fpParams->setNamedAutoConstant(
            "u_f3LightSpecularCol",
            ::Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR_ARRAY,
            10
        );
        fpParams->setNamedAutoConstant(
            "u_invWorldViewProj",
            ::Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEWPROJ_MATRIX
        );
        fpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
        fpParams->addSharedParameters(m_RTVSharedParameters->getName());

        // Setup texture unit states
        this->setRayCastingPassTextureUnits(pass, fpPPDefines);
    }

    // Compile the depth fragment shader
    const ::Ogre::String fpDepthName("RTVD_FP_" + std::to_string(hash));
    if(!gpm.resourceExists(fpDepthName, RESOURCE_GROUP))
    {
        ::Ogre::HighLevelGpuProgramPtr fsp = gpm.createProgram(
            fpDepthName,
            RESOURCE_GROUP,
            "glsl",
            ::Ogre::GPT_FRAGMENT_PROGRAM
        );
        fsp->setSourceFile("RayTracedVolumeDepth_FP.glsl");
        fsp->setParameter("attach", "DepthPeelingCommon_FP");

        if(fpPPDefines.size() > 0)
        {
            fsp->setParameter("preprocessor_defines", fpPPDefines);
        }
    }

    // Create the depth technique
    {
        ::Ogre::Technique* const tech = mat->createTechnique();
        tech->setSchemeName("DepthPeeling/depthMap");

        ::Ogre::Pass* const pass = tech->createPass();
        pass->setCullingMode(::Ogre::CULL_ANTICLOCKWISE);
        pass->setDepthCheckEnabled(true);
        pass->setDepthWriteEnabled(true);

        // Vertex program
        pass->setVertexProgram(vpName);
        ::Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
        vpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

        // Fragment program
        pass->setFragmentProgram(fpDepthName);
        ::Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
        fpParams->setNamedAutoConstant("u_viewportSize", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
        fpParams->setNamedAutoConstant("u_clippingNearDis", ::Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
        fpParams->setNamedAutoConstant("u_clippingFarDis", ::Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
        fpParams->setNamedAutoConstant(
            "u_invWorldViewProj",
            ::Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEWPROJ_MATRIX
        );
        fpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
        fpParams->addSharedParameters(m_RTVSharedParameters->getName());

        // Setup texture unit states
        ::Ogre::TextureUnitState* texUnitState;
        texUnitState = pass->createTextureUnitState();
        texUnitState->setTextureName(m_3DOgreTexture->getName(), ::Ogre::TEX_TYPE_3D);
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        fpParams->setNamedConstant("u_s3Image", 0);

        const auto gpuTF = m_gpuVolumeTF.lock();
        texUnitState = pass->createTextureUnitState();
        texUnitState->setName(s_VOLUME_TF_TEXUNIT_NAME);
        gpuTF->bind(pass, texUnitState->getName(), fpParams);
        fpParams->setNamedConstant("u_s1TFTexture", 1);

        texUnitState = pass->createTextureUnitState();
        texUnitState->setName("entryPoints");
        if(this->getLayer()->getNumberOfCameras() == 1)
        {
            const auto& rayEntryCompositorName = m_rayEntryCompositor->getName();
            texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
            texUnitState->setCompositorReference(rayEntryCompositorName, rayEntryCompositorName + "Texture");
        }

        texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        fpParams->setNamedConstant("u_s2EntryPoints", 2);
    }

    m_entryPointGeometry->setMaterialName(0, m_currentMtlName, RESOURCE_GROUP);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::initEntryPoints()
{
    m_entryPointGeometry = m_sceneManager->createManualObject(m_parentId + "_RayTracingVREntryPoints");

    // Use the default material before the raytracing material is created otherwise we get an error.
    m_entryPointGeometry->begin("Default", ::Ogre::RenderOperation::OT_TRIANGLE_LIST, RESOURCE_GROUP);
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

    m_proxyGeometry = viz::scene3d::vr::GridProxyGeometry::New(
        this->m_parentId + "_GridProxyGeometry",
        m_sceneManager,
        m_3DOgreTexture,
        m_gpuVolumeTF.lock(),
        "RayEntryPoints"
    );

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
                                      {return cameraPlane.getDistance(v1) < cameraPlane.getDistance(v2);};

    const auto closestFurthestImgPoints =
        std::minmax_element(m_clippedImagePositions, m_clippedImagePositions + 8, cameraDistComparator);

    const auto focusPoint = *closestFurthestImgPoints.first + m_focalLength
                            * (*closestFurthestImgPoints.second - *closestFurthestImgPoints.first);

    const float realFocalLength = m_camera->getRealPosition().distance(focusPoint);

    m_camera->setFocalLength(realFocalLength);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateVolIllumMat()
{
    std::string volIllumMtl = "VolIllum";

    volIllumMtl += m_ambientOcclusion || m_colorBleeding ? "_AO" : "";
    volIllumMtl += m_shadows ? "_Shadows" : "";

    SIGHT_ASSERT("Camera listener not instantiated", m_cameraListener);
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

    return std::make_tuple(vpPPDefs.str(), fpPPDefs.str(), std::hash<std::string> {}(vpPPDefs.str() + fpPPDefs.str()));
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setMaterialLightParams(::Ogre::MaterialPtr mtl)
{
    ::Ogre::ColourValue diffuse(1.2f, 1.2f, 1.2f, 1.f);
    mtl->setDiffuse(diffuse);

    ::Ogre::ColourValue specular(2.5f, 2.5f, 2.5f, 1.f);
    mtl->setSpecular(specular);
    mtl->setShininess(20);
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d
