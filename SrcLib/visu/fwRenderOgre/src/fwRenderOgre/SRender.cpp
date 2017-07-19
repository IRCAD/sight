/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/SRender.hpp"

#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/registry/Adaptor.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>

#include <fwRuntime/ConfigurationElementContainer.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/fwID.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

#include <stack>

fwServicesRegisterMacro( ::fwRender::IRender, ::fwRenderOgre::SRender, ::fwData::Composite );

namespace fwRenderOgre
{

const std::string SRender::s_OGREBACKGROUNDID = "ogreBackground";

const ::fwCom::Slots::SlotKeyType SRender::s_SCENE_STARTED_SIG = "sceneStarted";

const ::fwCom::Slots::SlotKeyType SRender::s_START_OBJECT_SLOT            = "startObject";
const ::fwCom::Slots::SlotKeyType SRender::s_COMPUTE_CAMERA_ORIG_SLOT     = "computeCameraParameters";
const ::fwCom::Slots::SlotKeyType SRender::s_COMPUTE_CAMERA_CLIPPING_SLOT = "computeCameraClipping";
const ::fwCom::Slots::SlotKeyType SRender::s_DO_RAY_CAST_SLOT             = "doRayCast";
const ::fwCom::Slots::SlotKeyType SRender::s_REQUEST_RENDER_SLOT          = "requestRender";

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObject";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT = "changeObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

//-----------------------------------------------------------------------------

SRender::SRender() noexcept :
    m_interactorManager(nullptr),
    m_showOverlay(false),
    m_startAdaptor(false),
    m_renderOnDemand(true),
    m_isReady(false),
    m_fullscreen(false)
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();

    newSignal<SceneStartedSignalType>(s_SCENE_STARTED_SIG);

    newSlot(s_START_OBJECT_SLOT, &SRender::startObject, this);
    newSlot(s_COMPUTE_CAMERA_ORIG_SLOT, &SRender::resetCameraCoordinates, this);
    newSlot(s_COMPUTE_CAMERA_CLIPPING_SLOT, &SRender::computeCameraClipping, this);
    newSlot(s_DO_RAY_CAST_SLOT, &SRender::doRayCast, this);
    newSlot(s_REQUEST_RENDER_SLOT, &SRender::requestRender, this);
}

//-----------------------------------------------------------------------------

SRender::~SRender() noexcept
{
    m_ogreRoot = nullptr;

    for(auto& sceneAdaptor : m_adaptors)
    {
        auto& registry = ::fwRenderOgre::registry::getAdaptorRegistry();
        registry.erase(sceneAdaptor.first);
    }
}

//-----------------------------------------------------------------------------

void SRender::configuring()
{
    this->initialize();

    std::vector < ConfigurationType > vectConfig = m_configuration->find("scene");

    if(!vectConfig.empty())
    {
        auto sceneConfiguration = vectConfig.at(0);

        if (sceneConfiguration->hasAttribute("overlay"))
        {
            const std::string overlay = sceneConfiguration->getAttributeValue("overlay");
            if (overlay == "true")
            {
                m_showOverlay = true;
            }
        }

        if(sceneConfiguration->hasAttribute("fullscreen"))
        {
            std::string fullscreen = sceneConfiguration->getAttributeValue("fullscreen");

            m_fullscreen = (fullscreen == "yes");
        }

        std::string renderMode = sceneConfiguration->getAttributeValue("renderMode");
        if (renderMode == "auto")
        {
            m_renderOnDemand = true;
        }
        else if (renderMode == "always")
        {
            m_renderOnDemand = false;
        }
    }

    auto adaptorConfigs = m_configuration->findAllConfigurationElement("adaptor");
    for (const auto& currentConfig : adaptorConfigs)
    {
        SLM_ASSERT("Missing 'uid' attribute in adaptor configuration", currentConfig->hasAttribute("uid"));
        SLM_ASSERT("Missing 'start' attribute in adaptor configuration", currentConfig->hasAttribute("start"));

        SceneAdaptor adaptor;
        std::string uid        = currentConfig->getAttributeValue("uid");
        std::string startValue = currentConfig->getAttributeValue("start");

        SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                   startValue == "yes" || startValue == "no");
        adaptor.m_start = (startValue == "yes");

        m_adaptors[uid] = adaptor;

        auto& registry = ::fwRenderOgre::registry::getAdaptorRegistry();
        registry[uid] = this->getID();
    }
}

