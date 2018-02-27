/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/SRender.hpp"

#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/OffScreenRenderWindowInteractorManager.hpp"
#include "fwRenderOgre/registry/Adaptor.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRuntime/ConfigurationElementContainer.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

#include <stack>

fwServicesRegisterMacro( ::fwRender::IRender, ::fwRenderOgre::SRender, ::fwData::Composite );

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

const std::string SRender::s_OGREBACKGROUNDID = "ogreBackground";

//-----------------------------------------------------------------------------

static const ::fwServices::IService::KeyType s_OFFSCREEN_INOUT = "offScreen";

//-----------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SRender::s_COMPOSITOR_UPDATED_SIG = "compositorUpdated";

//-----------------------------------------------------------------------------

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
    m_renderOnDemand(true),
    m_fullscreen(false)
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();

    newSignal<CompositorUpdatedSignalType>(s_COMPOSITOR_UPDATED_SIG);

    newSlot(s_COMPUTE_CAMERA_ORIG_SLOT, &SRender::resetCameraCoordinates, this);
    newSlot(s_COMPUTE_CAMERA_CLIPPING_SLOT, &SRender::computeCameraClipping, this);
    newSlot(s_DO_RAY_CAST_SLOT, &SRender::doRayCast, this);
    newSlot(s_REQUEST_RENDER_SLOT, &SRender::requestRender, this);
}

//-----------------------------------------------------------------------------

SRender::~SRender() noexcept
{
    m_ogreRoot = nullptr;
}

//-----------------------------------------------------------------------------

void SRender::configuring()
{
    const ConfigType config = this->getConfigTree();

    SLM_ERROR_IF("Only one scene must be configured.", config.count("scene") != 1);
    const ConfigType sceneCfg = config.get_child("scene");

    const size_t nbInouts = config.count("inout");
    SLM_ASSERT("This service accepts at most one inout.", nbInouts <= 1);

    if(nbInouts == 1)
    {
        const std::string key = config.get<std::string>("inout.<xmlattr>.key", "");
        m_offScreen = (key == s_OFFSCREEN_INOUT);

        SLM_ASSERT("'" + key + "' is not a valid key. Only '" + s_OFFSCREEN_INOUT +"' is accepted.", m_offScreen);

        m_width  = sceneCfg.get<unsigned int>("<xmlattr>.width", m_width);
        m_height = sceneCfg.get<unsigned int>("<xmlattr>.height", m_height);

    }
    else // no offscreen rendering.
    {
        this->initialize();
    }

    m_showOverlay = sceneCfg.get<bool>("<xmlattr>.overlay", false);
    m_fullscreen  = sceneCfg.get<bool>("<xmlattr>.fullscreen", false);

    const std::string renderMode = sceneCfg.get<std::string>("<xmlattr>.renderMode", "auto");
    if (renderMode == "auto")
    {
        m_renderOnDemand = true;
    }
    else if (renderMode == "always")
    {
        m_renderOnDemand = false;
    }

    auto adaptorConfigs = sceneCfg.equal_range("adaptor");
    for( auto it = adaptorConfigs.first; it != adaptorConfigs.second; ++it )
    {
        const std::string uid = it->second.get<std::string>("<xmlattr>.uid");
        auto& registry        = ::fwRenderOgre::registry::getAdaptorRegistry();
        registry[uid] = this->getID();
    }
}

//-----------------------------------------------------------------------------

