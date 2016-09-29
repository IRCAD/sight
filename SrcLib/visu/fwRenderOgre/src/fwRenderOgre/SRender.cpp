/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/SRender.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwRuntime/ConfigurationElementContainer.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/fwID.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneNode.h>

#include <stack>

fwServicesRegisterMacro( ::fwRender::IRender, ::fwRenderOgre::SRender, ::fwData::Composite );

namespace fwRenderOgre
{

const ::fwCom::Slots::SlotKeyType SRender::s_START_OBJECT_SLOT            = "startObject";
const ::fwCom::Slots::SlotKeyType SRender::s_COMPUTE_CAMERA_ORIG_SLOT     = "computeCameraParameters";
const ::fwCom::Slots::SlotKeyType SRender::s_COMPUTE_CAMERA_CLIPPING_SLOT = "computeCameraClipping";
const ::fwCom::Slots::SlotKeyType SRender::s_DO_RAY_CAST_SLOT             = "doRayCast";

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObject";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT = "changeObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

static const char* s_ogreBackgroundId = "ogreBackground";

//-----------------------------------------------------------------------------

SRender::SRender() throw() :
    m_interactorManager(nullptr),
    m_showOverlay(false),
    m_startAdaptor(false),
    m_renderOnDemand(true),
    m_fullscreen(false)
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();

    newSlot(s_START_OBJECT_SLOT, &SRender::startObject, this);
    newSlot(s_COMPUTE_CAMERA_ORIG_SLOT, &SRender::resetCameraCoordinates, this);
    newSlot(s_COMPUTE_CAMERA_CLIPPING_SLOT, &SRender::computeCameraClipping, this);
    newSlot(s_DO_RAY_CAST_SLOT, &SRender::doRayCast, this);
    newSlot(s_ADD_OBJECTS_SLOT, &SRender::addObjects, this);
    newSlot(s_CHANGE_OBJECTS_SLOT, &SRender::changeObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &SRender::removeObjects, this);
}

//-----------------------------------------------------------------------------

SRender::~SRender() throw()
{
    m_ogreRoot = nullptr;
}

//-----------------------------------------------------------------------------

void SRender::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    SLM_FATAL_IF( "Depreciated tag \"win\" in configuration", m_configuration->findConfigurationElement("win") );
    this->initialize();

    std::vector < ConfigurationType > vectConfig = m_configuration->find("scene");

    SLM_ASSERT("Empty config !", !vectConfig.empty());
    m_sceneConfiguration = vectConfig.at(0);

    if (m_sceneConfiguration->hasAttribute("overlay"))
    {
        const std::string overlay = m_sceneConfiguration->getAttributeValue("overlay");
        if (overlay == "true")
        {
            m_showOverlay = true;
        }
    }

    std::string renderMode = m_sceneConfiguration->getAttributeValue("renderMode");
    if (renderMode == "auto")
    {
        m_renderOnDemand = true;
    }
    else if (renderMode == "always")
    {
        m_renderOnDemand = false;
    }

    if(m_sceneConfiguration->hasAttribute("fullscreen"))
    {
        std::string fullscreen = m_sceneConfiguration->getAttributeValue("fullscreen");

        m_fullscreen = (fullscreen == "yes");
    }
}

//-----------------------------------------------------------------------------

void SRender::starting() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    bool bHasBackground = false;

    this->create();

    // Instantiate ogre object, class...
    for (auto iter : * m_sceneConfiguration)
    {
        // Configure layers
        if (iter->getName() == "renderer")
        {
            this->configureLayer(iter);
        } // Configure adaptors
        else if (iter->getName() == "adaptor")
        {
            this->configureObject(iter);
        }
        else if(iter->getName() == "background")
        {
            OSLM_ERROR_IF("A background has already been set, overriding it...", bHasBackground);
            this->configureBackgroundLayer(iter);
            bHasBackground = true;
        }
        else if(iter->getName() == "connect" || iter->getName() == "proxy")
        {
            // Connect later in startObject() slot
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
        ogreLayer->setRenderService(SRender::dynamicCast(this->shared_from_this()));
        ogreLayer->setID("backgroundLayer");
        ogreLayer->setDepth(0);
        ogreLayer->setWorker(m_associatedWorker);
        ogreLayer->setBackgroundColor("#000000", "#000000");
        ogreLayer->setBackgroundScale(0, 0.5);

        m_layers[s_ogreBackgroundId] = ogreLayer;
    }
    this->startContext();
}