//-----------------------------------------------------------------------------

void SRender::starting()
{
    SLM_TRACE_FUNC();

    bool bHasBackground = false;

    this->create();

    // Instantiate ogre object, class...
    for (auto iter : * m_configuration)
    {
        // Configure layers
        if (iter->getName() == "layer")
        {
            this->configureLayer(iter);
        }
        else if(iter->getName() == "background")
        {
            OSLM_ERROR_IF("A background has already been set, overriding it...", bHasBackground);
            this->configureBackgroundLayer(iter);
            bHasBackground = true;
        }
        else if (iter->getName() == "adaptor" || iter->getName() == "scene" || iter->getName() == "registry")
        {
            continue;
        }
        else
        {
            OSLM_ASSERT("Bad scene configurationType, unknown xml node : " << iter->getName(), false);
        }
    }

    if(!bHasBackground)
    {
        // Create a default black background
        ::fwRenderOgre::Layer::sptr ogreLayer = ::fwRenderOgre::Layer::New();
        ogreLayer->setRenderService(::fwRenderOgre::SRender::dynamicCast(this->shared_from_this()));
        ogreLayer->setID("backgroundLayer");
        ogreLayer->setDepth(0);
        ogreLayer->setWorker(m_associatedWorker);
        ogreLayer->setBackgroundColor("#000000", "#000000");
        ogreLayer->setBackgroundScale(0, 0.5);
        ogreLayer->setHasDefaultLight(false);

        m_layers[s_OGREBACKGROUNDID] = ogreLayer;
    }

    m_interactorManager = ::fwRenderOgre::IRenderWindowInteractorManager::createManager();
    m_interactorManager->setRenderService(this->getSptr());
    m_interactorManager->createContainer( this->getContainer(), m_showOverlay, m_renderOnDemand, m_fullscreen );
}

//-----------------------------------------------------------------------------

void SRender::stopping()
{
    m_connections.disconnect();

    // Stop adaptors in the reverse order of their starting priority
    std::vector< SPTR(IAdaptor) > stopAdaptors;

    for(auto& sceneAdaptor : m_adaptors)
    {
        if(sceneAdaptor.second.m_start && sceneAdaptor.second.getService())
        {
            stopAdaptors.emplace_back(sceneAdaptor.second.getService());
        }
    }

    std::sort(stopAdaptors.begin(), stopAdaptors.end(),
              [](const SPTR(IAdaptor)& a, const SPTR(IAdaptor)& b)
        {
            return b->getStartPriority() < a->getStartPriority();
        });

    for(auto& adaptor : stopAdaptors)
    {
        if(adaptor->isStarted())
        {
            adaptor->stop();
        }
    }
    stopAdaptors.clear();
    m_layers.clear();

    m_interactorManager->disconnectInteractor();
    m_interactorManager.reset();

    this->destroy();
}

//-----------------------------------------------------------------------------

void SRender::updating()
{
}

//-----------------------------------------------------------------------------

void SRender::makeCurrent()
{
    m_interactorManager->makeCurrent();
}

//-----------------------------------------------------------------------------

