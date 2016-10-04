/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SVolumeRender.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>
#include <fwRenderOgre/vr/RayTracingVolumeRenderer.hpp>
#include <fwRenderOgre/vr/SliceVolumeRenderer.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>

#include <boost/algorithm/clamp.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <numeric>
#include <sstream>
#include <string>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVolumeRender, ::fwData::Image);

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEW_IMAGE_SLOT                      = "newImage";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAMPLING_SLOT                = "updateSampling";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SIZE_RATIO_SLOT          = "updateSatSizeRatio";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SHELLS_NUMBER_SLOT       = "updateSatShellsNumber";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SHELL_RADIUS_SLOT        = "updateSatShellRadius";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_CONE_ANGLE_SLOT          = "updateSatConeAngle";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_CONE_SAMPLES_NUMBER_SLOT = "updateSatConeSamplesNumber";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_PREINTEGRATION_SLOT          = "togglePreintegration";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_AMBIENT_OCCLUSION_SLOT       = "toggleAmbientOcclusion";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_COLOR_BLEEDING_SLOT          = "toggleColorBleeding";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_WIDGETS_SLOT                 = "toggleWidgets";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_RESIZE_VIEWPORT_SLOT                = "resizeViewport";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_FOCAL_DISTANCE_SLOT             = "setFocalDistance";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_MODE3D_SLOT                     = "setStereoMode";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() throw() :
    m_volumeRenderer         (nullptr),
    m_sceneManager           (nullptr),
    m_volumeSceneNode        (nullptr),
    m_camera                 (nullptr),
    m_nbSlices               (512),
    m_preIntegratedRendering (false),
    m_ambientOcclusion       (false),
    m_colorBleeding         (false),
    m_widgetVisibilty        (true),
    m_illum                  (nullptr),
    m_satSizeRatio           (0.25f),
    m_satShells              (3),
    m_satShellRadius         (3),
    m_satConeAngle           (0.1f),
    m_satConeSamples         (50)
{
    this->installTFSlots(this);
    newSlot(s_NEW_IMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_UPDATE_SAMPLING_SLOT, &SVolumeRender::updateSampling, this);
    newSlot(s_UPDATE_SAT_SIZE_RATIO_SLOT, &SVolumeRender::updateSatSizeRatio, this);
    newSlot(s_UPDATE_SAT_SHELLS_NUMBER_SLOT, &SVolumeRender::updateSatShellsNumber, this);
    newSlot(s_UPDATE_SAT_SHELL_RADIUS_SLOT, &SVolumeRender::updateSatShellRadius, this);
    newSlot(s_UPDATE_SAT_CONE_ANGLE_SLOT, &SVolumeRender::updateSatConeAngle, this);
    newSlot(s_UPDATE_SAT_CONE_SAMPLES_NUMBER_SLOT, &SVolumeRender::updateSatConeSamplesNumber, this);
    newSlot(s_TOGGLE_PREINTEGRATION_SLOT, &SVolumeRender::togglePreintegration, this);
    newSlot(s_TOGGLE_AMBIENT_OCCLUSION_SLOT, &SVolumeRender::toggleAmbientOcclusion, this);
    newSlot(s_TOGGLE_COLOR_BLEEDING_SLOT, &SVolumeRender::toggleColorBleeding, this);
    newSlot(s_TOGGLE_WIDGETS_SLOT, &SVolumeRender::toggleWidgets, this);
    newSlot(s_RESIZE_VIEWPORT_SLOT, &SVolumeRender::resizeViewport, this);
    newSlot(s_SET_FOCAL_DISTANCE_SLOT, &SVolumeRender::setFocalDistance, this);
    newSlot(s_SET_MODE3D_SLOT, &SVolumeRender::setStereoMode, this);

    m_transform     = ::Ogre::Matrix4::IDENTITY;
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

            if(m_configuration->hasAttribute("ao"))
            {
                m_ambientOcclusion = (m_configuration->getAttributeValue("ao") == "yes");
            }

            if(m_configuration->hasAttribute("colorBleeding"))
            {
                m_colorBleeding = (m_configuration->getAttributeValue("colorBleeding") == "yes");
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

    if(m_ambientOcclusion || m_colorBleeding)
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

    if(m_ambientOcclusion || m_colorBleeding)
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
    m_camera          = m_sceneManager->getCamera("PlayerCam");

    // Create textures
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
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
                                                                       &m_gpuTF,
                                                                       &m_preIntegrationTable);
    }
    else
    {
        m_volumeRenderer = new ::fwRenderOgre::vr::RayTracingVolumeRenderer(this->getID(),
                                                                            m_sceneManager,
                                                                            m_volumeSceneNode,
                                                                            m_3DOgreTexture,
                                                                            &m_gpuTF,
                                                                            &m_preIntegrationTable,
                                                                            layer->getStereoMode(),
                                                                            m_ambientOcclusion,
                                                                            m_colorBleeding);

        if(layer->getStereoMode() != ::fwRenderOgre::Layer::StereoModeType::NONE)
        {
            auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer*>(m_volumeRenderer);

            OSLM_ERROR_IF("Stereo rendering is supported only by ray casting VR.", !rayCastVolumeRenderer);

            rayCastVolumeRenderer->configure3DViewport(layer);

            // Initially focus on the image center.
            setFocalDistance(50);
        }
    }

    if(m_ambientOcclusion || m_colorBleeding)
    {
        m_illum = new ::fwRenderOgre::vr::SATVolumeIllumination(this->getID(), m_sceneManager,
                                                                m_satSizeRatio, m_satShells, m_satShellRadius,
                                                                m_satConeAngle, m_satConeSamples);
    }

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    m_volumeConnection.connect(layer, ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_RESIZE_VIEWPORT_SLOT);
    m_volumeConnection.connect(layer, ::fwRenderOgre::Layer::s_MODE3D_CHANGED_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_SET_MODE3D_SLOT);

    initWidgets();
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

    this->getRenderService()->resetCameraCoordinates(m_layerID);

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

    updatingTFPoints();

    m_volumeSceneNode->setVisible(true, m_widgets->getVisibility());

    m_volumeRenderer->imageUpdate(image, this->getTransferFunction());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSampling(int nbSamples)
{
    OSLM_ASSERT("Sampling rate must fit in a 16 bit uint.", nbSamples < 65536 && nbSamples >= 0);
    m_nbSlices = static_cast<uint16_t>(nbSamples);

    m_volumeRenderer->setSampling(m_nbSlices);

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_volumeRenderer->getSamplingRate());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSatSizeRatio(int sizeRatio)
{
    if(m_ambientOcclusion || m_colorBleeding)
    {
        m_satSizeRatio = static_cast<float>(sizeRatio) / 100;
        m_illum->updateSAT(m_satSizeRatio);

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
    if(m_ambientOcclusion || m_colorBleeding)
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
    if(m_ambientOcclusion || m_colorBleeding)
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
    if(m_ambientOcclusion || m_colorBleeding)
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

void SVolumeRender::updateSatConeSamplesNumber(int nbConeSamples)
{
    if(m_ambientOcclusion || m_colorBleeding)
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
    auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

    // Volume illumination is only implemented for raycasting rendering
    if(rayCastVolumeRenderer)
    {
        m_ambientOcclusion = ambientOcclusion;

        if((m_ambientOcclusion || m_colorBleeding) && !m_illum)
        {
            m_illum = new ::fwRenderOgre::vr::SATVolumeIllumination(this->getID(), m_sceneManager,
                                                                    m_satSizeRatio, m_satShells, m_satShellRadius);
            this->updateVolumeIllumination();
        }

        rayCastVolumeRenderer->setAmbientOcclusion(m_ambientOcclusion);

        if(m_preIntegratedRendering)
        {
            m_volumeRenderer->imageUpdate(this->getImage(), this->getTransferFunction());
        }

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::toggleColorBleeding(bool colorBleeding)
{
    auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

    // Volume illumination is only implemented for raycasting rendering
    if(rayCastVolumeRenderer)
    {
        m_colorBleeding = colorBleeding;

        if((m_ambientOcclusion || m_colorBleeding) && !m_illum)
        {
            m_illum = new ::fwRenderOgre::vr::SATVolumeIllumination(this->getID(), m_sceneManager,
                                                                    m_satSizeRatio, m_satShells, m_satShellRadius);
            this->updateVolumeIllumination();
        }

        rayCastVolumeRenderer->setColorBleeding(m_colorBleeding);

        if(m_preIntegratedRendering)
        {
            m_volumeRenderer->imageUpdate(this->getImage(), this->getTransferFunction());
        }

        this->requestRender();
    }
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

void SVolumeRender::initWidgets()
{
    // Create widgets.
    {
        auto widget = new ::fwRenderOgre::ui::VRWidget(this->getID(),
                                                       m_volumeSceneNode,
                                                       m_camera,
                                                       this->getRenderService(),
                                                       m_sceneManager,
                                                       m_volumeRenderer);

        m_widgets = std::shared_ptr< ::fwRenderOgre::ui::VRWidget >(widget);
    }

    // Connect widgets to interactor.
    {
        ::fwRenderOgre::Layer::sptr layer                        = this->getRenderService()->getLayer(m_layerID);
        ::fwRenderOgre::interactor::IInteractor::sptr interactor = layer->getInteractor();

        auto vrInteractor =
            std::dynamic_pointer_cast< ::fwRenderOgre::interactor::VRWidgetsInteractor >(interactor);

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
    m_illum->SATUpdate(m_3DOgreTexture, m_gpuTF.getTexture());

    // Volume illumination is only implemented for raycasting rendering
    if(m_renderingMode == VR_MODE_RAY_TRACING)
    {
        ::fwRenderOgre::vr::RayTracingVolumeRenderer* rayTracingVolumeRenderer =
            static_cast< ::fwRenderOgre::vr::RayTracingVolumeRenderer* >(m_volumeRenderer);

        rayTracingVolumeRenderer->setIlluminationVolume(m_illum);
    }
}

//-----------------------------------------------------------------------------

}