//-----------------------------------------------------------------------------

void SRender::stopping() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    m_connections.disconnect();

    if(this->isVersion2())
    {
        ConstObjectMapType container;
        for(auto obj : this->getInputs())
        {
            if(obj.first != s_DEFAULT_OBJECT)
            {
                container[obj.first] = obj.second.lock();
            }
        }
        for(auto obj : this->getInOuts())
        {
            if(obj.first != s_DEFAULT_OBJECT)
            {
                container[obj.first] = obj.second.lock();
            }
        }
        this->disconnect(container);
        ::fwServices::helper::Config::disconnectProxies("self", m_proxyMap);
    }
    else
    {
        this->disconnect(this->getComposite()->getContainer());
    }

    // Stop adaptors in the reverse order of their starting priority
    std::vector< SPTR(IAdaptor) > stopAdaptors;

    for(auto& sceneAdaptor : m_sceneAdaptors)
    {
        stopAdaptors.emplace_back(sceneAdaptor.second.getService());
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
        SLM_ASSERT("Adaptor is not stopped", adaptor->isStopped());
        ::fwServices::OSR::unregisterService(adaptor);
    }
    stopAdaptors.clear();
    m_sceneAdaptors.clear();
    m_layers.clear();

    this->stopContext();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SRender::updating() throw(fwTools::Failed)
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
    const std::string layer                 = conf->getAttributeValue("layer");
    const std::string compositors           = conf->getAttributeValue("compositors");
    const std::string transparencyTechnique = conf->getAttributeValue("transparency");
    const std::string numPeels              = conf->getAttributeValue("numPeels");
    const std::string mode3D                = conf->getAttributeValue("mode3D");

    SLM_ASSERT( "'id' required attribute missing or empty", !id.empty() );
    SLM_ASSERT( "'layer' required attribute missing or empty", !layer.empty() );
    SLM_ASSERT( "Unknown 3D mode : " << mode3D,
                mode3D.empty() || mode3D == "no" || mode3D == "AutoStereo5" || mode3D == "AutoStereo8");

    const int layerDepth = ::boost::lexical_cast<int>(layer);

    SLM_ASSERT("Attribute 'layer' must be greater than 0", layerDepth > 0);

    ::fwRenderOgre::Layer::sptr ogreLayer = ::fwRenderOgre::Layer::New();
    ogreLayer->setRenderService(SRender::dynamicCast(this->shared_from_this()));
    ogreLayer->setID(id);
    ogreLayer->setDepth(layerDepth);
    ogreLayer->setWorker(m_associatedWorker);
    ogreLayer->setStereoMode(mode3D == "AutoStereo5" ? ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5 :
                             mode3D == "AutoStereo8" ? ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8 :
                             ::fwRenderOgre::Layer::StereoModeType::NONE);

    ogreLayer->setCoreCompositorEnabled(id == "default", transparencyTechnique, numPeels);
    ogreLayer->setCompositorChainEnabled(compositors != "", compositors);

    // Finally, the layer is pushed in the map
    m_layers[id] = ogreLayer;
}

//-----------------------------------------------------------------------------

void SRender::configureBackgroundLayer( ConfigurationType conf )
{
    SLM_ASSERT( "'id' required attribute missing or empty", !this->getID().empty() );

    ::fwRenderOgre::Layer::sptr ogreLayer = ::fwRenderOgre::Layer::New();
    ogreLayer->setRenderService(SRender::dynamicCast(this->shared_from_this()));
    ogreLayer->setID("backgroundLayer");
    ogreLayer->setDepth(0);
    ogreLayer->setWorker(m_associatedWorker);

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

    m_layers[s_ogreBackgroundId] = ogreLayer;
}

