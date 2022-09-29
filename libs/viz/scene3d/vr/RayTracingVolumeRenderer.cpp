/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include <data/helper/MedicalImage.hpp>

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
#include <memory>
#include <string>
#include <utility>

namespace sight::viz::scene3d::vr
{

//-----------------------------------------------------------------------------

class RayTracingVolumeRenderer::CameraListener : public Ogre::Camera::Listener
{
/// Renderer to which this object listens
const RayTracingVolumeRenderer& m_renderer;

/// The renderer layer
std::weak_ptr<Layer> m_layer;

/// Material name used to retrieve the pass
std::string m_currentMtlName;

/// Frame ID.
int m_frameId {0};

public:

    /// Constructor
    CameraListener(const RayTracingVolumeRenderer& renderer, Layer::wptr layer) :
        m_renderer(renderer),
        m_layer(std::move(layer)),
        m_currentMtlName("VolIllum")
    {
    }

    //------------------------------------------------------------------------------

    void cameraPreRenderScene(Ogre::Camera* /*cam*/) override
    {
        if(auto layer = m_layer.lock())
        {
            const int frameId = layer->getRenderService()->getInteractorManager()->getFrameId();
            if(frameId != m_frameId)
            {
                if(m_renderer.m_shadows.parameters.soft_shadows)
                {
                    // Set light directions in shader.
                    const Ogre::LightList closestLights =
                        m_renderer.m_volumeSceneNode->getAttachedObject(0)->queryLights();

                    if(!closestLights.empty())
                    {
                        Ogre::Vector3 lightDir = m_renderer.m_volumeSceneNode->convertLocalToWorldDirection(
                            closestLights[0]->getDerivedDirection(),
                            true
                        );

                        const auto& material = Ogre::MaterialManager::getSingleton().getByName(
                            m_currentMtlName,
                            RESOURCE_GROUP
                        );
                        const Ogre::Pass* const satIllumPass = material->getTechnique(0)->getPass(0);

                        Ogre::GpuProgramParametersSharedPtr params = satIllumPass->getFragmentProgramParameters();

                        params->setNamedConstant("u_lightDir", lightDir);
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

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::RayTracingVolumeRenderer(
    std::string parentId,
    Layer::sptr layer,
    Ogre::SceneNode* const parentNode,
    sight::data::Image::csptr image,
    sight::data::TransferFunction::csptr tf,
    bool buffer,
    bool preintegration,
    std::optional<shadows_parameters_t> shadows,
    std::optional<IllumAmbientOcclusionSAT::sat_parameters_t> sat,
    std::optional<std::string> shader
) :
    IVolumeRenderer(parentId,
                    layer->getSceneManager(),
                    parentNode,
                    image,
                    tf,
                    buffer,
                    preintegration),
    m_shader(shader.value_or("RayTracedVolume_FP.glsl")),
    m_shadows({shadows.value_or(shadows_parameters_t {})}),
    m_layer(layer),
    m_sat(m_parentId,
          m_sceneManager,
          (m_shadows.parameters.ao.enabled || m_shadows.parameters.colour_bleeding.enabled),
          m_shadows.parameters.ao.enabled || m_shadows.parameters.colour_bleeding.enabled,
          sat.value_or(IllumAmbientOcclusionSAT::sat_parameters_t {}))
{
    //Listeners
    {
        auto* exitDepthListener = new compositor::listener::RayExitDepthListener();
        Ogre::MaterialManager::getSingleton().addListener(exitDepthListener);
    }

    //Compositor parameters
    const std::uint8_t numViewPoints  = m_layer.lock()->numCameras();
    const auto stereoMode             = layer->getStereoMode();
    const auto rayEntryCompositorName = "VolumeEntries" + std::to_string(numViewPoints);

    //Compositor for ray entry
    m_rayEntryCompositor = std::make_unique<RayEntryCompositor>(
        rayEntryCompositorName,
        defines::PROXY_GEOMETRY_RQ_GROUP,
        stereoMode,
        true
    );

    //Compositor manager
    {
        Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();
        auto* const viewport                       = layer->getViewport();
        auto* const compositorInstance             =
            compositorManager.addCompositor(viewport, rayEntryCompositorName, 0);
        SIGHT_ERROR_IF("Compositor '" + rayEntryCompositorName + "' not found.", compositorInstance == nullptr);
        compositorInstance->setEnabled(true);
    }

    //Shader parameters
    {
        const std::string rtvSharedParamsName = parentId + "_RTVParams";

        // First check that we did not already instance Shared parameters
        // This can happen when reinstancing this class (e.g. switching 3D mode)
        Ogre::GpuProgramManager::SharedParametersMap spMap =
            Ogre::GpuProgramManager::getSingleton().getAvailableSharedParameters();
        if(!spMap[rtvSharedParamsName])
        {
            m_RTVSharedParameters = Ogre::GpuProgramManager::getSingleton().createSharedParameters(rtvSharedParamsName);
        }
        else
        {
            m_RTVSharedParameters = spMap[rtvSharedParamsName];
        }

        // define the shared param structure
        m_RTVSharedParameters->addConstantDefinition("u_f3TFWindow", Ogre::GCT_FLOAT3);
        m_RTVSharedParameters->addConstantDefinition("u_fSampleDis_Ms", Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_f4VolIllumFactor", Ogre::GCT_FLOAT4);
        m_RTVSharedParameters->addConstantDefinition("u_f3VolumeClippingBoxMinPos_Ms", Ogre::GCT_FLOAT3);
        m_RTVSharedParameters->addConstantDefinition("u_f3VolumeClippingBoxMaxPos_Ms", Ogre::GCT_FLOAT3);
        m_RTVSharedParameters->addConstantDefinition("u_iMinImageValue", Ogre::GCT_INT1);
        m_RTVSharedParameters->addConstantDefinition("u_iMaxImageValue", Ogre::GCT_INT1);
        m_RTVSharedParameters->addConstantDefinition("u_fOpacityCorrectionFactor", Ogre::GCT_FLOAT1);
        m_RTVSharedParameters->addConstantDefinition("u_window", Ogre::GCT_FLOAT2);
        m_RTVSharedParameters->setNamedConstant("u_fOpacityCorrectionFactor", m_opacityCorrectionFactor);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::update(const data::TransferFunction::csptr& tf)
{
    initEntryPoints(); //Does nothing after the first call
    updateRayTracingMaterial();
    setSampling(m_nbSlices, tf);
}

//-----------------------------------------------------------------------------

RayTracingVolumeRenderer::~RayTracingVolumeRenderer()
{
    if(m_camera != nullptr)
    {
        m_camera->removeListener(m_cameraListener.get());
        m_cameraListener.reset();
    }

    //Notes:
    //- Cannot use smart pointers otherwise the destructor is called twice (which is bad)

    if(m_entryPointGeometry != nullptr)
    {
        m_volumeSceneNode->detachObject(m_entryPointGeometry);
        m_sceneManager->destroyManualObject(m_entryPointGeometry);
        m_entryPointGeometry = nullptr;
    }

    if(m_proxyGeometry != nullptr)
    {
        m_volumeSceneNode->detachObject(m_proxyGeometry);
        m_sceneManager->destroyMovableObject(m_proxyGeometry);
        m_proxyGeometry = nullptr;
    }

    Ogre::CompositorManager& compositorManager = Ogre::CompositorManager::getSingleton();
    auto* const viewport                       = m_layer.lock()->getViewport();

    const auto& rayEntryCompositorName = m_rayEntryCompositor->getName();
    compositorManager.setCompositorEnabled(viewport, rayEntryCompositorName, false);
    compositorManager.removeCompositor(viewport, rayEntryCompositorName);

    m_RTVSharedParameters->removeAllConstantDefinitions();

    // FIXME_DW: Doesn't seem to be a resource any longer
//    Ogre::GpuProgramManager::getSingleton().remove(m_RTVSharedParameters->getName(), RESOURCE_GROUP);

    Ogre::MaterialManager::getSingleton().remove(m_currentMtlName, RESOURCE_GROUP);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::imageUpdate(const data::Image::csptr image, const data::TransferFunction::csptr tf)
{
    if(!data::helper::MedicalImage::checkImageValidity(image))
    {
        return;
    }

    this->scaleTranslateCube(image->getSpacing(), image->getOrigin());

    const data::Image::Size& newSize = image->getSize();

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

    const auto material = Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName, RESOURCE_GROUP);
    if(m_preintegration)
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
        m_gpuVolumeTF->bind(pass, defines::VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
    }

    m_RTVSharedParameters->setNamedConstant("u_window", m_3DOgreTexture->window());

    // The depth technique always used the transfer function
    const auto* const technique = material->getTechnique(1);
    SIGHT_ASSERT("Technique not found", technique);
    const auto* const pass = technique->getPass(0);
    m_gpuVolumeTF->bind(pass, defines::VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateSATSizeRatio(unsigned ratio)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.updateSatFromRatio(static_cast<float>(ratio));
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateSATShellsNumber(unsigned count)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.setNbShells(count);
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateSATShellRadius(unsigned radius)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.setShellRadius(radius);
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateSATConeAngle(float angle)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.setConeAngle(angle);
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateSATConeSamples(unsigned samples)
{
    if(m_shadows.parameters.enabled())
    {
        m_sat.setSamplesAlongCone(samples);
        updateSAT();
    }
}

//------------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateSAT()
{
    m_sat.SATUpdate(m_3DOgreTexture, m_gpuVolumeTF, m_sampleDistance);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateVolumeTF(const data::TransferFunction::csptr& tf)
{
    //Update the attributes
    {
        m_gpuVolumeTF->update();
    }

    if(m_preintegration)
    {
        m_preIntegrationTable.tfUpdate(tf, m_sampleDistance);
    }

    const auto material = Ogre::MaterialManager::getSingleton().getByName(m_currentMtlName, RESOURCE_GROUP);

    if(!m_preintegration)
    {
        const auto* const technique = material->getTechnique(0);
        SIGHT_ASSERT("Technique not found", technique);
        const auto* const pass = technique->getPass(0);
        SIGHT_ASSERT("Pass not found", pass);
        m_gpuVolumeTF->bind(pass, defines::VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);
    }

    // The depth technique always uses the transfer function
    const auto* const technique = material->getTechnique(1);
    SIGHT_ASSERT("Technique not found", technique);

    const auto* const pass = technique->getPass(0);
    SIGHT_ASSERT("Pass not found", pass);
    m_gpuVolumeTF->bind(pass, defines::VOLUME_TF_TEXUNIT_NAME, m_RTVSharedParameters);

    m_proxyGeometry->computeGrid();

    const auto& shadows = m_shadows.parameters;

    if(shadows.ao.enabled || shadows.colour_bleeding.enabled || shadows.soft_shadows)
    {
        updateSAT();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setSampling(std::uint16_t nbSamples, const data::TransferFunction::csptr& tf)
{
    if(nbSamples == 0)
    {
        throw std::invalid_argument("Sample count cannot be 0.");
    }

    m_nbSlices = nbSamples;

    updateSampleDistance();

    // Update the sample distance in the shaders
    m_RTVSharedParameters->setNamedConstant("u_fSampleDis_Ms", m_sampleDistance);
    if(m_shadows.parameters.enabled())
    {
        updateSAT();
    }

    if(m_preintegration)
    {
        m_preIntegrationTable.tfUpdate(tf, m_sampleDistance);
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setOpacityCorrection(int opacityCorrection)
{
    m_opacityCorrectionFactor = static_cast<float>(opacityCorrection);

    // Update shader parameter
    m_RTVSharedParameters->setNamedConstant("u_fOpacityCorrectionFactor", m_opacityCorrectionFactor);
}

//-----------------------------------------------------------------------------

bool RayTracingVolumeRenderer::setAOFactor(float factor)
{
    m_shadows.parameters.ao.factor = factor;

    m_shadows.factors =
    {
        m_shadows.parameters.colour_bleeding.r,
        m_shadows.parameters.colour_bleeding.g,
        m_shadows.parameters.colour_bleeding.b,
        m_shadows.parameters.ao.factor
    };

    // Update the shader parameter
    m_RTVSharedParameters->setNamedConstant("u_f4VolIllumFactor", m_shadows.factors);

    return m_shadows.parameters.ao.enabled;
}

//-----------------------------------------------------------------------------

bool RayTracingVolumeRenderer::setColorBleedingFactor(float factor)
{
    return setColorBleedingFactor({factor, factor, factor});
}

//------------------------------------------------------------------------------

bool RayTracingVolumeRenderer::setColorBleedingFactor(const Ogre::Vector3& factors)
{
    //Update the parameters
    {
        m_shadows.parameters.colour_bleeding.r = factors.x;
        m_shadows.parameters.colour_bleeding.g = factors.x;
        m_shadows.parameters.colour_bleeding.b = factors.x;
    }

    m_shadows.factors =
    {
        m_shadows.parameters.colour_bleeding.r,
        m_shadows.parameters.colour_bleeding.g,
        m_shadows.parameters.colour_bleeding.b,
        m_shadows.parameters.ao.factor
    };

    // Update the shader parameter
    m_RTVSharedParameters->setNamedConstant("u_f4VolIllumFactor", m_shadows.factors);

    return m_shadows.parameters.colour_bleeding.enabled;
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleAmbientOcclusion(bool enable)
{
    if(enable)
    {
        enableAmbientOcclusion();
    }
    else
    {
        disableAmbientOcclusion();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::enableAmbientOcclusion()
{
    if(!m_shadows.parameters.ao.enabled)
    {
        m_shadows.parameters.ao.enabled = true;
        m_sat.setAO(true);

        this->updateSAT();
        this->updateRayTracingMaterial();
        this->updateVolumeIlluminationMaterial();
    }
}

//------------------------------------------------------------------------------

void RayTracingVolumeRenderer::disableAmbientOcclusion()
{
    if(m_shadows.parameters.ao.enabled) //Don't reallocate the material if it makes no sense
    {
        m_shadows.parameters.ao.enabled = false;
        m_sat.setAO(false);

        this->updateRayTracingMaterial();
        this->updateVolumeIlluminationMaterial();
        this->updateSAT();
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setPreIntegratedRendering(bool preIntegratedRendering)
{
    m_preintegration = preIntegratedRendering;

    this->updateRayTracingMaterial();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleColorBleeding(bool enable)
{
    m_shadows.parameters.colour_bleeding.enabled = enable;

    this->updateRayTracingMaterial();
    this->updateVolumeIlluminationMaterial();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::toggleShadows(bool enable)
{
    m_shadows.parameters.soft_shadows = enable;
    m_sat.setShadows(enable);

    this->updateSAT();
    this->updateRayTracingMaterial();
    this->updateVolumeIlluminationMaterial();
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::setFocalLength(float focalLength)
{
    m_focalLength = focalLength;
    computeRealFocalLength();
}

//------------------------------------------------------------------------------

void RayTracingVolumeRenderer::clipImage(const Ogre::AxisAlignedBox& clippingBox)
{
    const Ogre::AxisAlignedBox maxBoxSize(Ogre::Vector3::ZERO, Ogre::Vector3(1.F, 1.F, 1.F));
    const Ogre::AxisAlignedBox clampedClippingBox = maxBoxSize.intersection(clippingBox);

    IVolumeRenderer::clipImage(clampedClippingBox);

    if(m_entryPointGeometry == nullptr) //Prevent misusage
    {
        initEntryPoints();
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

void RayTracingVolumeRenderer::setRayCastingPassTextureUnits(Ogre::Pass* const _rayCastingPass) const
{
    //Fragment shaders parameters
    Ogre::GpuProgramParametersSharedPtr fpParams = _rayCastingPass->getFragmentProgramParameters();

    //The argument passes to setNamedConstant. Increased each call.
    int numTexUnit = 0;

    // Volume data
    {
        Ogre::TextureUnitState* const texUnitState = _rayCastingPass->createTextureUnitState();
        m_3DOgreTexture->bind(texUnitState, Ogre::TEX_TYPE_3D, Ogre::TFO_BILINEAR, Ogre::TextureUnitState::TAM_CLAMP);
    }

    fpParams->setNamedConstant("u_s3Image", numTexUnit++);

    // Transfer function
    if(m_options.fragment.find(defines::PREINTEGRATION) != std::string::npos)
    {
        Ogre::TextureUnitState* const texUnitState =
            _rayCastingPass->createTextureUnitState(m_preIntegrationTable.getTexture()->getName());
        texUnitState->setTextureFiltering(Ogre::TFO_BILINEAR);
        fpParams->setNamedConstant("u_s2PreintegratedTFTexture", numTexUnit++);
    }
    else
    {
        Ogre::TextureUnitState* const texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setName(defines::VOLUME_TF_TEXUNIT_NAME);
        texUnitState->setTextureFiltering(Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        m_gpuVolumeTF->bind(_rayCastingPass, texUnitState->getName(), fpParams);
        fpParams->setNamedConstant("u_s1TFTexture", numTexUnit++);
    }

    if(m_options.fragment.find(defines::AO) != std::string::npos) //i.e. ambient occlusion is enabled
    {
        Ogre::TextureUnitState* const texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setTextureFiltering(Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
        texUnitState->setTexture(m_sat.getIlluminationVolume());

        // Update the shader parameter
        fpParams->setNamedConstant("u_s3IlluminationVolume", numTexUnit++);
        m_RTVSharedParameters->setNamedConstant("u_f4VolIllumFactor", m_shadows.factors);
    }

    // Entry points texture
    Ogre::TextureUnitState* const texUnitState = _rayCastingPass->createTextureUnitState();
    texUnitState->setName("entryPoints");

    if(m_layer.lock()->numCameras() == 1)
    {
        const auto& rayEntryCompositorName = m_rayEntryCompositor->getName();
        texUnitState->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        texUnitState->setCompositorReference(rayEntryCompositorName, rayEntryCompositorName + "Texture");
    }

    texUnitState->setTextureFiltering(Ogre::TFO_NONE);
    texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    fpParams->setNamedConstant("u_s2EntryPoints", numTexUnit++);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateRayTracingMaterial()
{
    //Update the current options
    updateOptions();

    m_currentMtlName = "RTV_Mat_" + m_parentId;

    Ogre::MaterialManager& materialManager = Ogre::MaterialManager::getSingleton();

    // The material needs to be destroyed only if it already exists
    {
        const Ogre::ResourcePtr matResource = materialManager.getResourceByName(m_currentMtlName, RESOURCE_GROUP);
        if(matResource)
        {
            materialManager.remove(matResource);
            // Our manual object still references the material and uses the material name to know if it should modify
            // its pointer (see ManualObject::ManualObjectSection::setMaterialName() in OgreManualObject.cpp)
            // So we just force it to release the pointer otherwise the material is not destroyed
            if(m_entryPointGeometry != nullptr)
            {
                m_entryPointGeometry->setMaterialName(0, "");
            }
        }
    }

    Ogre::HighLevelGpuProgramManager& gpm = Ogre::HighLevelGpuProgramManager::getSingleton();
    const Ogre::String vpName("RTV_VP_" + std::to_string(m_options.hash));
    const Ogre::String fpName("RTV_FP_" + std::to_string(m_options.hash));

    // Compile the common vertex shader
    {
        if(!gpm.resourceExists(vpName, RESOURCE_GROUP))
        {
            Ogre::HighLevelGpuProgramPtr vsp = gpm.createProgram(
                vpName,
                RESOURCE_GROUP,
                "glsl",
                Ogre::GPT_VERTEX_PROGRAM
            );
            vsp->setSourceFile("RayTracedVolume_VP.glsl");

            if(!m_options.vertex.empty())
            {
                vsp->setParameter("preprocessor_defines", m_options.vertex);
            }
        }
    }

    // Compile fragment shader
    {
        if(!gpm.resourceExists(fpName, RESOURCE_GROUP))
        {
            Ogre::HighLevelGpuProgramPtr fsp =
                gpm.createProgram(fpName, RESOURCE_GROUP, "glsl", Ogre::GPT_FRAGMENT_PROGRAM);
            fsp->setSourceFile(m_shader);

            if(!m_options.fragment.empty())
            {
                fsp->setParameter("preprocessor_defines", m_options.fragment);
            }
        }
    }

    // Create the material
    const Ogre::MaterialPtr mat = materialManager.create(m_currentMtlName, viz::scene3d::RESOURCE_GROUP);

    // Create the technique
    {
        // Ensure that we have the color parameters set for the current material
        this->setMaterialLightParams(mat);
        // Get the already created pass through the already created technique
        const Ogre::Technique* const tech = mat->getTechnique(0);

        Ogre::Pass* const pass = tech->getPass(0);
        pass->setCullingMode(Ogre::CULL_ANTICLOCKWISE);
        pass->setSeparateSceneBlendingOperation(Ogre::SBO_ADD, Ogre::SBO_ADD);
        pass->setSeparateSceneBlending(
            Ogre::SBF_SOURCE_ALPHA,
            Ogre::SBF_ONE_MINUS_SOURCE_ALPHA,
            Ogre::SBF_ONE,
            Ogre::SBF_ONE_MINUS_SOURCE_ALPHA
        );
        pass->setDepthCheckEnabled(true);
        pass->setDepthWriteEnabled(true);

        // Vertex program
        pass->setVertexProgram(vpName);
        Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
        vpParams->setNamedAutoConstant("u_worldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

        // Fragment program
        pass->setFragmentProgram(fpName);
        Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
        fpParams->setNamedAutoConstant("u_viewportSize", Ogre::GpuProgramParameters::ACT_VIEWPORT_SIZE);
        fpParams->setNamedAutoConstant("u_clippingNearDis", Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
        fpParams->setNamedAutoConstant("u_clippingFarDis", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
        fpParams->setNamedAutoConstant("u_f3CameraPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
        fpParams->setNamedAutoConstant("u_fShininess", Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
        fpParams->setNamedAutoConstant("u_fNumLights", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
        fpParams->setNamedAutoConstant(
            "u_f4LightPos",
            Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY,
            10
        );
        fpParams->setNamedAutoConstant(
            "u_f3LightDiffuseCol",
            Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_ARRAY,
            10
        );
        fpParams->setNamedAutoConstant(
            "u_f3LightSpecularCol",
            Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR_ARRAY,
            10
        );
        fpParams->setNamedAutoConstant(
            "u_invWorldViewProj",
            Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEWPROJ_MATRIX
        );
        fpParams->setNamedAutoConstant("u_worldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
        fpParams->addSharedParameters(m_RTVSharedParameters->getName());

        // Setup texture unit states
        this->setRayCastingPassTextureUnits(pass);
    }

    // Compile the depth fragment shader
    const Ogre::String fpDepthName("RTVD_FP_" + std::to_string(m_options.hash));
    if(!gpm.resourceExists(fpDepthName, RESOURCE_GROUP))
    {
        Ogre::HighLevelGpuProgramPtr fsp = gpm.createProgram(
            fpDepthName,
            RESOURCE_GROUP,
            "glsl",
            Ogre::GPT_FRAGMENT_PROGRAM
        );
        fsp->setSourceFile("RayTracedVolumeDepth_FP.glsl");
        fsp->setParameter("attach", "DepthPeelingCommon_FP");

        if(!m_options.fragment.empty())
        {
            fsp->setParameter("preprocessor_defines", m_options.fragment);
        }
    }

    // Create the depth technique
    {
        using program_parameters_t = Ogre::GpuProgramParameters;

        Ogre::Technique* const tech = mat->createTechnique();

        //Technique scheme name
        {
            tech->setSchemeName("DepthPeeling/depthMap");
        }

        Ogre::Pass* const pass = tech->createPass();

        //Culling and depth
        {
            pass->setCullingMode(Ogre::CULL_ANTICLOCKWISE);
            pass->setDepthCheckEnabled(true);
            pass->setDepthWriteEnabled(true);
        }

        // Vertex program
        {
            pass->setVertexProgram(vpName);
            Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
            vpParams->setNamedAutoConstant("u_worldViewProj", program_parameters_t::ACT_WORLDVIEWPROJ_MATRIX);
        }

        // Fragment program
        {
            pass->setFragmentProgram(fpDepthName);
            Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
            fpParams->setNamedAutoConstant("u_viewportSize", program_parameters_t::ACT_VIEWPORT_SIZE);
            fpParams->setNamedAutoConstant("u_clippingNearDis", program_parameters_t::ACT_NEAR_CLIP_DISTANCE);
            fpParams->setNamedAutoConstant("u_clippingFarDis", program_parameters_t::ACT_FAR_CLIP_DISTANCE);
            fpParams->setNamedAutoConstant(
                "u_invWorldViewProj",
                program_parameters_t::ACT_INVERSE_WORLDVIEWPROJ_MATRIX
            );
            fpParams->setNamedAutoConstant("u_worldViewProj", program_parameters_t::ACT_WORLDVIEWPROJ_MATRIX);
            fpParams->addSharedParameters(m_RTVSharedParameters->getName());
        }

        //Texture unit state updates
        {
            Ogre::GpuProgramParametersSharedPtr fpParams = pass->getFragmentProgramParameters();
            //Input texture
            {
                Ogre::TextureUnitState* const texUnitState = pass->createTextureUnitState();
                SIGHT_ASSERT("texUnitState is nullptr.", texUnitState != nullptr);

                m_3DOgreTexture->bind(
                    texUnitState,
                    Ogre::TEX_TYPE_3D,
                    Ogre::TFO_BILINEAR,
                    Ogre::TextureUnitState::TAM_CLAMP
                );
                fpParams->setNamedConstant("u_s3Image", 0);
            }

            //Transfer function and compositor
            {
                //GPU TF binding
                {
                    Ogre::TextureUnitState* const texUnitState = pass->createTextureUnitState();
                    texUnitState->setName(defines::VOLUME_TF_TEXUNIT_NAME);
                    texUnitState->setTextureFiltering(Ogre::TFO_BILINEAR);
                    texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                    m_gpuVolumeTF->bind(pass, texUnitState->getName(), fpParams);
                    fpParams->setNamedConstant("u_s1TFTexture", 1);
                }

                //Ray compositor
                {
                    Ogre::TextureUnitState* const texUnitState = pass->createTextureUnitState();
                    texUnitState->setName("entryPoints");

                    if(m_layer.lock()->numCameras() == 1)
                    {
                        const auto& rayEntryCompositorName = m_rayEntryCompositor->getName();
                        texUnitState->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                        texUnitState->setCompositorReference(
                            rayEntryCompositorName,
                            rayEntryCompositorName + "Texture"
                        );
                    }

                    texUnitState->setTextureFiltering(Ogre::TFO_NONE);
                    texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
                    fpParams->setNamedConstant("u_s2EntryPoints", 2);
                }
            }
        }
    }

    //3D texture to render
    {
        Ogre::MaterialManager& mm         = Ogre::MaterialManager::getSingleton();
        Ogre::MaterialPtr currentMaterial = mm.getByName(m_currentMtlName, RESOURCE_GROUP);
        SIGHT_ASSERT("Missing material '" + m_currentMtlName + "'.", currentMaterial);
        const Ogre::Technique* const tech = currentMaterial->getTechnique(0);
        SIGHT_ASSERT("Material '" + m_currentMtlName + "' has no techniques.", tech);
        Ogre::Pass* const pass = tech->getPass(0);
        SIGHT_ASSERT("Material '" + m_currentMtlName + "' has no passes.", pass);

        Ogre::TextureUnitState* const texUnitState = pass->getTextureUnitState(0);
        SIGHT_ASSERT("Material '" + m_currentMtlName + "' has no texture units.", texUnitState);
        texUnitState->setTextureName(m_3DOgreTexture->name(), Ogre::TEX_TYPE_3D);

        m_proxyGeometry->set3DImageTexture(m_3DOgreTexture);
    }

    if(static_cast<bool>(m_entryPointGeometry))
    {
        m_entryPointGeometry->setMaterialName(0, m_currentMtlName, RESOURCE_GROUP);
    }
}

//------------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateOptions()
{
    std::ostringstream vpPPDefs;
    std::ostringstream fpPPDefs;

    vpPPDefs.str("");
    fpPPDefs.str("");

    if(m_shadows.parameters.ao.enabled)
    {
        fpPPDefs << (fpPPDefs.str().empty() ? "" : ",") << defines::AO;
    }

    if(m_shadows.parameters.colour_bleeding.enabled)
    {
        fpPPDefs << (fpPPDefs.str().empty() ? "" : ",") << defines::COLOR_BLEEDING;
    }

    if(m_shadows.parameters.soft_shadows)
    {
        fpPPDefs << (fpPPDefs.str().empty() ? "" : ",") << defines::SHADOWS;
    }

    if(m_preintegration)
    {
        fpPPDefs << (fpPPDefs.str().empty() ? "" : ",") << defines::PREINTEGRATION;
    }

    const std::string vertex   = vpPPDefs.str();
    const std::string fragment = fpPPDefs.str();

    m_options = options_t(vertex, fragment);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::initEntryPoints()
{
    //Entry point geometry
    if(m_entryPointGeometry == nullptr)
    {
        m_entryPointGeometry = m_sceneManager->createManualObject(m_parentId + "_RayTracingVREntryPoints");

        {
            // Use the default material before the raytracing material is created otherwise we get an error.
            m_entryPointGeometry->begin("Default", Ogre::RenderOperation::OT_TRIANGLE_LIST, RESOURCE_GROUP);
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

            m_entryPointGeometry->end();
        }

        // Render volumes after surfaces.
        m_entryPointGeometry->setRenderQueueGroup(rq::s_VOLUME_ID);

        m_volumeSceneNode->attachObject(m_entryPointGeometry);
    }

    //Proxy geometry
    if(m_proxyGeometry == nullptr)
    {
        m_proxyGeometry =
            viz::scene3d::vr::GridProxyGeometry::New(
                this->m_parentId + "_GridProxyGeometry",
                m_sceneManager,
                m_3DOgreTexture,
                m_gpuVolumeTF,
                "RayEntryPoints"
            );

        m_proxyGeometry->setRenderQueueGroup(defines::PROXY_GEOMETRY_RQ_GROUP);

        m_volumeSceneNode->attachObject(m_proxyGeometry);
    }

    //Camera listener
    if(m_cameraListener == nullptr)
    {
        m_cameraListener = std::make_unique<CameraListener>(*this, m_layer);
        m_camera->addListener(m_cameraListener.get());
    }
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::computeRealFocalLength()
{
    const Ogre::Plane cameraPlane(m_camera->getRealDirection(), m_camera->getRealPosition());
    const auto cameraDistComparator = [&cameraPlane](const Ogre::Vector3& v1, const Ogre::Vector3& v2)
                                      {return cameraPlane.getDistance(v1) < cameraPlane.getDistance(v2);};

    const auto closestFurthestImgPoints =
        std::minmax_element(m_clippedImagePositions.begin(), m_clippedImagePositions.end(), cameraDistComparator);

    const auto focusPoint = *closestFurthestImgPoints.first + m_focalLength
                            * (*closestFurthestImgPoints.second - *closestFurthestImgPoints.first);

    const float realFocalLength = m_camera->getRealPosition().distance(focusPoint);

    m_camera->setFocalLength(realFocalLength);
}

//-----------------------------------------------------------------------------

void RayTracingVolumeRenderer::updateVolumeIlluminationMaterial()
{
    std::string volIllumMtl = "VolIllum";

    volIllumMtl += m_shadows.parameters.ao.enabled || m_shadows.parameters.colour_bleeding.enabled ? "_AO" : "";
    volIllumMtl += m_shadows.parameters.soft_shadows ? "_Shadows" : "";

    SIGHT_ASSERT("Camera listener not instantiated", m_cameraListener);
    m_cameraListener->setCurrentMtlName(volIllumMtl);
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
