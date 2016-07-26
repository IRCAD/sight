#include "visuOgreAdaptor/SVolumeRender.hpp"

#include <algorithm>

#include <boost/algorithm/clamp.hpp>
#include <boost/lexical_cast.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <fwRenderOgre/helper/Shading.hpp>
#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>
#include <fwRenderOgre/SliceVolumeRenderer.hpp>
#include <fwRenderOgre/RayTracingVolumeRenderer.hpp>

#include <numeric>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreTechnique.h>

#include <sstream>
#include <string>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVolumeRender, ::fwData::Image);

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEW_IMAGE_SLOT                  = "newImage";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAMPLING_SLOT            = "updateSampling";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_UPDATE_SAT_SIZE_RATIO_SLOT      = "updateSatSizeRatio";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_PREINTEGRATION_SLOT      = "togglePreintegration";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_VOLUME_ILLUMINATION_SLOT = "toggleVolumeIllumination";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLE_WIDGETS_SLOT             = "toggleWidgets";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_RESIZE_VIEWPORT_SLOT            = "resizeViewport";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_SET_FOCAL_DISTANCE_SLOT         = "setFocalDistance";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() throw() :
    m_volumeRenderer         (nullptr),
    m_sceneManager           (nullptr),
    m_volumeSceneNode        (nullptr),
    m_camera                 (nullptr),
    m_nbSlices               (512),
    m_preIntegratedRendering (false),
    m_volumeIllumination     (false),
    m_widgetVisibilty        (true),
    m_illum                  (nullptr),
    m_satSizeRatio           (0.25f),
    m_satShells              (3),
    m_satShellRadius         (3)
{
    this->installTFSlots(this);
    newSlot(s_NEW_IMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_UPDATE_SAMPLING_SLOT, &SVolumeRender::updateSampling, this);
    newSlot(s_UPDATE_SAT_SIZE_RATIO_SLOT, &SVolumeRender::updateSatSizeRatio, this);
    newSlot(s_TOGGLE_PREINTEGRATION_SLOT, &SVolumeRender::togglePreintegration, this);
    newSlot(s_TOGGLE_VOLUME_ILLUMINATION_SLOT, &SVolumeRender::toggleVolumeIllumination, this);
    newSlot(s_TOGGLE_WIDGETS_SLOT, &SVolumeRender::toggleWidgets, this);
    newSlot(s_RESIZE_VIEWPORT_SLOT, &SVolumeRender::resizeViewport, this);
    newSlot(s_SET_FOCAL_DISTANCE_SLOT, &SVolumeRender::setFocalDistance, this);

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

                m_volumeIllumination = true;
            }

            if(m_configuration->hasAttribute("satShells"))
            {
                std::string shellsString = m_configuration->getAttributeValue("satShells");
                m_satShells = std::stoi(shellsString);

                m_volumeIllumination = true;
            }

            if(m_configuration->hasAttribute("satShellRadius"))
            {
                std::string shellRadiusString = m_configuration->getAttributeValue("satShellRadius");
                m_satShellRadius = std::stoi(shellRadiusString);

                m_volumeIllumination = true;
            }

            if(m_configuration->hasAttribute("volumeIllumination"))
            {
                m_volumeIllumination = (m_configuration->getAttributeValue("volumeIllumination") == "yes");
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

    if(m_volumeIllumination)
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

    if(m_volumeIllumination)
    {
        this->updateVolumeIllumination();
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsType SVolumeRender::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;

    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_NEW_IMAGE_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

void SVolumeRender::doStart() throw ( ::fwTools::Failed )
{
    ::fwData::Composite::wptr tfSelection = this->getSafeInOut< ::fwData::Composite>(this->getTFSelectionFwID());
    this->setTransferFunctionSelection(tfSelection);

    this->updateImageInfos(this->getObject< ::fwData::Image >());
    this->updateTransferFunction(this->getImage());

    this->installTFConnections();

    m_sceneManager    = this->getSceneManager();
    m_volumeSceneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    m_camera          = m_sceneManager->getCamera("PlayerCam");

    // Create textures
    m_3DOgreTexture = ::Ogre::TextureManager::getSingletonPtr()->create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    m_gpuTF.createTexture(this->getID());
    m_preIntegrationTable.createTexture(this->getID());

    if(m_renderingMode == VR_MODE_SLICE)
    {
        m_volumeRenderer = new ::fwRenderOgre::SliceVolumeRenderer(this->getID(),
                                                                   m_sceneManager,
                                                                   m_volumeSceneNode,
                                                                   m_3DOgreTexture,
                                                                   &m_gpuTF,
                                                                   &m_preIntegrationTable);
    }
    else
    {
        ::fwRenderOgre::Layer::sptr serviceLayer = this->getRenderService()->getLayer(m_layerID);

        m_volumeRenderer = new ::fwRenderOgre::RayTracingVolumeRenderer(this->getID(),
                                                                        m_sceneManager,
                                                                        m_volumeSceneNode,
                                                                        m_3DOgreTexture,
                                                                        &m_gpuTF,
                                                                        &m_preIntegrationTable,
                                                                        serviceLayer->is3D(),
                                                                        m_volumeIllumination);

        if(serviceLayer->is3D())
        {
            auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::RayTracingVolumeRenderer*>(m_volumeRenderer);

            OSLM_ERROR_IF("Stereo rendering is supported only by ray casting VR.", !rayCastVolumeRenderer);

            rayCastVolumeRenderer->configure3DViewport(serviceLayer);

            // Initially focus on the image center.
            setFocalDistance(50);
        }
    }

    if(m_volumeIllumination)
    {
        m_illum = new ::fwRenderOgre::SATVolumeIllumination(this->getID(), m_sceneManager,
                                                            m_satSizeRatio, m_satShells, m_satShellRadius);
    }

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    m_volumeConnection.connect(this->getRenderService()->getLayer(m_layerID), ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_RESIZE_VIEWPORT_SLOT);

    initWidgets();
    m_widgets->setVisibility(m_widgetVisibilty);

    bool isValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(this->getImage());
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
    m_volumeConnection.disconnect();
    delete m_volumeRenderer;

    m_sceneManager->getRootSceneNode()->removeChild(m_volumeSceneNode->getName());
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
    if(m_volumeIllumination)
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

void SVolumeRender::toggleVolumeIllumination(bool volumeIllumination)
{
    auto rayCastVolumeRenderer = dynamic_cast< ::fwRenderOgre::RayTracingVolumeRenderer* >(m_volumeRenderer);

    // Volume illumination is only implemented for raycasting rendering
    if(rayCastVolumeRenderer)
    {
        m_volumeIllumination = volumeIllumination;

        if(m_volumeIllumination && !m_illum)
        {
            m_illum = new ::fwRenderOgre::SATVolumeIllumination(this->getID(), m_sceneManager,
                                                                m_satSizeRatio, m_satShells, m_satShellRadius);
            this->updateVolumeIllumination();
        }

        rayCastVolumeRenderer->setVolumeIllumination(m_volumeIllumination);

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
    if(this->getRenderService()->getLayer(m_layerID)->is3D())
    {
        auto rayTracingRenderer = dynamic_cast< ::fwRenderOgre::RayTracingVolumeRenderer*>(m_volumeRenderer);

        if(rayTracingRenderer)
        {
            rayTracingRenderer->setFocalLength(static_cast<float>(focalDistance) / 100);

            this->requestRender();
        }
    }
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
    m_illum->updateVolIllum(m_3DOgreTexture, m_gpuTF.getTexture());

    // Volume illumination is only implemented for raycasting rendering
    if(m_renderingMode == VR_MODE_RAY_TRACING)
    {
        ::fwRenderOgre::RayTracingVolumeRenderer *rayTracingVolumeRenderer =
            static_cast< ::fwRenderOgre::RayTracingVolumeRenderer* >(m_volumeRenderer);

        rayTracingVolumeRenderer->setIlluminationVolume(m_illum->getIlluminationVolume());
    }
}

//-----------------------------------------------------------------------------

}