void SRender::configureLayer( ConfigurationType conf )
{
    const std::string id                    = conf->getAttributeValue("id");
    const std::string layer                 = conf->getAttributeValue("depth");
    const std::string compositors           = conf->getAttributeValue("compositors");
    const std::string transparencyTechnique = conf->getAttributeValue("transparency");
    const std::string numPeels              = conf->getAttributeValue("numPeels");
    const std::string stereoMode            = conf->getAttributeValue("stereoMode");
    const std::string defaultLight          = conf->getAttributeValue("defaultLight");

    SLM_ASSERT( "'id' required attribute missing or empty", !id.empty() );
    SLM_ASSERT( "'layer' required attribute missing or empty", !layer.empty() );
    SLM_ASSERT( "Unknown 3D mode : " << stereoMode,
                stereoMode.empty() || stereoMode == "no" || stereoMode == "AutoStereo5" || stereoMode == "AutoStereo8");

    const int layerDepth = ::boost::lexical_cast<int>(layer);

    SLM_ASSERT("Attribute 'layer' must be greater than 0", layerDepth > 0);

    ::fwRenderOgre::Layer::sptr ogreLayer = ::fwRenderOgre::Layer::New();
    ogreLayer->setRenderService(::fwRenderOgre::SRender::dynamicCast(this->shared_from_this()));
    ogreLayer->setID(id);
    ogreLayer->setDepth(layerDepth);
    ogreLayer->setWorker(m_associatedWorker);
    ogreLayer->setStereoMode(stereoMode == "AutoStereo5" ? ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5 :
                             stereoMode == "AutoStereo8" ? ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8 :
                             ::fwRenderOgre::Layer::StereoModeType::NONE);

    ogreLayer->setCoreCompositorEnabled(id == "default", transparencyTechnique, numPeels);
    ogreLayer->setCompositorChainEnabled(compositors);

    if(!defaultLight.empty() && defaultLight == "no")
    {
        ogreLayer->setHasDefaultLight(false);
    }

    // Finally, the layer is pushed in the map
    m_layers[id] = ogreLayer;
}

//-----------------------------------------------------------------------------

void SRender::configureBackgroundLayer( ConfigurationType conf )
{
    SLM_ASSERT( "'id' required attribute missing or empty", !this->getID().empty() );

    ::fwRenderOgre::Layer::sptr ogreLayer = ::fwRenderOgre::Layer::New();
    ogreLayer->setRenderService(::fwRenderOgre::SRender::dynamicCast(this->shared_from_this()));
    ogreLayer->setID(s_OGREBACKGROUNDID);
    ogreLayer->setDepth(0);
    ogreLayer->setWorker(m_associatedWorker);
    ogreLayer->setHasDefaultLight(false);

    if (conf)
    {
        if (conf->hasAttribute("topColor") && conf->hasAttribute("bottomColor"))
        {
            std::string topColor = conf->getAttributeValue("topColor");
            std::string botColor = conf->getAttributeValue("bottomColor");

            ogreLayer->setBackgroundColor(topColor, botColor);
        }

        if (conf->hasAttribute("topScale") && conf->hasAttribute("bottomScale"))
        {
            std::string topScale = conf->getAttributeValue("topScale");
            std::string botScale = conf->getAttributeValue("bottomScale");

            const float topScaleVal = ::boost::lexical_cast<float>(topScale);
            const float botScaleVal = ::boost::lexical_cast<float>(botScale);

            ogreLayer->setBackgroundScale(topScaleVal, botScaleVal);
        }
    }

    m_layers[s_OGREBACKGROUNDID] = ogreLayer;
}

//-----------------------------------------------------------------------------

