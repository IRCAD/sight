/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SVolumeRender.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>
#include <fwRenderOgre/vr/RayTracingVolumeRenderer.hpp>
#include <fwRenderOgre/vr/SliceVolumeRenderer.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>

// For now deactivate the specific optimizations for autostereo that causes
// blending problem with the background
#define ENABLE_AUTO_STEREO_OPTIM

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVolumeRender, ::fwData::Image);

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEW_IMAGE_SLOT                    = "newImage";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEW_MASK_SLOT                     = "newMask";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAMPLING_SLOT              = "updateSampling";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_AO_FACTOR_SLOT             = "updateAOFactor";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_COLOR_BLEEDING_FACTOR_SLOT = "updateColorBleedingFactor";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SIZE_RATIO_SLOT        = "updateSatSizeRatio";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SHELLS_SLOT            = "updateSatShellsNumber";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SHELL_RADIUS_SLOT      = "updateSatShellRadius";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_CONE_ANGLE_SLOT        = "updateSatConeAngle";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_CONE_SAMPLES_SLOT      = "updateSatConeSamplesNumber";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_PREINTEGRATION_SLOT        = "togglePreintegration";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_AMBIENT_OCCLUSION_SLOT     = "toggleAmbientOcclusion";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_COLOR_BLEEDING_SLOT        = "toggleColorBleeding";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_SHADOWS_SLOT               = "toggleShadows";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_WIDGETS_SLOT               = "toggleWidgets";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_RESIZE_VIEWPORT_SLOT              = "resizeViewport";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_FOCAL_DISTANCE_SLOT           = "setFocalDistance";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_MODE3D_SLOT                   = "setStereoMode";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_BOOL_PARAMETER_SLOT           = "setBoolParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_INT_PARAMETER_SLOT            = "setIntParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_DOUBLE_PARAMETER_SLOT         = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_ENUM_PARAMETER_SLOT           = "setEnumParameter";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() throw() :
    m_volumeRenderer(nullptr),
    m_sceneManager(nullptr),
    m_volumeSceneNode(nullptr),
    m_camera(nullptr),
    m_nbSlices(512),
    m_preIntegratedRendering(false),
    m_ambientOcclusion(false),
    m_colorBleeding(false),
    m_shadows(false),
    m_widgetVisibilty(true),
    m_illum(nullptr),
    m_satSizeRatio(0.25f),
    m_satShells(4),
    m_satShellRadius(4),
    m_satConeAngle(0.1f),
    m_satConeSamples(50),
    m_aoFactor(1.f),
    m_colorBleedingFactor(1.f),
    m_autoResetCamera(true)
{
    this->installTFSlots(this);
    newSlot(s_NEW_IMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_NEW_MASK_SLOT, &SVolumeRender::newMask, this);
    newSlot(s_UPDATE_SAMPLING_SLOT, &SVolumeRender::updateSampling, this);
    newSlot(s_UPDATE_AO_FACTOR_SLOT, &SVolumeRender::updateAOFactor, this);
    newSlot(s_UPDATE_COLOR_BLEEDING_FACTOR_SLOT, &SVolumeRender::updateColorBleedingFactor, this);
    newSlot(s_UPDATE_SAT_SIZE_RATIO_SLOT, &SVolumeRender::updateSatSizeRatio, this);
    newSlot(s_UPDATE_SAT_SHELLS_SLOT, &SVolumeRender::updateSatShellsNumber, this);
    newSlot(s_UPDATE_SAT_SHELL_RADIUS_SLOT, &SVolumeRender::updateSatShellRadius, this);
    newSlot(s_UPDATE_SAT_CONE_ANGLE_SLOT, &SVolumeRender::updateSatConeAngle, this);
    newSlot(s_UPDATE_SAT_CONE_SAMPLES_SLOT, &SVolumeRender::updateSatConeSamples, this);
    newSlot(s_TOGGLE_PREINTEGRATION_SLOT, &SVolumeRender::togglePreintegration, this);
    newSlot(s_TOGGLE_AMBIENT_OCCLUSION_SLOT, &SVolumeRender::toggleAmbientOcclusion, this);
    newSlot(s_TOGGLE_COLOR_BLEEDING_SLOT, &SVolumeRender::toggleColorBleeding, this);
    newSlot(s_TOGGLE_SHADOWS_SLOT, &SVolumeRender::toggleShadows, this);
    newSlot(s_TOGGLE_WIDGETS_SLOT, &SVolumeRender::toggleWidgets, this);
    newSlot(s_RESIZE_VIEWPORT_SLOT, &SVolumeRender::resizeViewport, this);
    newSlot(s_SET_FOCAL_DISTANCE_SLOT, &SVolumeRender::setFocalDistance, this);
    newSlot(s_SET_MODE3D_SLOT, &SVolumeRender::setStereoMode, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SVolumeRender::setBoolParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &SVolumeRender::setIntParameter, this);
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SVolumeRender::setDoubleParameter, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SVolumeRender::setEnumParameter, this);

    m_ogreTransform = ::Ogre::Matrix4::IDENTITY;
    m_renderingMode = VR_MODE_RAY_TRACING;
}

