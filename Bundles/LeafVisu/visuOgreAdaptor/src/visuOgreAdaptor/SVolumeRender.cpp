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

#include <fwCore/Profiling.hpp>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVolumeRender, ::fwData::Image);

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEWIMAGE_SLOT = "newImage";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEWSAMPLING_SLOT = "updateSampling";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_TOGGLEPREINTEGRATION_SLOT = "togglePreintegration";
const ::fwCom::Slots::SlotKeyType SVolumeRender::s_RESIZE_VIEWPORT_SLOT        = "resizeViewport";

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
    newSlot(s_TOGGLEPREINTEGRATION_SLOT, &SVolumeRender::togglePreintegration, this);    
    newSlot(s_RESIZE_VIEWPORT_SLOT, &SVolumeRender::resizeViewport, this);

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

    // -- PROFILING --
//    tf->clear();

    // CT-Liver
//    tf->addTFColor(-23, ::fwData::TransferFunction::TFColor(0, 0, 0, 0));
//    tf->addTFColor( 50, ::fwData::TransferFunction::TFColor(0.5, 0, 0.08, 0));
//    tf->addTFColor(100, ::fwData::TransferFunction::TFColor(1, 0.28, 0.28, 0.2));
//    tf->addTFColor(150, ::fwData::TransferFunction::TFColor(1, 0.5, 0.5, 0.4));
//    tf->addTFColor(200, ::fwData::TransferFunction::TFColor(1, 0.5, 0, 0.6));
//    tf->addTFColor(250, ::fwData::TransferFunction::TFColor(1, 1, 0, 0.8));
//    tf->addTFColor(300, ::fwData::TransferFunction::TFColor(1, 1, 1, 1));

//    tf->setLevel(138);
//    tf->setWindow(323);

    // CT-Bones
//    tf->addTFColor(184, ::fwData::TransferFunction::TFColor(0, 0, 0, 0));
//    tf->addTFColor(191, ::fwData::TransferFunction::TFColor(1, 0.5, 0, 0.3));
//    tf->addTFColor(247, ::fwData::TransferFunction::TFColor(1, 1, 0.72, 0.59));
//    tf->addTFColor(341, ::fwData::TransferFunction::TFColor(1, 1, 1, 0.79));

//    tf->setLevel(262);
//    tf->setWindow(157);

    // CT-Vessels
//    tf->addTFColor(128, ::fwData::TransferFunction::TFColor(0, 0, 0, 0));
//    tf->addTFColor(140, ::fwData::TransferFunction::TFColor(0.62, 0, 0, 0));
//    tf->addTFColor(159, ::fwData::TransferFunction::TFColor(1, 0.5, 0.5, 0.3));
//    tf->addTFColor(189, ::fwData::TransferFunction::TFColor(1, 0.5, 0, 0.6));
//    tf->addTFColor(219, ::fwData::TransferFunction::TFColor(1, 1, 0.64, 0.9));
//    tf->addTFColor(304, ::fwData::TransferFunction::TFColor(1, 1, 1, 1));

//    tf->setLevel(216);
//    tf->setWindow(176);

    // CT-Thorax
//    tf->addTFColor(-1011, ::fwData::TransferFunction::TFColor(1, 1, 1, 1));
//    tf->addTFColor(-195, ::fwData::TransferFunction::TFColor(0, 0, 0, 0));
//    tf->addTFColor(-480, ::fwData::TransferFunction::TFColor(0.50196099281311035,0,0,0.28623199462890625));
//    tf->addTFColor(-597, ::fwData::TransferFunction::TFColor(0.9803919792175293,0.67843097448348999,0.27058801054954529,0.51906198263168335));
//    tf->addTFColor(-690, ::fwData::TransferFunction::TFColor(0.9882349967956543,0.83921599388122559,0.63529402017593384,0.72727298736572266));
//    tf->addTFColor(121, ::fwData::TransferFunction::TFColor(0.50196099281311035,0,0,0.39882698655128479));
//    tf->addTFColor(135, ::fwData::TransferFunction::TFColor(1,0.50196099281311035,0.50196099281311035,0.52199399471282959));
//    tf->addTFColor(155, ::fwData::TransferFunction::TFColor(1,1,0.69019597768783569,0.7536659836769104));
//    tf->addTFColor(176, ::fwData::TransferFunction::TFColor(1,1,1,1));
//    tf->addTFColor(5, ::fwData::TransferFunction::TFColor(0,0,0,0));
//    tf->addTFColor(95, ::fwData::TransferFunction::TFColor(0.25097998976707458,0,0,0));

//    tf->setLevel(-418);
//    tf->setWindow(1187);


//    double invWindow = 1. / tf->getWindow();

//    const ::fwData::TransferFunction::TFValuePairType intensityMinMax = tf->getWLMinMax();

//    const ::fwData::TransferFunction::TFValuePairType tfMinMax = tf->getMinMaxTFValues();

//    ::fwData::Image::sptr img = this->getImage();

//    ::fwComEd::helper::Image imgHelper(img);

