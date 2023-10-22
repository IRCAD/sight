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
#include "viz/scene3d/utils.hpp"

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
    m_ogreRoot(viz::scene3d::utils::getOgreRoot())
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
    const config_t scene_cfg = config.get_child("scene");

    const std::size_t nb_inouts = config.count("inout");
    SIGHT_ASSERT("This service accepts at most one inout.", nb_inouts <= 1);

    if(nb_inouts == 1)
    {
        const auto key = config.get<std::string>("inout.<xmlattr>.key", "");
        m_offScreen = (key == s_OFFSCREEN_INOUT);

        SIGHT_ASSERT("'" + key + "' is not a valid key. Only '" << s_OFFSCREEN_INOUT << "' is accepted.", m_offScreen);

        m_width  = scene_cfg.get<unsigned int>("<xmlattr>.width", m_width);
        m_height = scene_cfg.get<unsigned int>("<xmlattr>.height", m_height);
        m_flip   = scene_cfg.get<bool>("<xmlattr>.flip", m_flip);
    }
    else // no offscreen rendering.
    {
        this->initialize();
    }

    m_fullscreen = scene_cfg.get<bool>("<xmlattr>.fullscreen", false);

    const auto render_mode = scene_cfg.get<std::string>("<xmlattr>.renderMode", "auto");
    if(render_mode == "auto")
    {
        m_renderMode = RenderMode::AUTO;
    }
    else if(render_mode == "manual" || render_mode == "sync") /* Keep sync for backwards compatibility */
    {
        m_renderMode = RenderMode::MANUAL;
    }
    else
    {
        SIGHT_ERROR("Unknown rendering mode '" + render_mode + "', use the default 'auto'.");
    }

    auto& adaptor_registry = viz::scene3d::registry::get_adaptor_registry();

    /// New config
    auto layer_configs = scene_cfg.equal_range("layer");
    for(const auto& it_layer : boost::make_iterator_range(layer_configs))
    {
        const auto layer_id  = it_layer.second.get<std::string>("<xmlattr>.id");
        auto adaptor_configs = it_layer.second.equal_range("adaptor");
        for(const auto& it : boost::make_iterator_range(adaptor_configs))
        {
            const auto uid = it.second.get<std::string>("<xmlattr>.uid");
            adaptor_registry[uid] = {this->get_id(), layer_id};
        }

        //create resetcamera_layerID slot
        const core::com::slots::key_t resetcamera_slotkey = "resetCamera_" + layer_id;
        auto reset_camera_layer_slot                      = new_slot(
            resetcamera_slotkey,
            [this, layer_id]()
            {
                this->resetCameraCoordinates(layer_id);
            });

        reset_camera_layer_slot->set_worker(sight::core::thread::get_default_worker());
    }

    /// Old config
    auto adaptor_configs = scene_cfg.equal_range("adaptor");
    for(const auto& it : boost::make_iterator_range(adaptor_configs))
    {
        const auto uid = it.second.get<std::string>("<xmlattr>.uid");
        adaptor_registry[uid] = {this->get_id(), ""};
    }
}

//-----------------------------------------------------------------------------