void SRender::starting()
{
    SLM_TRACE_FUNC();

    bool bHasBackground = false;

    if (!m_offScreen)
    {
        this->create();

    }
    const ConfigType config = this->getConfigTree();

    SLM_ERROR_IF("Only one scene must be configured.", config.count("scene") != 1);

    const ConfigType sceneCfg = config.get_child("scene");

    auto layerConfigs = sceneCfg.equal_range("layer");
    for( auto it = layerConfigs.first; it != layerConfigs.second; ++it )
    {
        this->configureLayer(it->second);
    }
    auto bkgConfigs = sceneCfg.equal_range("background");
    for( auto it = bkgConfigs.first; it != bkgConfigs.second; ++it )
    {
        OSLM_ERROR_IF("A background has already been set, overriding it...", bHasBackground);
        try
        {
            this->configureBackgroundLayer(it->second);
        }
        catch (std::exception& e)
        {
            OSLM_ERROR("Error configuring background for layer '" + this->getID() + "': " + e.what());
        }

        bHasBackground = true;
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

    if(m_offScreen)
    {
        // Instantiate the manager that help to communicate between this service and the widget
        m_interactorManager = ::fwRenderOgre::OffScreenRenderWindowInteractorManager::New(m_width, m_height);
        m_interactorManager->setRenderService(this->getSptr());
        m_interactorManager->createContainer( nullptr, m_showOverlay, m_renderOnDemand, m_fullscreen );

    }
    else
    {
        // Instantiate the manager that help to communicate between this service and the widget
        m_interactorManager = ::fwRenderOgre::IRenderWindowInteractorManager::createManager();
        m_interactorManager->setRenderService(this->getSptr());
        m_interactorManager->createContainer( this->getContainer(), m_showOverlay, m_renderOnDemand, m_fullscreen );
    }
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    for (auto it : m_layers)
    {
        ::fwRenderOgre::Layer::sptr layer = it.second;
        layer->setRenderTarget(m_interactorManager->getRenderTarget());
        layer->createScene();
    }

    // Everything is started now, we can safely create connections and thus receive interactions from the widget
    m_interactorManager->connectToContainer();
}

//-----------------------------------------------------------------------------

void SRender::stopping()
{
    m_connections.disconnect();

    for (auto it : m_layers)
    {
        ::fwRenderOgre::Layer::sptr layer = it.second;
        layer->destroyScene();
    }
    m_layers.clear();

    m_interactorManager->disconnectInteractor();
    m_interactorManager.reset();

    if(!m_offScreen)
    {
        this->destroy();
    }
}

//-----------------------------------------------------------------------------

void SRender::updating()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SRender::makeCurrent()
{
    m_interactorManager->makeCurrent();
}

//-----------------------------------------------------------------------------

void SRender::configureLayer(const ConfigType& _cfg )
{
    const ConfigType attributes             = _cfg.get_child("<xmlattr>");
    const std::string id                    = attributes.get<std::string>("id", "");
    const std::string compositors           = attributes.get<std::string>("compositors", "");
    const std::string transparencyTechnique = attributes.get<std::string>("transparency", "");
    const std::string numPeels              = attributes.get<std::string>("numPeels", "");
    const std::string stereoMode            = attributes.get<std::string>("stereoMode", "");
    const std::string defaultLight          = attributes.get<std::string>("defaultLight", "");

    SLM_ASSERT( "'id' required attribute missing or empty", !id.empty() );
    SLM_ASSERT( "Unknown 3D mode : " << stereoMode,
                stereoMode.empty() || stereoMode == "no" || stereoMode == "AutoStereo5" || stereoMode == "AutoStereo8" ||
                stereoMode == "Stereo");

    const int layerDepth = attributes.get<int>("depth");
    SLM_ASSERT("Attribute 'depth' must be greater than 0", layerDepth > 0);

    ::fwRenderOgre::Layer::sptr ogreLayer = ::fwRenderOgre::Layer::New();
    ogreLayer->setRenderService(::fwRenderOgre::SRender::dynamicCast(this->shared_from_this()));
    ogreLayer->setID(id);
    ogreLayer->setDepth(layerDepth);
    ogreLayer->setWorker(m_associatedWorker);
    ogreLayer->setStereoMode(stereoMode == "AutoStereo5" ? ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_5 :
                             stereoMode == "AutoStereo8" ? ::fwRenderOgre::Layer::StereoModeType::AUTOSTEREO_8 :
                             stereoMode == "Stereo" ? ::fwRenderOgre::Layer::StereoModeType::STEREO :
                             ::fwRenderOgre::Layer::StereoModeType::NONE);

    ogreLayer->setCoreCompositorEnabled(true, transparencyTechnique, numPeels);
    ogreLayer->setCompositorChainEnabled(compositors);

    if(!defaultLight.empty() && defaultLight == "no")
    {
        ogreLayer->setHasDefaultLight(false);
    }

    // Finally, the layer is pushed in the map
    m_layers[id] = ogreLayer;
}

//-----------------------------------------------------------------------------

void SRender::configureBackgroundLayer(const ConfigType& _cfg )
{
    SLM_ASSERT( "'id' required attribute missing or empty", !this->getID().empty() );
    const ConfigType attributes = _cfg.get_child("<xmlattr>");

    ::fwRenderOgre::Layer::sptr ogreLayer = ::fwRenderOgre::Layer::New();
    ogreLayer->setRenderService(::fwRenderOgre::SRender::dynamicCast(this->shared_from_this()));
    ogreLayer->setID(s_OGREBACKGROUNDID);
    ogreLayer->setDepth(0);
    ogreLayer->setWorker(m_associatedWorker);
    ogreLayer->setHasDefaultLight(false);

    if (attributes.count("topColor") && attributes.count("bottomColor"))
    {
        std::string topColor = attributes.get<std::string>("topColor");
        std::string botColor = attributes.get<std::string>("bottomColor");

        ogreLayer->setBackgroundColor(topColor, botColor);
    }

    if (attributes.count("topScale") && attributes.count("bottomScale"))
    {
        const float topScaleVal = attributes.get<float>("topScale");
        const float botScaleVal = attributes.get<float>("bottomScale");

        ogreLayer->setBackgroundScale(topScaleVal, botScaleVal);
    }

    m_layers[s_OGREBACKGROUNDID] = ogreLayer;
}

//-----------------------------------------------------------------------------

void SRender::doRayCast(int x, int y, int width, int height)
{
    for (auto it : m_layers)
    {
        ::fwRenderOgre::Layer::sptr layer = it.second;
        if(layer->doRayCast(x, y, width, height))
        {
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void SRender::requestRender()
{
    if( this->isShownOnScreen() )
    {
        m_interactorManager->requestRender();

        if(m_offScreen)
        {
            ::Ogre::RenderTarget* renderTarget = m_interactorManager->getRenderTarget();
            SLM_ASSERT("No render target found.", renderTarget);

            ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_OFFSCREEN_INOUT);
            SLM_ASSERT("Offscreen image not found.", image);

            auto offScreenInteractor = OffScreenRenderWindowInteractorManager::dynamicCast(m_interactorManager);
            {
                ::fwData::mt::ObjectWriteLock lock(image);
                ::fwRenderOgre::Utils::convertFromOgreTexture(offScreenInteractor->getRenderTexture(), image);
            }

            auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
        }
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
    if( m_offScreen )
    {
        return true;
    }
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
