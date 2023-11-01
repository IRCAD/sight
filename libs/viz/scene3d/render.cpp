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

const std::string render::OGREBACKGROUNDID = "ogreBackground";

//-----------------------------------------------------------------------------

const core::com::slots::key_t render::COMPUTE_CAMERA_ORIG_SLOT = "computeCameraParameters";
const core::com::slots::key_t render::RESET_CAMERAS_SLOT       = "reset_cameras";
const core::com::slots::key_t render::REQUEST_RENDER_SLOT      = "request_render";
const core::com::slots::key_t render::DISABLE_FULLSCREEN       = "disable_fullscreen";
const core::com::slots::key_t render::ENABLE_FULLSCREEN        = "enable_fullscreen";
const core::com::slots::key_t render::SET_MANUAL_MODE          = "set_manual_mode";
const core::com::slots::key_t render::SET_AUTO_MODE            = "set_auto_mode";

//-----------------------------------------------------------------------------

render::render() noexcept :
    m_ogre_root(viz::scene3d::utils::get_ogre_root())
{
    new_signal<signals::compositor_updated_signal_t>(signals::COMPOSITOR_UPDATED);
    new_signal<signals::void_signal_t>(signals::FULLSCREEN_SET);
    new_signal<signals::void_signal_t>(signals::FULLSCREEN_UNSET);

    new_slot(COMPUTE_CAMERA_ORIG_SLOT, &render::reset_camera_coordinates, this);
    new_slot(RESET_CAMERAS_SLOT, &render::reset_cameras, this);
    new_slot(REQUEST_RENDER_SLOT, &render::request_render, this);
    new_slot(DISABLE_FULLSCREEN, &render::disable_fullscreen, this);
    new_slot(ENABLE_FULLSCREEN, &render::enable_fullscreen, this);
    new_slot(SET_MANUAL_MODE, [this](){this->m_render_mode = render_mode::manual;});
    new_slot(SET_AUTO_MODE, [this](){this->m_render_mode = render_mode::AUTO;});
}

//-----------------------------------------------------------------------------

