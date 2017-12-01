/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SVolumeRender.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>
#include <fwRenderOgre/vr/ImportanceDrivenVolumeRenderer.hpp>
#include <fwRenderOgre/vr/RayTracingVolumeRenderer.hpp>
#include <fwRenderOgre/vr/SliceVolumeRenderer.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVolumeRender, ::fwData::Image);

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEW_IMAGE_SLOT                    = "newImage";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEW_MASK_SLOT                     = "newMask";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_IMAGE_SLOT                 = "updateImage";
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
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_STEREO_MODE_SLOT              = "setStereoMode";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_BOOL_PARAMETER_SLOT           = "setBoolParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_INT_PARAMETER_SLOT            = "setIntParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_DOUBLE_PARAMETER_SLOT         = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_ENUM_PARAMETER_SLOT           = "setEnumParameter";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_COLOR_PARAMETER_SLOT          = "setColorParameter";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT           = "image";
static const ::fwServices::IService::KeyType s_TF_INOUT              = "tf";
static const ::fwServices::IService::KeyType s_CLIPPING_MATRIX_INOUT = "clippingMatrix";
static const ::fwServices::IService::KeyType s_MASK_INOUT            = "mask";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() noexcept :
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
    m_aoFactor(1.),
    m_colorBleedingFactor(1.),
    m_autoResetCamera(true)
{
    this->installTFSlots(this);
    newSlot(s_NEW_IMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_NEW_MASK_SLOT, &SVolumeRender::newMask, this);
    newSlot(s_UPDATE_IMAGE_SLOT, &SVolumeRender::updateImage, this);
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
    newSlot(s_SET_STEREO_MODE_SLOT, &SVolumeRender::setStereoMode, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SVolumeRender::setBoolParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &SVolumeRender::setIntParameter, this);
    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &SVolumeRender::setDoubleParameter, this);
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SVolumeRender::setEnumParameter, this);
    newSlot(s_SET_COLOR_PARAMETER_SLOT, &SVolumeRender::setColorParameter, this);

    m_ogreTransform = ::Ogre::Matrix4::IDENTITY;
    m_renderingMode = VR_MODE_RAY_TRACING;
}

//-----------------------------------------------------------------------------

