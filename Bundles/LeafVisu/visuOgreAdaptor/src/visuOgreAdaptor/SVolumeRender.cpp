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

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVolumeRender, ::fwData::Image);

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEWIMAGE_SLOT = "newImage";

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEWSAMPLING_SLOT = "updateSampling";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() throw() :
    m_volumeRenderer         (nullptr),
    m_sceneManager           (nullptr),
    m_volumeSceneNode        (nullptr),
    m_camera                 (nullptr),
    m_nbSlices               (512),
    m_preIntegratedRendering (false)
{
    this->installTFSlots(this);
    newSlot(s_NEWIMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_NEWSAMPLING_SLOT, &SVolumeRender::samplingChanged, this);

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
        }
        else
        {
            OSLM_WARN("Unknown VR mode, defaults to ray tracing.");
        }
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
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_nbSlices);
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
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_nbSlices);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsType SVolumeRender::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;

    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

void SVolumeRender::doStart() throw ( ::fwTools::Failed )
{
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
        m_volumeRenderer = new ::fwRenderOgre::RayTracingVolumeRenderer(this->getID(),
                                                                        m_sceneManager,
                                                                        m_volumeSceneNode,
                                                                        m_3DOgreTexture,
                                                                        &m_gpuTF,
                                                                        &m_preIntegrationTable);
    }

    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    initWidgets();
    m_widgets->hide();

    bool isValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(this->getImage());
    if (isValid)
    {
        this->newImage();
        m_widgets->show();
    }
    else
    {
        m_volumeSceneNode->setVisible(false);
    }

    this->getRenderService()->resetCameraCoordinates(m_layerID);
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::doStop() throw ( ::fwTools::Failed )
{
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
    m_widgets->show();
    m_volumeSceneNode->setVisible(true);

    m_volumeRenderer->imageUpdate(image, this->getTransferFunction());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::samplingChanged(int nbSamples)
{
    OSLM_ASSERT("Sampling rate must fit in a 16 bit uint.", nbSamples < 65536 && nbSamples >= 0);
    m_nbSlices = static_cast<uint16_t>(nbSamples);

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_nbSlices);
    }

    m_volumeRenderer->setSampling(m_nbSlices);

    this->requestRender();
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
                                                       m_volumeRenderer);

        m_widgets = std::shared_ptr< ::fwRenderOgre::ui::VRWidget >(widget);
    }

    // Connect widgets to interactor.
    {
        ::fwRenderOgre::Layer::sptr layer                        = this->getRenderService()->getLayer(m_layerID);
        ::fwRenderOgre::interactor::IInteractor::sptr interactor = layer->getInteractor();

        auto vrInteractor =
            std::dynamic_pointer_cast< ::fwRenderOgre::interactor::VRWidgetsInteractor >(interactor);

        vrInteractor->initPicker();
        vrInteractor->attachWidget(m_widgets);
    }
}

//-----------------------------------------------------------------------------

}