//-----------------------------------------------------------------------------

void SRender::configureObject( ConfigurationType conf )
{
    SLM_ASSERT("Not an \"adaptor\" configuration", conf->getName() == "adaptor");
    ::fwData::Composite::sptr composite = this->getComposite();

    const std::string id            = conf->getAttributeValue("id");
    const std::string objectId      = conf->getAttributeValue("objectId");
    const std::string adaptor       = conf->getAttributeValue("class");
    const std::string uid           = conf->getAttributeValue("uid");
    const std::string compositeName = "self";

    SLM_ASSERT( "'id' required attribute missing or empty", !id.empty() );
    SLM_ASSERT( "'objectId' required attribute missing or empty", !objectId.empty() );
    SLM_ASSERT( "'adaptor' required attribute missing or empty", !adaptor.empty() );

    const size_t compositeObjectCount = composite->getContainer().count(objectId);

    OSLM_TRACE_IF(objectId << " not found in composite. If it exists, associated Adaptor will be destroyed",
                  !(compositeObjectCount == 1 || objectId == compositeName) );

    ::fwData::Object::csptr object;
    if (compositeObjectCount)
    {
        object = ::fwData::Object::dynamicCast(composite->getContainer()[objectId]);
    }
    else if (objectId == compositeName)
    {
        object = ::fwData::Object::dynamicCast(composite);
    }
    else if(this->isVersion2())
    {
        // Last chance with V2 behavior
        object = this->getInput< ::fwData::Object >(objectId);
        if(!object)
        {
            object = this->getInOut< ::fwData::Object >(objectId);
        }
    }

    // If the adaptor isn't already stored in the map and the object exists
    if ( m_sceneAdaptors.count(id) == 0 && object )
    {
        OSLM_TRACE( "Adding service : IAdaptor " << adaptor << " on "<< objectId );
        SceneAdaptor adaptee;
        adaptee.m_config = *(conf->begin());
        auto osr = ::fwServices::registry::ServiceFactory::getDefault();

        if (!uid.empty())
        {
            OSLM_TRACE("SRender::configureObject : uid = " << uid);

            ::fwServices::IService::sptr service = osr->create( adaptor );
            ::fwServices::OSR::registerService( ::fwData::Object::constCast(object), service );
            service->setID( uid );

            adaptee.m_service = ::fwRenderOgre::IAdaptor::dynamicCast(service);
        }
        else
        {
            ::fwServices::IService::sptr service = osr->create( adaptor );
            ::fwServices::OSR::registerService( ::fwData::Object::constCast(object), service );

            adaptee.m_service = ::fwRenderOgre::IAdaptor::dynamicCast(service);
        }

        SLM_ASSERT("Not a 'config' configuration", adaptee.m_config->getName() == "config");
        SLM_ASSERT("The SceneAdptor's associated service is not instantiated", adaptee.getService());

        adaptee.getService()->setConfiguration(adaptee.m_config);
        adaptee.getService()->configure();
        adaptee.getService()->setRenderService(SRender::dynamicCast(this->shared_from_this()));
        adaptee.getService()->setName(id);

        if (m_startAdaptor)
        {
            adaptee.getService()->start();
        }

        m_sceneAdaptors[id] = adaptee;
    }
    else if(m_sceneAdaptors.count(id) == 1)
    {
        SceneAdaptor& adaptee = m_sceneAdaptors[id];
        SLM_ASSERT("Adaptor service expired !", adaptee.getService() );
        OSLM_ASSERT( adaptee.getService()->getID() <<  " is not started ", adaptee.getService()->isStarted());
        if (object)
        {
            OSLM_TRACE ("Swapping IAdaptor " << adaptor << " on "<< objectId );
            if(adaptee.getService()->getObject() != object)
            {
                adaptee.getService()->swap(::fwData::Object::constCast(object));
            }
            else
            {
                OSLM_WARN(adaptor << "'s object already is '"
                                  << adaptee.getService()->getObject()->getID()
                                  << "', no need to swap");
            }
        }
        else
        {
            adaptee.getService()->stop();
            ::fwServices::OSR::unregisterService(adaptee.getService());
            adaptee.m_service.reset();
            m_sceneAdaptors.erase(id);
        }
    }
    else
    {
        OSLM_ERROR("'" << objectId << "' inexistent, passing by '" << adaptor << "'");
    }
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


    // Instantiate ogre object, class...
    if (this->isStarted())
    {
        std::vector< WPTR(IAdaptor) > startAdaptors;

        for(auto& sceneAdaptor : m_sceneAdaptors)
        {
            startAdaptors.emplace_back(sceneAdaptor.second.getService());
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

    // Configure connections
    for (auto iter : * m_sceneConfiguration)
    {
        if(iter->getName() == "connect")
        {
            if(iter->hasAttribute("waitForKey"))
            {
                std::string key = iter->getAttributeValue("waitForKey");

                if(this->isVersion2())
                {
                    auto object = this->getInput< ::fwData::Object >(key);
                    if(object)
                    {
                        this->manageConnection(key, object, iter);
                    }
                }
                else
                {
                    ::fwData::Composite::sptr composite               = this->getComposite();
                    ::fwData::Composite::const_iterator iterComposite = composite->find(key);
                    if(iterComposite != composite->end())
                    {
                        this->manageConnection(key, iterComposite->second, iter);
                    }
                }
                m_connect.push_back(iter);
            }
            else
            {
                ::fwServices::helper::Config::createConnections(iter, m_connections);
            }
        }
        else if(iter->getName() == "proxy")
        {
            if(iter->hasAttribute("waitForKey"))
            {
                std::string key = iter->getAttributeValue("waitForKey");

                if(this->isVersion2())
                {
                    auto object = this->getInput< ::fwData::Object >(key);
                    if(object)
                    {
                        this->manageProxy(key, object, iter);
                    }
                }
                else
                {
                    ::fwData::Composite::sptr composite               = this->getComposite();
                    ::fwData::Composite::const_iterator iterComposite = composite->find(key);
                    if(iterComposite != composite->end())
                    {
                        this->manageProxy(key, iterComposite->second, iter);
                    }
                }
                m_proxies.push_back(iter);
            }
            else
            {
                ::fwServices::helper::Config::createProxy("self", iter, m_proxyMap);
            }
        }
    }
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

void SRender::render()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

bool SRender::isShownOnScreen()
{
    return this->getContainer()->isShownOnScreen();
}

//-----------------------------------------------------------------------------

void SRender::startContext()
{
    m_interactorManager = ::fwRenderOgre::IRenderWindowInteractorManager::createManager();
    m_interactorManager->setRenderService(this->getSptr());
    m_interactorManager->createContainer( this->getContainer(), m_showOverlay, m_renderOnDemand, m_fullscreen );
}

//-----------------------------------------------------------------------------

void SRender::stopContext()
{
    SLM_TRACE_FUNC();

    m_interactorManager->disconnectInteractor();
    m_interactorManager.reset();
}

//------------------------------------------------------------------------------

void SRender::configureObjects(::fwData::Composite::ContainerType objects)
{
    SLM_ASSERT("Scene configuration is not defined",  m_sceneConfiguration );
    SLM_ASSERT("This should not be called in a AppXml2 application",  !this->isVersion2() );

    for( ::fwData::Composite::ContainerType::value_type objectId : objects)
    {
        std::vector< ConfigurationType > confVec = m_sceneConfiguration->find("adaptor","objectId",objectId.first);
        for( ConfigurationType cfg :  confVec )
        {
            this->configureObject(cfg);
        }
    }
}

//------------------------------------------------------------------------------

void SRender::addObjects(::fwData::Composite::ContainerType objects)
{
    this->configureObjects(objects);
    this->connectAfterWait(objects);
}

//------------------------------------------------------------------------------

void SRender::changeObjects(::fwData::Composite::ContainerType newObjects,
                            ::fwData::Composite::ContainerType oldObjects)
{
    this->disconnect(oldObjects);
    this->configureObjects(newObjects);
    this->connectAfterWait(newObjects);
}

//------------------------------------------------------------------------------

void SRender::removeObjects(::fwData::Composite::ContainerType objects)
{
    this->disconnect(objects);
    this->configureObjects(objects);
}

//-----------------------------------------------------------------------------

SPTR (IAdaptor) SRender::getAdaptor(SRender::AdaptorIdType adaptorId)
{
    IAdaptor::sptr adaptor;
    SceneAdaptorsMapType::iterator it = m_sceneAdaptors.find(adaptorId);

    OSLM_WARN_IF("adaptor '" << adaptorId << "' not found", it == m_sceneAdaptors.end());

    if ( it != m_sceneAdaptors.end() )
    {
        adaptor = it->second.getService();
    }

    return adaptor;
}

//-----------------------------------------------------------------------------

std::vector<CSPTR (IAdaptor)> fwRenderOgre::SRender::getAdaptors() const
{
    std::vector<CSPTR(IAdaptor)> adaptors;
    for(const auto& adaptor : m_sceneAdaptors)
    {
        adaptors.push_back(adaptor.second.getService());
    }
    return adaptors;
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
    OSLM_ASSERT("Layer ID "<< sceneID <<" does not exist", m_layers.find(sceneID) !=  m_layers.end());

    ::fwRenderOgre::Layer::sptr layer = m_layers.at(sceneID);

    return layer;
}

// ----------------------------------------------------------------------------

::fwRenderOgre::SRender::LayerMapType SRender::getLayers()
{
    return m_layers;
}

// ----------------------------------------------------------------------------

::fwRenderOgre::IRenderWindowInteractorManager::sptr SRender::getInteractorManager()
{
    return m_interactorManager;
}

//-----------------------------------------------------------------------------

void SRender::connectAfterWait(::fwData::Composite::ContainerType objects)
{

    for(::fwData::Composite::value_type element : objects)
    {
        for(::fwRuntime::ConfigurationElement::sptr connect : m_connect)
        {
            this->manageConnection(element.first, element.second, connect);
        }
        for(::fwRuntime::ConfigurationElement::sptr proxy : m_proxies)
        {
            this->manageProxy(element.first, element.second, proxy);
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::manageConnection(const std::string& key, const ::fwData::Object::csptr& obj,
                               const ConfigurationType& config)
{
    if(config->hasAttribute("waitForKey"))
    {
        std::string waitForKey = config->getAttributeValue("waitForKey");
        if(waitForKey == key)
        {
            ::fwServices::helper::Config::createConnections(config, m_objectConnections[key], obj);
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::manageProxy(const std::string& key, const ::fwData::Object::csptr& obj,
                          const ConfigurationType& config)
{
    if(config->hasAttribute("waitForKey"))
    {
        std::string waitForKey = config->getAttributeValue("waitForKey");
        if(waitForKey == key)
        {
            ::fwServices::helper::Config::createProxy(key, config, m_proxyMap, obj);
        }
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SRender::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT ) );


    return connections;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SRender::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_DEFAULT_OBJECT, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT );
    connections.push( s_DEFAULT_OBJECT, ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT );
    connections.push( s_DEFAULT_OBJECT, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr SRender::getComposite()
{
    if(this->isVersion2())
    {
        return this->getInOut< ::fwData::Composite >(s_DEFAULT_OBJECT);
    }
    else
    {
        return this->getObject< ::fwData::Composite >();
    }
}

// ----------------------------------------------------------------------------

} //namespace fwRenderOgre