SVolumeRender::~SVolumeRender() noexcept
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if(config.count("autoresetcamera"))
    {
        m_autoResetCamera = config.get<std::string>("autoresetcamera") == "yes";
    }

    if(config.count("preintegration"))
    {
        m_preIntegratedRendering = (config.get<std::string>("preintegration") == "yes");
    }

    if(config.count("mode"))
    {
        if(config.get<std::string>("mode") == "slice")
        {
            m_renderingMode = VR_MODE_SLICE;
        }
        if(config.get<std::string>("mode") == "raytracing")
        {
            m_renderingMode = VR_MODE_RAY_TRACING;

            if(config.count("satSizeRatio"))
            {
                m_satSizeRatio = config.get<float>("satSizeRatio");
            }

            if(config.count("satShells"))
            {
                m_satShells = config.get<int>("satShells");
            }

            if(config.count("satShellRadius"))
            {
                m_satShellRadius = config.get<int>("satShellRadius");
            }

            if(config.count("satConeAngle"))
            {
                m_satConeAngle = config.get<float>("satConeAngle");
            }

            if(config.count("satConeSamples"))
            {
                m_satConeSamples = config.get<int>("satConeSamples");
            }

            if(config.count("aoFactor"))
            {
                m_aoFactor = config.get<double>("aoFactor");
            }

            if(config.count("colorBleedingFactor"))
            {
                m_colorBleedingFactor = config.get<double>("colorBleedingFactor");
            }

            if(config.count("ao"))
            {
                m_ambientOcclusion = (config.get<std::string>("ao") == "yes");
            }

            if(config.count("colorBleeding"))
            {
                m_colorBleeding = (config.get<std::string>("colorBleeding") == "yes");
            }

            if(config.count("shadows"))
            {
                m_shadows = (config.get<std::string>("shadows") == "yes");
            }
        }
        else
        {
            OSLM_WARN("Unknown VR mode, defaults to ray tracing.");
        }
    }

    if(config.count("widgets"))
    {
        m_widgetVisibilty = (config.get<std::string>("widgets") == "yes");
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateTFPoints()
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

void SVolumeRender::updateTFWindowing(double /*window*/, double /*level*/)
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

    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEW_IMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT );
    connections.push( s_MASK_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEW_MASK_SLOT );
    connections.push( s_MASK_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEW_MASK_SLOT );
    connections.push( s_CLIPPING_MATRIX_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEW_IMAGE_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

void SVolumeRender::starting()
{
    this->initialize();
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT +"' is missing.", image);

    ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    this->setOrCreateTF(tf, image);

    this->updateImageInfos(image);

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
        m_volumeRenderer = new ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer(this->getID(),
                                                                                  layer,
                                                                                  m_volumeSceneNode,
                                                                                  m_3DOgreTexture,
                                                                                  m_maskTexture,
                                                                                  m_gpuTF,
                                                                                  m_preIntegrationTable,
                                                                                  m_ambientOcclusion,
                                                                                  m_colorBleeding);

        // Initially focus on the image center.
        setFocalDistance(50);
    }

    m_gpuTF.setSampleDistance(m_volumeRenderer->getSamplingRate());

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    m_volumeConnection.connect(layer, ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_RESIZE_VIEWPORT_SLOT);
    m_volumeConnection.connect(layer, ::fwRenderOgre::Layer::s_STEREO_MODE_CHANGED_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_SET_STEREO_MODE_SLOT);

    this->initWidgets();
    m_widgets->setVisibility(m_widgetVisibilty);

    const bool isValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image);
    if (isValid)
    {
        this->newImage();
    }
    else
    {
        m_volumeSceneNode->setVisible(false, false);
    }

    m_volumeRenderer->tfUpdate(this->getTransferFunction());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::stopping()
{
    this->removeTFConnections();

    m_volumeConnection.disconnect();
    delete m_volumeRenderer;
    m_volumeRenderer = nullptr;

    m_sceneManager->getRootSceneNode()->removeChild(m_volumeSceneNode->getName());

    ::Ogre::TextureManager::getSingleton().remove(m_3DOgreTexture->getHandle());
    m_3DOgreTexture.reset();

    ::Ogre::TextureManager::getSingleton().remove(m_maskTexture->getHandle());
    m_maskTexture.reset();

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

void SVolumeRender::updating()
{
}

//------------------------------------------------------------------------------

void SVolumeRender::swapping(const KeyType& key)
{
    if (key == s_TF_INOUT)
    {
        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction >(s_TF_INOUT);
        ::fwData::Image::sptr image         = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Missing image", image);

        this->setOrCreateTF(tf, image);
        this->updateTFPoints();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::newImage()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    this->updateImage();

    if (m_autoResetCamera || image->getField("resetCamera"))
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
    else
    {
        this->getLayer()->computeCameraParameters();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateImage()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    this->updateImageInfos(image);

    this->getRenderService()->makeCurrent();

    // Retrieves or creates the slice index fields
    this->updateImageInfos(image);

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    this->createTransferFunction(image);

    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    m_gpuTF.updateTexture(tf);

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_volumeRenderer->getSamplingRate());
    }

    if(m_ambientOcclusion || m_colorBleeding || m_shadows)
    {
        if(m_illum == nullptr)
        {
            m_illum = std::make_shared< ::fwRenderOgre::vr::SATVolumeIllumination>(this->getID(), m_sceneManager,
                                                                                   m_satSizeRatio,
                                                                                   (m_ambientOcclusion ||
                                                                                    m_colorBleeding), m_shadows,
                                                                                   m_satShells, m_satShellRadius,
                                                                                   m_satConeAngle, m_satConeSamples);
        }
        this->updateVolumeIllumination();
    }

    m_volumeRenderer->imageUpdate(image, this->getTransferFunction());

    m_volumeSceneNode->setVisible(true, m_widgets->getVisibility());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::newMask()
{
    ::fwData::Image::sptr mask = this->getInOut< ::fwData::Image>(s_MASK_INOUT);
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
            ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
            SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

            m_volumeRenderer->imageUpdate(image, this->getTransferFunction());
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
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

        m_volumeRenderer->imageUpdate(image, this->getTransferFunction());
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

void SVolumeRender::setStereoMode(::fwRenderOgre::Layer::StereoModeType)
{
    this->stopping();
    this->starting();
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
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->toggleIDVRCountersinkGeometry(val);
    }
    else if(key == "idvrCSGBorder")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->toggleIDVRCSGBorder(val);
    }
    else if(key == "idvrCSGModulation")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->toggleIDVRCSGModulation(val);
    }
    else if(key == "idvrCSGOpacityDecrease")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->toggleIDVRCSGOpacityDecrease(val);
    }
    else if(key == "idvrCSGDepthLines")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->toggleIDVRDepthLines(val);
    }
    else if(key == "idvrCSGDisableContext")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->toggleIDVRCSGDisableContext(val);
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
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRCountersinkSlope(val);
    }
    else if(key == "idvrCSGBlurWeight")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRCSGBlurWeight(val);
    }
    else if(key == "idvrCSGBorderThickness")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRCSGBorderThickness(val);
    }
    else if(key == "idvrCSGDepthLinesThreshold")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRCSGDepthLinesThreshold(val);
    }
    else if(key == "idvrCSGModulationFactor")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRCSGModulationFactor(val);
    }
    else if(key == "idvrCSGOpacityDecreaseFactor")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRCSGOpacityDecreaseFactor(val);
    }
    else if(key == "idvrVPImCAlphaCorrection")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRVPImCAlphaCorrection(val);
    }
    else if(key == "idvrAImCAlphaCorrection")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRAImCAlphaCorrection(val);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setEnumParameter(std::string val, std::string key)
{
    if(key == "idvrMethod")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRMethod(val);
        this->requestRender();
    }
    else if(key == "idvrCSGModulationMethod")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);

        if(val == "Average_grayscale")
        {
            rayCastVolumeRenderer->setIDVRCSModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::AVERAGE_GRAYSCALE);
        }
        else if(val == "Lightness_grayscale")
        {
            rayCastVolumeRenderer->setIDVRCSModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::LIGHTNESS_GRAYSCALE);
        }
        else if(val == "Luminosity_grayscale")
        {
            rayCastVolumeRenderer->setIDVRCSModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::LUMINOSITY_GRAYSCALE);
        }
        else if(val == "Brightness_increase")
        {
            rayCastVolumeRenderer->setIDVRCSModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::COLOR1);
        }
        else if(val == "Saturation_increase")
        {
            rayCastVolumeRenderer->setIDVRCSModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::COLOR2);
        }
        else if(val == "SaturationBrightness_increase")
        {
            rayCastVolumeRenderer->setIDVRCSModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::COLOR3);
        }
        else if(val == "Saturation_decrease")
        {
            rayCastVolumeRenderer->setIDVRCSModulationMethod(
                ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer::IDVRCSGModulationMethod::COLOR4);
        }
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::setColorParameter(std::array<std::uint8_t, 4> color, std::string key)
{
    if(key == "idvrCSGBorderColor")
    {
        auto rayCastVolumeRenderer =
            dynamic_cast< ::fwRenderOgre::vr::ImportanceDrivenVolumeRenderer* >(m_volumeRenderer);
        OSLM_ASSERT("The current VolumeRenderer must be a RayTracingVolumeRenderer", rayCastVolumeRenderer);
        rayCastVolumeRenderer->setIDVRCSGBorderColor(color);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::initWidgets()
{
    // Create widgets.
    {
        auto clippingMatrix = this->getInOut< ::fwData::TransformationMatrix3D>(s_CLIPPING_MATRIX_INOUT);

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
    if(rayCastVolumeRenderer && ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(this->getImage()))
    {
        if((m_ambientOcclusion || m_colorBleeding || m_shadows) && !m_illum)
        {
            m_illum = ::std::make_shared< ::fwRenderOgre::vr::SATVolumeIllumination>(this->getID(), m_sceneManager,
                                                                                     m_satSizeRatio,
                                                                                     (m_ambientOcclusion ||
                                                                                      m_colorBleeding), m_shadows,
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
            ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
            SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

            m_volumeRenderer->imageUpdate(image, this->getTransferFunction());
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

}