//-----------------------------------------------------------------------------

SVolumeRender::~SVolumeRender() throw()
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::doConfigure() throw ( ::fwTools::Failed )
{
    SLM_ASSERT("No config tag", m_configuration->getName() == "config");

    if(m_configuration->hasAttribute("autoresetcamera"))
    {
        std::string autoResetCamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoResetCamera == "yes");
    }

    if(m_configuration->hasAttribute("preintegration"))
    {
        m_preIntegratedRendering = (m_configuration->getAttributeValue("preintegration") == "yes");
    }

    if(m_configuration->hasAttribute("mode"))
    {
        if(m_configuration->getAttributeValue("mode") == "slice")
        {
            m_renderingMode = VR_MODE_SLICE;
        }
        if(m_configuration->getAttributeValue("mode") == "raytracing")
        {
            m_renderingMode = VR_MODE_RAY_TRACING;

            if(m_configuration->hasAttribute("satSizeRatio"))
            {
                std::string sizeRatioString = m_configuration->getAttributeValue("satSizeRatio");
                m_satSizeRatio = std::stof(sizeRatioString);
            }

            if(m_configuration->hasAttribute("satShells"))
            {
                std::string shellsString = m_configuration->getAttributeValue("satShells");
                m_satShells = std::stoi(shellsString);
            }

            if(m_configuration->hasAttribute("satShellRadius"))
            {
                std::string shellRadiusString = m_configuration->getAttributeValue("satShellRadius");
                m_satShellRadius = std::stoi(shellRadiusString);
            }

            if(m_configuration->hasAttribute("satConeAngle"))
            {
                std::string coneAngleString = m_configuration->getAttributeValue("satConeAngle");
                m_satConeAngle = std::stof(coneAngleString);
            }

            if(m_configuration->hasAttribute("satConeSamples"))
            {
                std::string coneSamplesString = m_configuration->getAttributeValue("satConeSamples");
                m_satConeSamples = std::stoi(coneSamplesString);
            }

            if(m_configuration->hasAttribute("aoFactor"))
            {
                std::string aoFactorString = m_configuration->getAttributeValue("aoFactor");
                m_aoFactor = std::stod(aoFactorString);
            }

            if(m_configuration->hasAttribute("colorBleedingFactor"))
            {
                std::string colorBleedingFactorString = m_configuration->getAttributeValue("colorBleedingFactor");
                m_colorBleedingFactor = std::stod(colorBleedingFactorString);
            }

            if(m_configuration->hasAttribute("ao"))
            {
                m_ambientOcclusion = (m_configuration->getAttributeValue("ao") == "yes");
            }

            if(m_configuration->hasAttribute("colorBleeding"))
            {
                m_colorBleeding = (m_configuration->getAttributeValue("colorBleeding") == "yes");
            }

            if(m_configuration->hasAttribute("shadows"))
            {
                m_shadows = (m_configuration->getAttributeValue("shadows") == "yes");
            }
        }
        else
        {
            OSLM_WARN("Unknown VR mode, defaults to ray tracing.");
        }
    }

    if(m_configuration->hasAttribute("widgets"))
    {
        m_widgetVisibilty = (m_configuration->getAttributeValue("widgets") == "yes");
    }

    this->parseTFConfig(m_configuration);
}

//-----------------------------------------------------------------------------

