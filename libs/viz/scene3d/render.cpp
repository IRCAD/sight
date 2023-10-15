/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/render.hpp"

#include "viz/scene3d/adaptor.hpp"
#include "viz/scene3d/registry/adaptor.hpp"
#include "viz/scene3d/Utils.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <data/composite.hpp>

#define FW_PROFILING_DISABLED
#include <core/profiling.hpp>

#include <core/runtime/utils/generic_executable_factory_registry.hpp>

#include <service/macros.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreNode.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/Overlay/OgreOverlayManager.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>

#include <stack>

SIGHT_REGISTER_SERVICE(sight::viz::render, sight::viz::scene3d::render, sight::data::composite);

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

const std::string render::s_OGREBACKGROUNDID = "ogreBackground";

//-----------------------------------------------------------------------------

const core::com::slots::key_t render::COMPUTE_CAMERA_ORIG_SLOT = "computeCameraParameters";
const core::com::slots::key_t render::RESET_CAMERAS_SLOT       = "resetCameras";
const core::com::slots::key_t render::REQUEST_RENDER_SLOT      = "requestRender";
const core::com::slots::key_t render::DISABLE_FULLSCREEN       = "disableFullscreen";
const core::com::slots::key_t render::ENABLE_FULLSCREEN        = "enableFullscreen";
const core::com::slots::key_t render::SET_MANUAL_MODE          = "setManualMode";
const core::com::slots::key_t render::SET_AUTO_MODE            = "setAutoMode";

//-----------------------------------------------------------------------------

render::render() noexcept :
    m_ogreRoot(viz::scene3d::Utils::getOgreRoot())
{
    new_signal<signals::compositorUpdated_signal_t>(signals::COMPOSITOR_UPDATED);
    new_signal<signals::void_signal_t>(signals::FULLSCREEN_SET);
    new_signal<signals::void_signal_t>(signals::FULLSCREEN_UNSET);

    new_slot(COMPUTE_CAMERA_ORIG_SLOT, &render::resetCameraCoordinates, this);
    new_slot(RESET_CAMERAS_SLOT, &render::resetCameras, this);
    new_slot(REQUEST_RENDER_SLOT, &render::requestRender, this);
    new_slot(DISABLE_FULLSCREEN, &render::disableFullscreen, this);
    new_slot(ENABLE_FULLSCREEN, &render::enableFullscreen, this);
    new_slot(SET_MANUAL_MODE, [this](){this->m_renderMode = RenderMode::MANUAL;});
    new_slot(SET_AUTO_MODE, [this](){this->m_renderMode = RenderMode::AUTO;});
}

//-----------------------------------------------------------------------------

render::~render() noexcept
{
    m_ogreRoot = nullptr;
}

//-----------------------------------------------------------------------------

void render::configuring()
{
    const config_t config = this->get_config();

    SIGHT_ERROR_IF("Only one scene must be configured.", config.count("scene") != 1);
    const config_t sceneCfg = config.get_child("scene");

    const std::size_t nbInouts = config.count("inout");
    SIGHT_ASSERT("This service accepts at most one inout.", nbInouts <= 1);

    if(nbInouts == 1)
    {
        const auto key = config.get<std::string>("inout.<xmlattr>.key", "");
        m_offScreen = (key == s_OFFSCREEN_INOUT);

        SIGHT_ASSERT("'" + key + "' is not a valid key. Only '" << s_OFFSCREEN_INOUT << "' is accepted.", m_offScreen);

        m_width  = sceneCfg.get<unsigned int>("<xmlattr>.width", m_width);
        m_height = sceneCfg.get<unsigned int>("<xmlattr>.height", m_height);
        m_flip   = sceneCfg.get<bool>("<xmlattr>.flip", m_flip);
    }
    else // no offscreen rendering.
    {
        this->initialize();
    }

    m_fullscreen = sceneCfg.get<bool>("<xmlattr>.fullscreen", false);

    const auto renderMode = sceneCfg.get<std::string>("<xmlattr>.renderMode", "auto");
    if(renderMode == "auto")
    {
        m_renderMode = RenderMode::AUTO;
    }
    else if(renderMode == "manual" || renderMode == "sync") /* Keep sync for backwards compatibility */
    {
        m_renderMode = RenderMode::MANUAL;
    }
    else
    {
        SIGHT_ERROR("Unknown rendering mode '" + renderMode + "', use the default 'auto'.");
    }

    auto& adaptorRegistry = viz::scene3d::registry::get_adaptor_registry();

    /// New config
    auto layerConfigs = sceneCfg.equal_range("layer");
    for(const auto& itLayer : boost::make_iterator_range(layerConfigs))
    {
        const auto layerId  = itLayer.second.get<std::string>("<xmlattr>.id");
        auto adaptorConfigs = itLayer.second.equal_range("adaptor");
        for(const auto& it : boost::make_iterator_range(adaptorConfigs))
        {
            const auto uid = it.second.get<std::string>("<xmlattr>.uid");
            adaptorRegistry[uid] = {this->get_id(), layerId};
        }

        //create resetcamera_layerID slot
        const core::com::slots::key_t resetcamera_slotkey = "resetCamera_" + layerId;
        auto resetCameraLayerSlot                         = new_slot(
            resetcamera_slotkey,
            [this, layerId]()
            {
                this->resetCameraCoordinates(layerId);
            });

        resetCameraLayerSlot->set_worker(sight::core::thread::get_default_worker());
    }

    /// Old config
    auto adaptorConfigs = sceneCfg.equal_range("adaptor");
    for(const auto& it : boost::make_iterator_range(adaptorConfigs))
    {
        const auto uid = it.second.get<std::string>("<xmlattr>.uid");
        adaptorRegistry[uid] = {this->get_id(), ""};
    }
}