void render::starting()
{
    bool b_has_background = false;

    if(!m_offScreen)
    {
        this->create();
    }

    const config_t config = this->get_config();

    SIGHT_ERROR_IF("Only one scene must be configured.", config.count("scene") != 1);

    const config_t scene_cfg = config.get_child("scene");

    SIGHT_ERROR_IF("Overlays should be enabled at the layer level.", !scene_cfg.get("<xmlattr>.overlays", "").empty());

    auto layer_configs = scene_cfg.equal_range("layer");
    for(auto it = layer_configs.first ; it != layer_configs.second ; ++it)
    {
        this->configureLayer(it->second);
    }

    auto bkg_configs = scene_cfg.equal_range("background");
    for(auto it = bkg_configs.first ; it != bkg_configs.second ; ++it)
    {
        SIGHT_ERROR_IF("A background has already been set, overriding it...", b_has_background);
        try
        {
            this->configureBackgroundLayer(it->second);
        }
        catch(std::exception& e)
        {
            SIGHT_ERROR("Error configuring background for layer '" + this->get_id() + "': " + e.what());
        }

        b_has_background = true;
    }

    if(!b_has_background)
    {
        // Create a default black background
        viz::scene3d::layer::sptr ogre_layer = std::make_shared<viz::scene3d::layer>();
        ogre_layer->setRenderService(std::dynamic_pointer_cast<viz::scene3d::render>(this->shared_from_this()));
        ogre_layer->set_id("backgroundLayer");
        ogre_layer->setOrder(0);
        ogre_layer->set_worker(this->worker());
        ogre_layer->setBackgroundColor("#000000", "#000000");
        ogre_layer->setBackgroundScale(0, 0.5);
        ogre_layer->setHasDefaultLight(false);
        ogre_layer->setViewportConfig({0.F, 0.F, 1.F, 1.F});

        m_layers[s_OGREBACKGROUNDID] = ogre_layer;
    }

    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);
    if(m_offScreen)
    {
        // Instantiate the manager that help to communicate between this service and the widget
        m_interactorManager = viz::scene3d::window_interactor::createOffscreenManager(m_width, m_height);
        m_interactorManager->setRenderService(this->get_sptr());
        m_interactorManager->createContainer(nullptr, m_fullscreen, service_id);
    }
    else
    {
        // Instantiate the manager that help to communicate between this service and the widget
        m_interactorManager = viz::scene3d::window_interactor::createManager();
        m_interactorManager->setRenderService(this->get_sptr());
        m_interactorManager->createContainer(this->getContainer(), m_fullscreen, service_id);
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
        viz::scene3d::layer::sptr layer = it.second;
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
    const config_t attributes                = _cfg.get_child("<xmlattr>");
    const std::string id                     = attributes.get<std::string>("id", "");
    const std::string compositors            = attributes.get<std::string>("compositors", "");
    const std::string transparency_technique = attributes.get<std::string>("transparency", "");
    const std::string num_peels              = attributes.get<std::string>("numPeels", "4");
    const std::string stereo_mode            = attributes.get<std::string>("stereoMode", "");
    const std::string default_light          = attributes.get<std::string>("defaultLight", "");
    const auto viewport_config               = configureLayerViewport(_cfg);

    auto overlays = std::istringstream(attributes.get<std::string>("overlays", ""));
    std::vector<std::string> enabled_overlays;
    for(std::string overlay_name ; std::getline(overlays, overlay_name, ';') ; )
    {
        enabled_overlays.push_back(overlay_name);
    }

    SIGHT_ASSERT("'id' required attribute missing or empty", !id.empty());
    SIGHT_ASSERT(
        "Unknown 3D mode : " << stereo_mode,
        stereo_mode.empty() || stereo_mode == "false" || stereo_mode == "AutoStereo5" || stereo_mode == "AutoStereo8"
        || stereo_mode == "Stereo"
    );

    int layer_order        = 0;
    const auto layer_depth = attributes.get_optional<int>("depth");
    if(layer_depth)
    {
        FW_DEPRECATED_MSG("Attribute 'depth' is deprecated, please used 'order' instead", "22.0")
        layer_order = layer_depth.get();
    }
    else
    {
        layer_order = attributes.get<int>("order");
    }

    SIGHT_ASSERT("Attribute 'order' must be greater than 0", layer_order > 0);

    viz::scene3d::layer::sptr ogre_layer              = std::make_shared<viz::scene3d::layer>();
    compositor::core::stereo_mode_t layer_stereo_mode = stereo_mode == "AutoStereo5"
                                                        ? compositor::core::stereo_mode_t::AUTOSTEREO_5
                                                        : stereo_mode == "AutoStereo8"
                                                        ? compositor::core::stereo_mode_t::AUTOSTEREO_8
                                                        : stereo_mode == "Stereo"
                                                        ? compositor::core::stereo_mode_t::STEREO
                                                        : compositor::core::stereo_mode_t::NONE;

    ogre_layer->setRenderService(std::dynamic_pointer_cast<viz::scene3d::render>(this->shared_from_this()));
    ogre_layer->set_id(id);
    ogre_layer->setOrder(layer_order);
    ogre_layer->set_worker(this->worker());
    ogre_layer->setCoreCompositorEnabled(true, transparency_technique, num_peels, layer_stereo_mode);
    ogre_layer->setCompositorChainEnabled(compositors);
    ogre_layer->setViewportConfig(viewport_config);

    if(!default_light.empty() && default_light == "false")
    {
        ogre_layer->setHasDefaultLight(false);
    }

    // Finally, the layer is pushed in the map
    m_layers[id] = ogre_layer;
}

//-----------------------------------------------------------------------------