//    size_t nbPixels = img->getDataArray()->getNumberOfElements();
//    int16_t *ushortImgBuffer = static_cast<int16_t *>(imgHelper.getBuffer());

//    size_t nbNonEmptyPixels = 0;
//    for(size_t i = 0; i < nbPixels; ++ i)
//    {
//        double value = (ushortImgBuffer[i] - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow + tfMinMax.first;

//        ::fwData::TransferFunction::TFColor interpolatedColor = tf->getInterpolatedColor(value);

//        nbNonEmptyPixels += (interpolatedColor.a != 0);
//    }

//    std::cout << "Fill ratio: " << float(nbNonEmptyPixels) / float(nbPixels) << std::endl;

    this->updateTransferFunction(this->getImage());

    m_gpuTF.updateTexture(tf);

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_volumeRenderer->getSamplingRate());
    }

    m_volumeRenderer->tfUpdate(tf);

    FW_PROFILE("SAT")
    {
        m_illum->updateVolIllum(m_3DOgreTexture, m_gpuTF.getTexture());
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

    ::Ogre::Light *mainLight = m_sceneManager->getLight("MainLight");
    ::Ogre::Light *newLight  = m_sceneManager->createLight(getID() + "_TestLight");

    newLight->setType(::Ogre::Light::LT_DIRECTIONAL);
    newLight->setDiffuseColour(::Ogre::ColourValue());
    newLight->setSpecularColour(::Ogre::ColourValue());

    newLight->setDirection(::Ogre::Quaternion(::Ogre::Degree(-45), ::Ogre::Vector3(1,0,0)) * ::Ogre::Quaternion(::Ogre::Degree(90), ::Ogre::Vector3(0,1,0)) * mainLight->getDirection());

    m_camera->getParentSceneNode()->attachObject(newLight);

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
        ::fwRenderOgre::Layer::sptr serviceLayer = this->getRenderService()->getLayer();

        m_volumeRenderer = new ::fwRenderOgre::RayTracingVolumeRenderer(this->getID(),
                                                                        m_sceneManager,
                                                                        m_volumeSceneNode,
                                                                        m_3DOgreTexture,
                                                                        &m_gpuTF,
                                                                        &m_preIntegrationTable,
                                                                        serviceLayer->is3D());

        if(serviceLayer->is3D())
        {
            dynamic_cast< ::fwRenderOgre::RayTracingVolumeRenderer*>(m_volumeRenderer)->configure3DViewport(serviceLayer);
        }
    }

    m_illum = new ::fwRenderOgre::SATVolumeIllumination(this->getID(), m_sceneManager, 128, 128, 128);


    m_volumeRenderer->setPreIntegratedRendering(m_preIntegratedRendering);

    m_volumeConnection.connect(this->getRenderService()->getLayer(), ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                               this->getSptr(), ::visuOgreAdaptor::SVolumeRender::s_RESIZE_VIEWPORT_SLOT);

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

    // -- PROFILING --
//    struct CameraListener : public ::Ogre::Camera::Listener
//    {
//        int i = 1;

//        const int maxFrames = 10600;

//        int speed = 5;

//        ::Ogre::Vector3 rotAxis;
//        ::fwRenderOgre::interactor::TrackballInteractor::sptr vrInteractor;
//        SVolumeRender *m_renderer;

//        CameraListener(SVolumeRender *renderer) :
//            rotAxis(0, 1, 0),
//            m_renderer(renderer)
//        {
//            ::fwRenderOgre::Layer::sptr layer = m_renderer->getRenderService()->getLayer(m_renderer->m_layerID);
//            ::fwRenderOgre::interactor::IInteractor::sptr interactor = layer->getInteractor();

//            vrInteractor = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::TrackballInteractor>(interactor);
//            vrInteractor->mouseMoveEvent(::fwRenderOgre::interactor::IMovementInteractor::LEFT, 1, 1, 0, 0);
//        }

//        virtual void cameraPreRenderScene(::Ogre::Camera*)
//        {
//            vrInteractor->mouseMoveEvent(::fwRenderOgre::interactor::IMovementInteractor::LEFT, -1, -1, 1, 0);

//            if(i % 100 == 0)
//            {
//                vrInteractor->mouseMoveEvent(::fwRenderOgre::interactor::IMovementInteractor::LEFT, -1, -1, 0, speed);
//                speed += 5;
//            }
//            if(++i == maxFrames)
//            {
//                std::abort();
//            }

//            m_renderer->requestRender();
//        }
//    };

//    m_camera->addListener(new CameraListener(this));
    // -- PROFILING END --

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

    m_volumeRenderer->setSampling(m_nbSlices);

    if(m_preIntegratedRendering)
    {
        m_preIntegrationTable.tfUpdate(this->getTransferFunction(), m_volumeRenderer->getSamplingRate());
    }

    this->requestRender();
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

void SVolumeRender::resizeViewport(int w, int h)
{
    if(m_volumeRenderer)
    {
        m_volumeRenderer->resizeViewport(w, h);
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