//-----------------------------------------------------------------------------

void render::starting()
{
    bool bHasBackground = false;

    if(!m_offScreen)
    {
        this->create();
    }

    const config_t config = this->get_config();

    SIGHT_ERROR_IF("Only one scene must be configured.", config.count("scene") != 1);

    const config_t sceneCfg = config.get_child("scene");

    SIGHT_ERROR_IF("Overlays should be enabled at the layer level.", !sceneCfg.get("<xmlattr>.overlays", "").empty());

    auto layerConfigs = sceneCfg.equal_range("layer");
    for(auto it = layerConfigs.first ; it != layerConfigs.second ; ++it)
    {
        this->configureLayer(it->second);
    }

    auto bkgConfigs = sceneCfg.equal_range("background");
    for(auto it = bkgConfigs.first ; it != bkgConfigs.second ; ++it)
    {
        SIGHT_ERROR_IF("A background has already been set, overriding it...", bHasBackground);
        try
        {
            this->configureBackgroundLayer(it->second);
        }
        catch(std::exception& e)
        {
            SIGHT_ERROR("Error configuring background for layer '" + this->get_id() + "': " + e.what());
        }

        bHasBackground = true;
    }

    if(!bHasBackground)
    {
        // Create a default black background
        viz::scene3d::Layer::sptr ogreLayer = std::make_shared<viz::scene3d::Layer>();
        ogreLayer->setRenderService(std::dynamic_pointer_cast<viz::scene3d::render>(this->shared_from_this()));
        ogreLayer->set_id("backgroundLayer");
        ogreLayer->setOrder(0);
        ogreLayer->set_worker(this->worker());
        ogreLayer->setBackgroundColor("#000000", "#000000");
        ogreLayer->setBackgroundScale(0, 0.5);
        ogreLayer->setHasDefaultLight(false);
        ogreLayer->setViewportConfig({0.F, 0.F, 1.F, 1.F});

        m_layers[s_OGREBACKGROUNDID] = ogreLayer;
    }

    const std::string serviceID = get_id().substr(get_id().find_last_of('_') + 1);
    if(m_offScreen)
    {
        // Instantiate the manager that help to communicate between this service and the widget
        m_interactorManager = viz::scene3d::window_interactor::createOffscreenManager(m_width, m_height);
        m_interactorManager->setRenderService(this->get_sptr());
        m_interactorManager->createContainer(nullptr, m_fullscreen, serviceID);
    }
    else
    {
        // Instantiate the manager that help to communicate between this service and the widget
        m_interactorManager = viz::scene3d::window_interactor::createManager();
        m_interactorManager->setRenderService(this->get_sptr());
        m_interactorManager->createContainer(this->getContainer(), m_fullscreen, serviceID);
    }

    // Everything is started now, we can safely create connections and thus receive interactions from the widget
    m_interactorManager->connectToContainer();
}

//-----------------------------------------------------------------------------