void render::configureBackgroundLayer(const config_t& _cfg)
{
    SIGHT_ASSERT("'id' required attribute missing or empty", !this->get_id().empty());
    const config_t attributes = _cfg.get_child("<xmlattr>");

    viz::scene3d::layer::sptr ogre_layer = std::make_shared<viz::scene3d::layer>();
    ogre_layer->setRenderService(std::dynamic_pointer_cast<viz::scene3d::render>(this->shared_from_this()));
    ogre_layer->set_id(s_OGREBACKGROUNDID);
    ogre_layer->setOrder(0);
    ogre_layer->set_worker(this->worker());
    ogre_layer->setHasDefaultLight(false);

    if(attributes.count("material") != 0U)
    {
        const auto material = attributes.get<std::string>("material");
        ogre_layer->setBackgroundMaterial(material);
    }
    else if(attributes.count("color") != 0U)
    {
        const auto color = attributes.get<std::string>("color");
        ogre_layer->setBackgroundColor(color, color);
    }
    else if((attributes.count("topColor") != 0U) && (attributes.count("bottomColor") != 0U))
    {
        const auto top_color = attributes.get<std::string>("topColor");
        const auto bot_color = attributes.get<std::string>("bottomColor");

        ogre_layer->setBackgroundColor(top_color, bot_color);
    }

    if((attributes.count("topScale") != 0U) && (attributes.count("bottomScale") != 0U))
    {
        const auto top_scale_val = attributes.get<float>("topScale");
        const auto bot_scale_val = attributes.get<float>("bottomScale");

        ogre_layer->setBackgroundScale(top_scale_val, bot_scale_val);
    }
    else
    {
        ogre_layer->setBackgroundScale(0.5F, 0.5F);
    }

    m_layers[s_OGREBACKGROUNDID] = ogre_layer;
}

//-----------------------------------------------------------------------------

layer::viewport_config_t render::configureLayerViewport(const service::config_t& _cfg)
{
    layer::viewport_config_t cfg_type {0.F, 0.F, 1.F, 1.F};
    const auto vp_config = _cfg.get_child_optional("viewport.<xmlattr>");
    if(vp_config.has_value())
    {
        const auto& cfg = vp_config.get();

        float x_pos = cfg.get<float>("hOffset", 0.F);
        float y_pos = cfg.get<float>("vOffset", 0.F);

        const auto width  = cfg.get<float>("width");
        const auto height = cfg.get<float>("height");

        const std::map<std::string, float> horiz_align_to_x {
            {"left", x_pos},
            {"center", 0.5F - width * 0.5F + x_pos},
            {"right", 1.F - width - x_pos}
        };

        const std::map<std::string, float> vert_align_to_y {
            {"bottom", 1.F - height - y_pos},
            {"center", 0.5F - height * 0.5F + y_pos},
            {"top", y_pos}
        };

        const std::string h_align = cfg.get("hAlign", "left");
        const std::string v_align = cfg.get("vAlign", "top");

        x_pos = horiz_align_to_x.at(h_align);
        y_pos = vert_align_to_y.at(v_align);

        cfg_type = std::tie(x_pos, y_pos, width, height);
    }

    return cfg_type;
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
            Ogre::TexturePtr render_texture = m_interactorManager->getRenderTexture();
            SIGHT_ASSERT("The offscreen window doesn't write to a texture", render_texture);
            viz::scene3d::utils::convertFromOgreTexture(render_texture, image.get_shared(), m_flip);
        }

        auto sig = image->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void render::resetCameraCoordinates(const std::string& _layer_id)
{
    auto layer = m_layers.find(_layer_id);

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

Ogre::SceneManager* render::getSceneManager(const std::string& _scene_id)
{
    viz::scene3d::layer::sptr layer = this->getLayer(_scene_id);
    return layer->getSceneManager();
}

// ----------------------------------------------------------------------------

viz::scene3d::layer::sptr render::getLayer(const std::string& _scene_id)
{
    SIGHT_ASSERT("Empty sceneID", !_scene_id.empty());
    SIGHT_ASSERT("layer ID " << _scene_id << " does not exist", m_layers.find(_scene_id) != m_layers.end());

    viz::scene3d::layer::sptr layer = m_layers.at(_scene_id);

    return layer;
}

// ----------------------------------------------------------------------------

viz::scene3d::render::layer_map_t render::getLayers()
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