void SRender::startObject()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    for (auto it : m_layers)
    {
        ::fwRenderOgre::Layer::sptr layer = it.second;
        layer->setRenderWindow(m_interactorManager->getRenderWindow());
        layer->createScene();
    }

    // Everything is started now, we can safely create connections and thus receive interactions from the widget
    m_interactorManager->connectToContainer();

    m_isReady = true;

    // Instantiate ogre object, class...
    if (this->isStarted())
    {
        std::vector< WPTR(IAdaptor) > startAdaptors;
        auto servicesVector = ::fwServices::OSR::getServices("::fwRenderOgre::IAdaptor");

        for(auto& sceneAdaptor : m_adaptors)
        {
            if(sceneAdaptor.second.m_start)
            {
                auto result =
                    std::find_if(servicesVector.begin(), servicesVector.end(),
                                 [sceneAdaptor](const ::fwServices::IService::sptr& srv)
                    {
                        return srv->getID() == sceneAdaptor.first;
                    });

                SLM_ASSERT("Adaptor '" + sceneAdaptor.first + "' is not found", result != servicesVector.end());

                sceneAdaptor.second.m_service = ::fwRenderOgre::IAdaptor::dynamicCast(*result);
                startAdaptors.emplace_back(sceneAdaptor.second.getService());
            }
        }

        std::sort(startAdaptors.begin(), startAdaptors.end(), [](const WPTR(IAdaptor)& a, const WPTR(IAdaptor)& b)
            {
                return b.lock()->getStartPriority() > a.lock()->getStartPriority();
            });

        for(auto& adaptor : startAdaptors)
        {
            adaptor.lock()->start();
        }
        m_startAdaptor = true;
    }

    auto sig = this->signal<SceneStartedSignalType>(s_SCENE_STARTED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SRender::doRayCast(int x, int y, int width, int height)
{
    for (auto it : m_layers)
    {
        ::fwRenderOgre::Layer::sptr layer = it.second;
        layer->doRayCast(x, y, width, height);
        break;
    }
}

//-----------------------------------------------------------------------------

void SRender::requestRender()
{
    if( this->getContainer()->isShownOnScreen() )
    {
        m_interactorManager->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SRender::resetCameraCoordinates(const std::string& _layerId)
{
    auto layer = m_layers.find(_layerId);

    if(layer != m_layers.end())
    {
        layer->second->resetCameraCoordinates();
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SRender::computeCameraClipping()
{
    for (auto it : m_layers)
    {
        ::fwRenderOgre::Layer::sptr layer = it.second;
        layer->resetCameraClippingRange();
    }
}

//-----------------------------------------------------------------------------

void SRender::addAdaptor(::fwRenderOgre::IAdaptor::sptr _adaptor)
{
    SceneAdaptor newAdaptor;
    newAdaptor.m_service = _adaptor;
    newAdaptor.m_start   = true;

    auto adaptorId = _adaptor->getID();

    m_adaptors[adaptorId] = newAdaptor;

    auto& registry = ::fwRenderOgre::registry::getAdaptorRegistry();
    registry[adaptorId] = this->getID();
}

//-----------------------------------------------------------------------------

void SRender::removeAdaptor(::fwRenderOgre::IAdaptor::sptr _adaptor)
{
    m_adaptors.erase(_adaptor->getID());
}

//-----------------------------------------------------------------------------

void SRender::render()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

bool SRender::isShownOnScreen()
{
    return this->getContainer()->isShownOnScreen();
}

// ----------------------------------------------------------------------------

::Ogre::SceneManager* SRender::getSceneManager(const ::std::string& sceneID)
{
    ::fwRenderOgre::Layer::sptr layer = this->getLayer(sceneID);
    return layer->getSceneManager();
}

// ----------------------------------------------------------------------------

::fwRenderOgre::Layer::sptr SRender::getLayer(const ::std::string& sceneID)
{
    OSLM_ASSERT("Empty sceneID", !sceneID.empty());
    OSLM_ASSERT("Layer ID "<< sceneID <<" does not exist", m_layers.find(sceneID) != m_layers.end());

    ::fwRenderOgre::Layer::sptr layer = m_layers.at(sceneID);

    return layer;
}

// ----------------------------------------------------------------------------

::fwRenderOgre::SRender::LayerMapType SRender::getLayers()
{
    return m_layers;
}

// ----------------------------------------------------------------------------

::fwRenderOgre::IRenderWindowInteractorManager::sptr SRender::getInteractorManager() const
{
    return m_interactorManager;
}

// ----------------------------------------------------------------------------

} //namespace fwRenderOgre