void render::stopping()
{
    this->makeCurrent();

    for(const auto& it : m_layers)
    {
        viz::scene3d::Layer::sptr layer = it.second;
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

void render::updating()
{
}

//-----------------------------------------------------------------------------

void render::makeCurrent()
{
    m_interactorManager->makeCurrent();
}

//-----------------------------------------------------------------------------

void render::configureLayer(const config_t& _cfg)
{
    const config_t attributes               = _cfg.get_child("<xmlattr>");
    const std::string id                    = attributes.get<std::string>("id", "");
    const std::string compositors           = attributes.get<std::string>("compositors", "");
    const std::string transparencyTechnique = attributes.get<std::string>("transparency", "");
    const std::string numPeels              = attributes.get<std::string>("numPeels", "4");
    const std::string stereoMode            = attributes.get<std::string>("stereoMode", "");
    const std::string defaultLight          = attributes.get<std::string>("defaultLight", "");
    const auto viewportConfig               = configureLayerViewport(_cfg);

    auto overlays = std::istringstream(attributes.get<std::string>("overlays", ""));
    std::vector<std::string> enabledOverlays;
    for(std::string overlayName ; std::getline(overlays, overlayName, ';') ; )
    {
        enabledOverlays.push_back(overlayName);
    }

    SIGHT_ASSERT("'id' required attribute missing or empty", !id.empty());
    SIGHT_ASSERT(
        "Unknown 3D mode : " << stereoMode,
        stereoMode.empty() || stereoMode == "false" || stereoMode == "AutoStereo5" || stereoMode == "AutoStereo8"
        || stereoMode == "Stereo"
    );

    int layerOrder        = 0;
    const auto layerDepth = attributes.get_optional<int>("depth");
    if(layerDepth)
    {
        FW_DEPRECATED_MSG("Attribute 'depth' is deprecated, please used 'order' instead", "22.0")
        layerOrder = layerDepth.get();
    }
    else
    {
        layerOrder = attributes.get<int>("order");
    }

    SIGHT_ASSERT("Attribute 'order' must be greater than 0", layerOrder > 0);

    viz::scene3d::Layer::sptr ogreLayer              = std::make_shared<viz::scene3d::Layer>();
    compositor::core::StereoModeType layerStereoMode = stereoMode == "AutoStereo5"
                                                       ? compositor::core::StereoModeType::AUTOSTEREO_5
                                                       : stereoMode == "AutoStereo8"
                                                       ? compositor::core::StereoModeType::AUTOSTEREO_8
                                                       : stereoMode == "Stereo"
                                                       ? compositor::core::StereoModeType::STEREO
                                                       : compositor::core::StereoModeType::NONE;

    ogreLayer->setRenderService(std::dynamic_pointer_cast<viz::scene3d::render>(this->shared_from_this()));
    ogreLayer->set_id(id);
    ogreLayer->setOrder(layerOrder);
    ogreLayer->set_worker(this->worker());
    ogreLayer->setCoreCompositorEnabled(true, transparencyTechnique, numPeels, layerStereoMode);
    ogreLayer->setCompositorChainEnabled(compositors);
    ogreLayer->setViewportConfig(viewportConfig);

    if(!defaultLight.empty() && defaultLight == "false")
    {
        ogreLayer->setHasDefaultLight(false);
    }

    // Finally, the layer is pushed in the map
    m_layers[id] = ogreLayer;
}

//-----------------------------------------------------------------------------

void render::configureBackgroundLayer(const config_t& _cfg)
{
    SIGHT_ASSERT("'id' required attribute missing or empty", !this->get_id().empty());
    const config_t attributes = _cfg.get_child("<xmlattr>");

    viz::scene3d::Layer::sptr ogreLayer = std::make_shared<viz::scene3d::Layer>();
    ogreLayer->setRenderService(std::dynamic_pointer_cast<viz::scene3d::render>(this->shared_from_this()));
    ogreLayer->set_id(s_OGREBACKGROUNDID);
    ogreLayer->setOrder(0);
    ogreLayer->set_worker(this->worker());
    ogreLayer->setHasDefaultLight(false);

    if(attributes.count("material") != 0U)
    {
        const auto material = attributes.get<std::string>("material");
        ogreLayer->setBackgroundMaterial(material);
    }
    else if(attributes.count("color") != 0U)
    {
        const auto color = attributes.get<std::string>("color");
        ogreLayer->setBackgroundColor(color, color);
    }
    else if((attributes.count("topColor") != 0U) && (attributes.count("bottomColor") != 0U))
    {
        const auto topColor = attributes.get<std::string>("topColor");
        const auto botColor = attributes.get<std::string>("bottomColor");

        ogreLayer->setBackgroundColor(topColor, botColor);
    }

    if((attributes.count("topScale") != 0U) && (attributes.count("bottomScale") != 0U))
    {
        const auto topScaleVal = attributes.get<float>("topScale");
        const auto botScaleVal = attributes.get<float>("bottomScale");

        ogreLayer->setBackgroundScale(topScaleVal, botScaleVal);
    }
    else
    {
        ogreLayer->setBackgroundScale(0.5F, 0.5F);
    }

    m_layers[s_OGREBACKGROUNDID] = ogreLayer;
}

//-----------------------------------------------------------------------------

Layer::ViewportConfigType render::configureLayerViewport(const service::config_t& _cfg)
{
    Layer::ViewportConfigType cfgType {0.F, 0.F, 1.F, 1.F};
    const auto _vpConfig = _cfg.get_child_optional("viewport.<xmlattr>");
    if(_vpConfig.has_value())
    {
        const auto& cfg = _vpConfig.get();

        float xPos = cfg.get<float>("hOffset", 0.F);
        float yPos = cfg.get<float>("vOffset", 0.F);

        const auto width  = cfg.get<float>("width");
        const auto height = cfg.get<float>("height");

        const std::map<std::string, float> horizAlignToX {
            {"left", xPos},
            {"center", 0.5F - width * 0.5F + xPos},
            {"right", 1.F - width - xPos}
        };

        const std::map<std::string, float> vertAlignToY {
            {"bottom", 1.F - height - yPos},
            {"center", 0.5F - height * 0.5F + yPos},
            {"top", yPos}
        };

        const std::string hAlign = cfg.get("hAlign", "left");
        const std::string vAlign = cfg.get("vAlign", "top");

        xPos = horizAlignToX.at(hAlign);
        yPos = vertAlignToY.at(vAlign);

        cfgType = std::tie(xPos, yPos, width, height);
    }

    return cfgType;
}

//-----------------------------------------------------------------------------

void render::requestRender()
{
    if(m_renderMode == RenderMode::MANUAL)
    {
        m_interactorManager->renderNow();
    }
    else
    {
        m_interactorManager->requestRender();
    }

    if(m_offScreen)
    {
        FW_PROFILE("Offscreen rendering");

        const auto image = m_offScreenImage.lock();
        {
            this->makeCurrent();
            Ogre::TexturePtr renderTexture = m_interactorManager->getRenderTexture();
            SIGHT_ASSERT("The offscreen window doesn't write to a texture", renderTexture);
            viz::scene3d::Utils::convertFromOgreTexture(renderTexture, image.get_shared(), m_flip);
        }

        auto sig = image->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void render::resetCameraCoordinates(const std::string& _layerId)
{
    auto layer = m_layers.find(_layerId);

    if(layer != m_layers.end())
    {
        layer->second->resetCameraCoordinates();
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void render::resetCameras()
{
    for(const auto& layer : m_layers)
    {
        layer.second->resetCameraCoordinates();
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void render::paint()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

bool render::isShownOnScreen()
{
    return m_offScreen || m_fullscreen || this->getContainer()->isShownOnScreen();
}

// ----------------------------------------------------------------------------

Ogre::SceneManager* render::getSceneManager(const std::string& sceneID)
{
    viz::scene3d::Layer::sptr layer = this->getLayer(sceneID);
    return layer->getSceneManager();
}

// ----------------------------------------------------------------------------

viz::scene3d::Layer::sptr render::getLayer(const std::string& sceneID)
{
    SIGHT_ASSERT("Empty sceneID", !sceneID.empty());
    SIGHT_ASSERT("Layer ID " << sceneID << " does not exist", m_layers.find(sceneID) != m_layers.end());

    viz::scene3d::Layer::sptr layer = m_layers.at(sceneID);

    return layer;
}

// ----------------------------------------------------------------------------

viz::scene3d::render::LayerMapType render::getLayers()
{
    return m_layers;
}

// ----------------------------------------------------------------------------

viz::scene3d::window_interactor::sptr render::getInteractorManager() const
{
    return m_interactorManager;
}

// ----------------------------------------------------------------------------

void render::disableFullscreen()
{
    m_fullscreen = false;
    m_interactorManager->setFullscreen(m_fullscreen, -1);
    this->signal<signals::void_signal_t>(signals::FULLSCREEN_UNSET)->async_emit();
}

// ----------------------------------------------------------------------------

void render::enableFullscreen(int _screen)
{
    m_fullscreen = true;
    m_interactorManager->setFullscreen(m_fullscreen, _screen);
    this->signal<signals::void_signal_t>(signals::FULLSCREEN_SET)->async_emit();
}

// ----------------------------------------------------------------------------

} //namespace sight::viz::scene3d