void SVolumeRender::updatingTFPoints()
{
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    this->updateTransferFunction(this->getImage());

    m_gpuTF.updateTexture(tf);

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_volumeRenderer->getSamplingRate());
    }

    m_volumeRenderer->tfUpdate(tf);

    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->updateVolumeIllumination();
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updatingTFWindowing(double window, double level)
{
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    m_gpuTF.updateTexture(tf);

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_volumeRenderer->getSamplingRate());
    }

    m_volumeRenderer->tfUpdate(tf);

    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->updateVolumeIllumination();
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVolumeRender::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;

    connections.push( "image", ::fwData::Image::s_MODIFIED_SIG, s_NEW_IMAGE_SLOT );
    connections.push( "image", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEW_IMAGE_SLOT );
    connections.push( "mask", ::fwData::Image::s_MODIFIED_SIG, s_NEW_MASK_SLOT );
    connections.push( "mask", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEW_MASK_SLOT );
    connections.push( "clippingMatrix", ::fwData::Image::s_MODIFIED_SIG, s_NEW_IMAGE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

void SVolumeRender::doStart() throw ( ::fwTools::Failed )
{
    ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite>("TF");
    this->setTransferFunctionSelection(tfSelection);
    this->setTFSelectionFwID(tfSelection->getID());

    this->updateImageInfos(this->getObject< ::fwData::Image >());
    this->updateTransferFunction(this->getImage());

    this->installTFConnections();

    m_sceneManager    = this->getSceneManager();
    m_volumeSceneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    m_camera          = this->getLayer()->getDefaultCamera();

    // Create textures
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    m_maskTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_MaskTexture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    m_gpuTF.createTexture(this->getID());
    m_preIntegrationTable.createTexture(this->getID());

    ::fwRenderOgre::Layer::sptr layer = this->getRenderService()->getLayer(m_layerID);

    if(m_renderingMode == VR_MODE_SLICE)
    {
        m_volumeRenderer = new ::fwRenderOgre::vr::SliceVolumeRenderer(this->getID(),
                                                                       m_sceneManager,
                                                                       m_volumeSceneNode,
                                                                       m_3DOgreTexture,
                                                                       m_gpuTF,
                                                                       m_preIntegrationTable);
    }
    else
    {
#ifdef ENABLE_AUTO_STEREO_OPTIM
        const auto stereoMode = layer->getStereoMode();
#else
        const auto stereoMode = ::fwRenderOgre::Layer::StereoModeType::NONE;
#endif

        m_volumeRenderer = new ::fwRenderOgre::vr::RayTracingVolumeRenderer(this->getID(),
                                                                            layer,
                                                                            m_volumeSceneNode,
                                                                            m_3DOgreTexture,
                                                                            m_maskTexture,
                                                                            m_gpuTF,
                                                                            m_preIntegrationTable,
                                                                            stereoMode,
                                                                            m_ambientOcclusion,
                                                                            m_colorBleeding);

        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer*>(m_volumeRenderer);

        OSLM_ERROR_IF("Stereo rendering is supported only by ray casting VR.", !rayCastVolumeRenderer);

        // Initially focus on the image center.
        setFocalDistance(50);
    }

    m_gpuTF.setSampleDistance(m_volumeRenderer->getSamplingRate());

    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_illum = new ::fwRenderOgre::vr::SATVolumeIllumination(this->getID(), m_sceneManager, m_satSizeRatio,
                                                                (m_ambientOcclusion || m_colorBleeding), m_shadows,
                                                                m_satShells, m_satShellRadius, m_satConeAngle,
                                                                m_satConeSamples);
    }

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    m_volumeConnection.connect(layer, ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_RESIZE_VIEWPORT_SLOT);
#ifdef ENABLE_AUTO_STEREO_OPTIM
    m_volumeConnection.connect(layer, ::fwRenderOgre::Layer::s_MODE3D_CHANGED_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_SET_MODE3D_SLOT);
#endif

    this->initWidgets();
    m_widgets->setVisibility(m_widgetVisibilty);

    bool isValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(this->getImage());
    if (isValid)
    {
        this->newImage();
    }
    else
    {
        m_volumeSceneNode->setVisible(false, false);
    }

    if (m_autoResetCamera && !this->getImage()->getField("cameraTransform"))
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
    m_volumeRenderer->tfUpdate(this->getTransferFunction());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::doStop() throw ( ::fwTools::Failed )
{
    this->removeTFConnections();

    m_volumeConnection.disconnect();
    delete m_volumeRenderer;

    m_sceneManager->getRootSceneNode()->removeChild(m_volumeSceneNode->getName());

    ::Ogre::TextureManager::getSingleton().remove(m_3DOgreTexture->getHandle());
    m_3DOgreTexture.setNull();

    ::Ogre::TextureManager::getSingleton().remove(m_maskTexture->getHandle());
    m_maskTexture.setNull();

    m_gpuTF.removeTexture();
    m_preIntegrationTable.removeTexture();

    // Disconnect widget to interactor.
    {
        ::fwRenderOgre::Layer::sptr layer                        = this->getRenderService()->getLayer(m_layerID);
        ::fwRenderOgre::interactor::IInteractor::sptr interactor = layer->getInteractor();

        auto vrInteractor =
            std::dynamic_pointer_cast< ::fwRenderOgre::interactor::VRWidgetsInteractor >(interactor);

        if(vrInteractor)
        {
            vrInteractor->detachWidget(m_widgets);
        }
        m_widgets = nullptr;
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::doUpdate() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::doSwap() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::newImage()
{
    this->updateImageInfos(this->getObject< ::fwData::Image >());

    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getImage();

    // Retrieves or creates the slice index fields
    this->updateImageInfos(image);

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    this->updateTransferFunction(this->getImage());

    m_gpuTF.updateTexture(tf);

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_volumeRenderer->getSamplingRate());
    }

    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->updateVolumeIllumination();
    }

    m_volumeRenderer->imageUpdate(image, this->getTransferFunction());

    m_volumeSceneNode->setVisible(true, m_widgets->getVisibility());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::newMask()
{
    ::fwData::Image::sptr mask = this->getInOut< ::fwData::Image>("mask");
    ::fwRenderOgre::Utils::convertImageForNegato(m_maskTexture.get(), mask);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSampling(int nbSamples)
{
    OSLM_ASSERT("Sampling rate must fit in a 16 bit uint.", nbSamples < 65536 && nbSamples >= 0);
    m_nbSlices = static_cast<uint16_t>(nbSamples);

    m_volumeRenderer->setSampling(m_nbSlices);
    m_gpuTF.setSampleDistance(m_volumeRenderer->getSamplingRate());

    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        this->updateVolumeIllumination();
    }

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_volumeRenderer->getSamplingRate());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateOpacityCorrection(int opacityCorrection)
{
    auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);
    OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);

    rayCastVolumeRenderer->setOpacityCorrection(opacityCorrection);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateAOFactor(double aoFactor)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_aoFactor = aoFactor;

        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);

        rayCastVolumeRenderer->setAOFactor(m_aoFactor);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateColorBleedingFactor(double colorBleedingFactor)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_colorBleedingFactor = colorBleedingFactor;

        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);

        rayCastVolumeRenderer->setColorBleedingFactor(m_colorBleedingFactor);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatSizeRatio(int sizeRatio)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        float scaleCoef(.25f);
        m_satSizeRatio = static_cast<float>(sizeRatio) * scaleCoef;
        m_illum->updateSatFromRatio(m_satSizeRatio);

        this->updateVolumeIllumination();

        if(m_preIntegratedRendering)
        {
            m_volumeRenderer->imageUpdate(this->getImage(), this->getTransferFunction());
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatShellsNumber(int shellsNumber)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_satShells = shellsNumber;

        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);

        auto illumVolume = rayCastVolumeRenderer->getIllumVolume();
        illumVolume->setNbShells(m_satShells);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatShellRadius(int shellRadius)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_satShellRadius = shellRadius;

        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);

        auto illumVolume = rayCastVolumeRenderer->getIllumVolume();
        illumVolume->setShellRadius(m_satShellRadius);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatConeAngle(int coneAngle)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_satConeAngle = static_cast<float>(coneAngle) / 100;

        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);

        auto illumVolume = rayCastVolumeRenderer->getIllumVolume();
        illumVolume->setConeAngle(m_satConeAngle);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatConeSamples(int nbConeSamples)
{
    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        m_satConeSamples = nbConeSamples;

        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);

        auto illumVolume = rayCastVolumeRenderer->getIllumVolume();
        illumVolume->setSamplesAlongCone(m_satConeSamples);
        illumVolume->updateVolIllum();

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::togglePreintegration(bool preintegration)
{
    m_preIntegratedRendering = preintegration;

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    if(m_preIntegratedRendering)
    {
        m_volumeRenderer->imageUpdate(this->getImage(), this->getTransferFunction());
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_volumeRenderer->getSamplingRate());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleAmbientOcclusion(bool ambientOcclusion)
{
    m_ambientOcclusion = ambientOcclusion;
    this->toggleVREffect(::visuOgreAdaptor::SVolumeRender::VR_AMBIENT_OCCLUSION);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleColorBleeding(bool colorBleeding)
{
    m_colorBleeding = colorBleeding;
    this->toggleVREffect(::visuOgreAdaptor::SVolumeRender::VR_COLOR_BLEEDING);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleShadows(bool shadows)
{
    m_shadows = shadows;
    this->toggleVREffect(::visuOgreAdaptor::SVolumeRender::VR_SHADOWS);
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleWidgets(bool visible)
{
    m_widgetVisibilty = visible;

    m_widgets->setVisibility(m_widgetVisibilty);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::resizeViewport(int w, int h)
{
    if(m_volumeRenderer)
    {
        m_volumeRenderer->resizeViewport(w, h);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setFocalDistance(int focalDistance)
{
    if(this->getRenderService()->getLayer(m_layerID)->getStereoMode() != ::fwRenderOgre::Layer::StereoModeType::NONE)
    {
        auto rayTracingRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer*>(m_volumeRenderer);

        if(rayTracingRenderer)
        {
            rayTracingRenderer->setFocalLength(static_cast<float>(focalDistance) / 100);

            this->requestRender();
        }
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setStereoMode(::fwRenderOgre::Layer::StereoModeType mode)
{
    this->doStop();
    this->doStart();
}

//-----------------------------------------------------------------------------

void SVolumeRender::setBoolParameter(bool val, std::string key)
{
    if(key == "preIntegration")
    {
        this->togglePreintegration(val);
    }
    else if(key == "ambientOcclusion")
    {
        this->toggleAmbientOcclusion(val);
    }
    else if(key == "colorBleeding")
    {
        this->toggleColorBleeding(val);
    }
    else if(key == "shadows")
    {
        this->toggleShadows(val);
    }
    else if(key == "widgets")
    {
        this->toggleWidgets(val);
    }
    else if(key == "idvrCSG")
    {
        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->toggleIDVRCountersinkGeometry(val);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setIntParameter(int val, std::string key)
{
    if(key == "sampling")
    {
        this->updateSampling(val);
    }
    else if(key == "opacityCorrection")
    {
        this->updateOpacityCorrection(val);
    }
    else if(key == "satSizeRatio")
    {
        this->updateSatSizeRatio(val);
    }
    else if(key == "satShellsNumber")
    {
        this->updateSatShellsNumber(val);
    }
    else if(key == "satShellRadius")
    {
        this->updateSatShellRadius(val);
    }
    else if(key == "satConeAngle")
    {
        this->updateSatConeAngle(val);
    }
    else if(key == "satConeSamples")
    {
        this->updateSatConeSamples(val);
    }
    else if(key == "focalLength")
    {
        this->setFocalDistance(val);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setDoubleParameter(double val, std::string key)
{
    if(key == "aoFactor")
    {
        this->updateAOFactor(val);
    }
    else if(key == "colorBleedingFactor")
    {
        this->updateColorBleedingFactor(val);
    }
    else if(key == "idvrCSGSlope")
    {
        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRCountersinkSlope(val);
    }
    else if(key == "idvrVPImCAlphaCorrection")
    {
        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRVPImCAlphaCorrection(val);
    }
    else if(key == "idvrAImCAlphaCorrection")
    {
        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRAImCAlphaCorrection(val);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setEnumParameter(std::string val, std::string key)
{
    if(key == "idvrMethod")
    {
        auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRMethod(val);
        this->requestRender();
    }
    else if(key == "3DMode")
    {
        if(val == "None")
        {
            ::fwRenderOgre::Layer::sptr layer = this->getRenderService()->getLayer(m_layerID);
            layer->setStereoMode(::fwRenderOgre::Layer::StereoModeType::NONE);
        }
        else if(val == "Autostereo(5)")
        {
            ::fwRenderOgre::Layer::sptr layer = this->getRenderService()->getLayer(m_layerID);
            layer->setStereoMode(::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5);
        }
        else if(val == "Autostereo(8)")
        {
            ::fwRenderOgre::Layer::sptr layer = this->getRenderService()->getLayer(m_layerID);
            layer->setStereoMode(::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8);
        }
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::initWidgets()
{
    // Create widgets.
    {
        auto clippingMatrix = this->getInOut< ::fwData::TransformationMatrix3D>("clippingMatrix");

        m_widgets = ::std::make_shared< ::fwRenderOgre::ui::VRWidget >(this->getID(), m_volumeSceneNode,
                                                                       m_camera, this->getRenderService(),
                                                                       m_sceneManager, m_volumeRenderer,
                                                                       clippingMatrix);
    }

    // Connect widgets to interactor.
    {
        ::fwRenderOgre::Layer::sptr layer                        = this->getRenderService()->getLayer(m_layerID);
        ::fwRenderOgre::interactor::IInteractor::sptr interactor = layer->getInteractor();

        auto vrInteractor = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::VRWidgetsInteractor >(interactor);

        if(vrInteractor)
        {
            vrInteractor->initPicker();
            vrInteractor->attachWidget(m_widgets);
        }
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateVolumeIllumination()
{
    m_illum->SATUpdate(m_3DOgreTexture, m_gpuTF.getTexture(), m_volumeRenderer->getSamplingRate());

    // Volume illumination is only implemented for raycasting rendering
    if(m_renderingMode == VR_MODE_RAY_TRACING)
    {
        ::fwRenderOgre::vr::RayTracingVolumeRenderer* rayTracingVolumeRenderer =
            static_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

        rayTracingVolumeRenderer->setIlluminationVolume(m_illum);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleVREffect(::visuOgreAdaptor::SVolumeRender::VREffectType vrEffect)
{
    auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

    // Volume illumination is only implemented for raycasting rendering
    if(rayCastVolumeRenderer)
    {
        if((m_ambientOcclusion || m_colorBleeding || m_shadows) && !m_illum)
        {
            m_illum = new ::fwRenderOgre::vr::SATVolumeIllumination(this->getID(), m_sceneManager, m_satSizeRatio,
                                                                    (m_ambientOcclusion || m_colorBleeding), m_shadows,
                                                                    m_satShells, m_satShellRadius);
            this->updateVolumeIllumination();
        }
        else
        {
            switch(vrEffect)
            {
                case ::visuOgreAdaptor::SVolumeRender::VR_AMBIENT_OCCLUSION:
                case ::visuOgreAdaptor::SVolumeRender::VR_COLOR_BLEEDING:
                    m_illum->setAO(m_ambientOcclusion || m_colorBleeding);
                    break;
                case ::visuOgreAdaptor::SVolumeRender::VR_SHADOWS:
                    m_illum->setShadows(m_shadows);
                    break;
            }

        }

        switch(vrEffect)
        {
            case ::visuOgreAdaptor::SVolumeRender::VR_AMBIENT_OCCLUSION:
                rayCastVolumeRenderer->setAmbientOcclusion(m_ambientOcclusion);
                break;
            case ::visuOgreAdaptor::SVolumeRender::VR_COLOR_BLEEDING:
                rayCastVolumeRenderer->setColorBleeding(m_colorBleeding);
                break;
            case ::visuOgreAdaptor::SVolumeRender::VR_SHADOWS:
                rayCastVolumeRenderer->setShadows(m_shadows);
                break;
        }

        this->updateSampling(m_nbSlices);
        this->updateSatSizeRatio(static_cast<int>(m_satSizeRatio * 4));
        this->updateSatShellsNumber(m_satShells);
        this->updateSatShellRadius(m_satShellRadius);
        this->updateSatConeAngle(static_cast<int>(m_satConeAngle * 100));
        this->updateSatConeSamples(m_satConeSamples);

        if(m_preIntegratedRendering)
        {
            m_volumeRenderer->imageUpdate(this->getImage(), this->getTransferFunction());
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

}