render::~render() noexcept
{
    m_ogre_root = nullptr;
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
        m_off_screen = (key == OFFSCREEN_INOUT);

        SIGHT_ASSERT("'" + key + "' is not a valid key. Only '" << OFFSCREEN_INOUT << "' is accepted.", m_off_screen);

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
        m_render_mode = render_mode::AUTO;
    }
    else if(render_mode == "manual" || render_mode == "sync") /* Keep sync for backwards compatibility */
    {
        m_render_mode = render_mode::manual;
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
        const core::com::slots::key_t resetcamera_slotkey = "reset_camera_" + layer_id;
        auto reset_camera_layer_slot                      = new_slot(
            resetcamera_slotkey,
            [this, layer_id]()
            {
                this->reset_camera_coordinates(layer_id);
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

    if(!m_off_screen)
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
        this->configure_layer(it->second);
    }

    auto bkg_configs = scene_cfg.equal_range("background");
    for(auto it = bkg_configs.first ; it != bkg_configs.second ; ++it)
    {
        SIGHT_ERROR_IF("A background has already been set, overriding it...", b_has_background);
        try
        {
            this->configure_background_layer(it->second);
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
        ogre_layer->set_render_service(std::dynamic_pointer_cast<viz::scene3d::render>(this->shared_from_this()));
        ogre_layer->set_id("backgroundLayer");
        ogre_layer->set_order(0);
        ogre_layer->set_worker(this->worker());
        ogre_layer->set_background_color("#000000", "#000000");
        ogre_layer->set_background_scale(0, 0.5);
        ogre_layer->set_has_default_light(false);
        ogre_layer->set_viewport_config({0.F, 0.F, 1.F, 1.F});

        m_layers[OGREBACKGROUNDID] = ogre_layer;
    }

    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);
    if(m_off_screen)
    {
        // Instantiate the manager that help to communicate between this service and the widget
        m_interactor_manager = viz::scene3d::window_interactor::create_offscreen_manager(m_width, m_height);
        m_interactor_manager->set_render_service(this->get_sptr());
        m_interactor_manager->create_container(nullptr, m_fullscreen, service_id);
    }
    else
    {
        // Instantiate the manager that help to communicate between this service and the widget
        m_interactor_manager = viz::scene3d::window_interactor::create_manager();
        m_interactor_manager->set_render_service(this->get_sptr());
        m_interactor_manager->create_container(this->get_container(), m_fullscreen, service_id);
    }

    // Everything is started now, we can safely create connections and thus receive interactions from the widget
    m_interactor_manager->connect_to_container();
}

//-----------------------------------------------------------------------------

void render::stopping()
{
    this->make_current();

    for(const auto& it : m_layers)
    {
        viz::scene3d::layer::sptr layer = it.second;
        layer->destroy_scene();
    }

    m_layers.clear();

    m_interactor_manager->disconnect_interactor();
    m_interactor_manager.reset();

    if(!m_off_screen)
    {
        this->destroy();
    }
}

//-----------------------------------------------------------------------------

void render::updating()
{
}

//-----------------------------------------------------------------------------

void render::make_current()
{
    m_interactor_manager->make_current();
}

//-----------------------------------------------------------------------------

void render::configure_layer(const config_t& _cfg)
{
    const config_t attributes                = _cfg.get_child("<xmlattr>");
    const std::string id                     = attributes.get<std::string>("id", "");
    const std::string compositors            = attributes.get<std::string>("compositors", "");
    const std::string transparency_technique = attributes.get<std::string>("transparency", "");
    const std::string num_peels              = attributes.get<std::string>("numPeels", "4");
    const std::string stereo_mode            = attributes.get<std::string>("stereoMode", "");
    const std::string default_light          = attributes.get<std::string>("defaultLight", "");
    const auto viewport_config               = configure_layer_viewport(_cfg);

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
                                                        ? compositor::core::stereo_mode_t::autostereo_5
                                                        : stereo_mode == "AutoStereo8"
                                                        ? compositor::core::stereo_mode_t::autostereo_8
                                                        : stereo_mode == "Stereo"
                                                        ? compositor::core::stereo_mode_t::stereo
                                                        : compositor::core::stereo_mode_t::none;

    ogre_layer->set_render_service(std::dynamic_pointer_cast<viz::scene3d::render>(this->shared_from_this()));
    ogre_layer->set_id(id);
    ogre_layer->set_order(layer_order);
    ogre_layer->set_worker(this->worker());
    ogre_layer->set_core_compositor_enabled(true, transparency_technique, num_peels, layer_stereo_mode);
    ogre_layer->set_compositor_chain_enabled(compositors);
    ogre_layer->set_viewport_config(viewport_config);

    if(!default_light.empty() && default_light == "false")
    {
        ogre_layer->set_has_default_light(false);
    }

    // Finally, the layer is pushed in the map
    m_layers[id] = ogre_layer;
}

//-----------------------------------------------------------------------------

void render::configure_background_layer(const config_t& _cfg)
{
    SIGHT_ASSERT("'id' required attribute missing or empty", !this->get_id().empty());
    const config_t attributes = _cfg.get_child("<xmlattr>");

    viz::scene3d::layer::sptr ogre_layer = std::make_shared<viz::scene3d::layer>();
    ogre_layer->set_render_service(std::dynamic_pointer_cast<viz::scene3d::render>(this->shared_from_this()));
    ogre_layer->set_id(OGREBACKGROUNDID);
    ogre_layer->set_order(0);
    ogre_layer->set_worker(this->worker());
    ogre_layer->set_has_default_light(false);

    if(attributes.count("material") != 0U)
    {
        const auto material = attributes.get<std::string>("material");
        ogre_layer->set_background_material(material);
    }
    else if(attributes.count("color") != 0U)
    {
        const auto color = attributes.get<std::string>("color");
        ogre_layer->set_background_color(color, color);
    }
    else if((attributes.count("topColor") != 0U) && (attributes.count("bottomColor") != 0U))
    {
        const auto top_color = attributes.get<std::string>("topColor");
        const auto bot_color = attributes.get<std::string>("bottomColor");

        ogre_layer->set_background_color(top_color, bot_color);
    }

    if((attributes.count("topScale") != 0U) && (attributes.count("bottomScale") != 0U))
    {
        const auto top_scale_val = attributes.get<float>("topScale");
        const auto bot_scale_val = attributes.get<float>("bottomScale");

        ogre_layer->set_background_scale(top_scale_val, bot_scale_val);
    }
    else
    {
        ogre_layer->set_background_scale(0.5F, 0.5F);
    }

    m_layers[OGREBACKGROUNDID] = ogre_layer;
}

//-----------------------------------------------------------------------------

layer::viewport_config_t render::configure_layer_viewport(const service::config_t& _cfg)
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

void render::request_render()
{
    if(m_render_mode == render_mode::manual)
    {
        m_interactor_manager->render_now();
    }
    else
    {
        m_interactor_manager->request_render();
    }

    if(m_off_screen)
    {
        FW_PROFILE("Offscreen rendering");

        const auto image = m_off_screen_image.lock();
        {
            this->make_current();
            Ogre::TexturePtr render_texture = m_interactor_manager->get_render_texture();
            SIGHT_ASSERT("The offscreen window doesn't write to a texture", render_texture);
            viz::scene3d::utils::convert_from_ogre_texture(render_texture, image.get_shared(), m_flip);
        }

        auto sig = image->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void render::reset_camera_coordinates(const std::string& _layer_id)
{
    auto layer = m_layers.find(_layer_id);

    if(layer != m_layers.end())
    {
        layer->second->reset_camera_coordinates();
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void render::reset_cameras()
{
    for(const auto& layer : m_layers)
    {
        layer.second->reset_camera_coordinates();
    }

    this->request_render();
}

//-----------------------------------------------------------------------------

void render::paint()
{
    this->request_render();
}

//-----------------------------------------------------------------------------

bool render::is_shown_on_screen()
{
    return m_off_screen || m_fullscreen || this->get_container()->is_shown_on_screen();
}

// ----------------------------------------------------------------------------

Ogre::SceneManager* render::get_scene_manager(const std::string& _scene_id)
{
    viz::scene3d::layer::sptr layer = this->layer(_scene_id);
    return layer->get_scene_manager();
}

// ----------------------------------------------------------------------------

viz::scene3d::layer::sptr render::layer(const std::string& _scene_id)
{
    SIGHT_ASSERT("Empty sceneID", !_scene_id.empty());
    SIGHT_ASSERT("layer ID " << _scene_id << " does not exist", m_layers.find(_scene_id) != m_layers.end());

    viz::scene3d::layer::sptr layer = m_layers.at(_scene_id);

    return layer;
}

// ----------------------------------------------------------------------------

viz::scene3d::render::layer_map_t render::get_layers()
{
    return m_layers;
}

// ----------------------------------------------------------------------------

viz::scene3d::window_interactor::sptr render::get_interactor_manager() const
{
    return m_interactor_manager;
}

// ----------------------------------------------------------------------------

void render::disable_fullscreen()
{
    m_fullscreen = false;
    m_interactor_manager->set_fullscreen(m_fullscreen, -1);
    this->signal<signals::void_signal_t>(signals::FULLSCREEN_UNSET)->async_emit();
}

// ----------------------------------------------------------------------------

void render::enable_fullscreen(int _screen)
{
    m_fullscreen = true;
    m_interactor_manager->set_fullscreen(m_fullscreen, _screen);
    this->signal<signals::void_signal_t>(signals::FULLSCREEN_SET)->async_emit();
}

// ----------------------------------------------------------------------------

} //namespace sight::viz::scene3d
