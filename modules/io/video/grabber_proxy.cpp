/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/io/video/grabber_proxy.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/frame_tl.hpp>

#include <service/extension/config.hpp>
#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>

namespace sight::module::io::video
{

//-----------------------------------------------------------------------------

using sight::io::service::grabber;

//-----------------------------------------------------------------------------

grabber_proxy::grabber_proxy() noexcept
{
    new_slot(slots::RECONFIGURE, &grabber_proxy::reconfigure, this);

    new_slot(slots::MODIFY_POSITION, &grabber_proxy::modify_position, this);
    new_slot(slots::MODIFY_DURATION, &grabber_proxy::modify_duration, this);
    new_slot(slots::FWD_START_CAMERA, &grabber_proxy::fwd_start_camera, this);
    new_slot(slots::FWD_STOP_CAMERA, &grabber_proxy::fwd_stop_camera, this);
    new_slot(slots::FWD_PRESENT_FRAME, &grabber_proxy::fwd_present_frame, this);

    new_slot(slots::FWD_NOTIFY, &grabber_proxy::fwd_notify, this);

    new_slot(slots::FWD_SET_PARAMETER, &grabber_proxy::fwd_set_parameter, this);
    new_slot(slots::FWD_CREATE_JOB, &grabber_proxy::fwd_create_job, this);
}

//-----------------------------------------------------------------------------

grabber_proxy::~grabber_proxy() noexcept =
    default;

//-----------------------------------------------------------------------------

void grabber_proxy::starting()
{
}

//-----------------------------------------------------------------------------

void grabber_proxy::stopping()
{
    m_connections.disconnect();

    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->stop_camera();
            this->unregister_service(srv);
            srv.reset();
        }
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::configuring()
{
    const auto config = this->get_config();

    auto it_sub_config = config.find("config");

    m_selected_services.clear();
    m_service_to_config.clear();

    if(it_sub_config != config.not_found())
    {
        const auto& sub_config = it_sub_config->second;

        m_type = sub_config.get<std::string>("camera.<xmlattr>.type", "RGB") == "RGB"
                 ? camera_t::rgb
                 : camera_t::rgbd;

        const std::string mode = sub_config.get<std::string>("selection.<xmlattr>.mode", "exclude");
        SIGHT_ASSERT(
            "The xml attribute <mode> must be 'include' (to add the selection to selector list ) or "
            "'exclude' (to exclude the selection of the selector list).",
            mode == "exclude" || mode == "include"
        );
        m_exclude = (mode == "exclude");
        SIGHT_DEBUG("selection mode => " << (m_exclude ? "Exclude" : "Include"));

        const auto selection_cfg = sub_config.equal_range("addSelection");
        for(auto it_selection = selection_cfg.first ; it_selection != selection_cfg.second ; ++it_selection)
        {
            const auto service = it_selection->second.get<std::string>("<xmlattr>.service");
            m_selected_services.insert(service);
            SIGHT_DEBUG("add selection => " + service);

            const std::string config_id = it_selection->second.get<std::string>("<xmlattr>.config", "");
            // Check if service is not empty.
            SIGHT_ASSERT("add selection with config but service is missing", !service.empty());
            m_service_to_config[service].push_back(config_id);
            SIGHT_DEBUG(
                std::string("add config '") + config_id + "' for service '" + service + "'"
            );
        }

        const auto config_cfg = sub_config.equal_range("config");
        for(auto it_cfg = config_cfg.first ; it_cfg != config_cfg.second ; ++it_cfg)
        {
            const auto service   = it_cfg->second.get<std::string>("<xmlattr>.service");
            const auto config_id = it_cfg->second.get<std::string>("<xmlattr>.id");

            m_service_to_config[service].push_back(config_id);
            SIGHT_DEBUG(
                std::string("add config '") + config_id + "' for service '" + service + "'"
            );
        }

        m_gui_title = sub_config.get<std::string>("gui.<xmlattr>.title", m_gui_title);
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::updating()
{
}

//-----------------------------------------------------------------------------

void grabber_proxy::start_camera()
{
    if(m_services.empty())
    {
        if(m_grabber_impl.empty())
        {
            const auto srv_factory        = service::extension::factory::get();
            const auto srv_config_factory = service::extension::config::get_default();

            // We select all RGBD grabbers. They should be capable to output a single color frame
            auto grabbers_impl = srv_factory->get_implementation_id_from_object_and_type(
                "sight::data::frame_tl",
                "sight::io::service::rgbd_grabber"
            );

            auto rgb_grabbers_impl = srv_factory->get_implementation_id_from_object_and_type(
                "sight::data::frame_tl",
                "sight::io::service::grabber"
            );

            std::move(rgb_grabbers_impl.begin(), rgb_grabbers_impl.end(), std::back_inserter(grabbers_impl));

            if(const auto config = this->get_config().get_child_optional("config"); config.has_value())
            {
                auto camera_input = m_camera.lock();
                auto camera       = std::dynamic_pointer_cast<const data::camera>(camera_input.get_shared());
                if(camera)
                {
                    auto match_config = config->equal_range("match");
                    for(const auto& match : boost::make_iterator_range(match_config))
                    {
                        const auto id = match.second.get<std::string>("<xmlattr>.id");
                        if(camera->get_camera_id() == id)
                        {
                            auto service_impl = match.second.get<std::string>("<xmlattr>.service");
                            std::erase_if(
                                grabbers_impl,
                                [&](std::string _g)
                                {
                                    return _g != service_impl;
                                });
                        }
                    }
                }
            }

            data::camera::source_t source_type = data::camera::unknown;

            std::size_t num_cameras_in_series = 1;

            {
                auto camera_input = m_camera.lock();
                auto camera       = std::dynamic_pointer_cast<const data::camera>(camera_input.get_shared());
                if(camera)
                {
                    source_type = camera->get_camera_source();
                }
                else
                {
                    auto camera_set = std::dynamic_pointer_cast<const data::camera_set>(camera_input.get_shared());
                    if(camera_set)
                    {
                        num_cameras_in_series = camera_set->size();
                        SIGHT_ASSERT("Camera Series is empty", num_cameras_in_series);

                        // Assume same source on all cameras
                        source_type = camera_set->get_camera(0)->get_camera_source();
                    }
                }
            }

            std::vector<std::string> available_extensions_selector;
            std::map<std::string, std::size_t> impl_to_num_tl;

            for(const auto& srv_impl : grabbers_impl)
            {
                if(srv_impl != "sight::module::io::video::grabber_proxy")
                {
                    SIGHT_DEBUG("Evaluating if implementation '" + srv_impl + "' is suitable...");
                    auto objects_type = srv_factory->get_service_objects(srv_impl);
                    const auto config = this->get_config();

                    // 1. Verify that we have the same number of timelines
                    objects_type.erase(
                        std::remove_if(
                            objects_type.begin(),
                            objects_type.end(),
                            [&](const std::string& _type)
                        {
                            return _type != "data::frame_tl";
                        }),
                        objects_type.end()
                    );

                    std::size_t num_tl = 0;
                    auto inouts_cfg    = config.equal_range("inout");
                    for(auto it_cfg = inouts_cfg.first ; it_cfg != inouts_cfg.second ; ++it_cfg)
                    {
                        service::config_t parameter_cfg;

                        const auto key = it_cfg->second.get<std::string>("<xmlattr>.key");
                        SIGHT_DEBUG("Evaluating if key '" + key + "' is suitable...");
                        const auto obj = this->inout(key).lock();
                        SIGHT_ASSERT("Object key '" + key + "' not found", obj);
                        if(obj->get_classname() == "data::frame_tl")
                        {
                            ++num_tl;
                        }
                    }

                    if(num_tl > objects_type.size())
                    {
                        if(objects_type.size() == 1 && num_cameras_in_series == num_tl)
                        {
                            // We have only one TL, we may emulate a grabber with several timelines using multiple
                            // services
                            impl_to_num_tl[srv_impl] = num_tl;
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                        impl_to_num_tl[srv_impl] = 1;
                    }

                    // 2. filter against the source type
                    if(source_type != data::camera::unknown)
                    {
                        const auto tags = srv_factory->get_service_tags(srv_impl);

                        const boost::char_separator<char> sep(",");
                        const boost::tokenizer<boost::char_separator<char> > tokens(tags, sep);
                        bool caps_match = false;
                        for(const auto& token : tokens)
                        {
                            // Remove trailing and leading spaces.
                            const auto trimed_token = boost::algorithm::trim_copy(token);

                            data::camera::source_t handled_source_type = data::camera::unknown;
                            if(trimed_token == "FILE")
                            {
                                handled_source_type = data::camera::file;
                            }
                            else if(trimed_token == "STREAM")
                            {
                                handled_source_type = data::camera::stream;
                            }
                            else if(trimed_token == "DEVICE")
                            {
                                handled_source_type = data::camera::device;
                            }

                            if(handled_source_type == source_type)
                            {
                                caps_match = true;
                                break;
                            }
                        }

                        if(!caps_match)
                        {
                            continue;
                        }
                    }

                    available_extensions_selector.push_back(srv_impl);
                }
            }

            // 3. Check if specific service or configuration should be included/excluded
            {
                std::map<std::string, std::pair<std::string, std::string> > desc_to_extension;
                std::vector<std::string> descriptions;

                const auto& srv_config_registry = service::extension::config::get_default();
                for(const auto& extension : available_extensions_selector)
                {
                    // We need to test first if extension have specific configurations to include/exclude.
                    const auto configs_it = m_service_to_config.find(extension);
                    std::vector<std::string> selectable_configs;

                    if(!m_exclude) // Include mode
                    {
                        // Available service/configs are the ones the proxy's configuration.
                        if(configs_it != m_service_to_config.end())
                        {
                            selectable_configs = configs_it->second;
                        }
                    }
                    else // Exclude mode
                    {
                        // Find all configurations for the given grabber.
                        selectable_configs = srv_config_factory->get_all_config_for_service(extension, true);
                        selectable_configs.emplace_back(""); // Add the empty config (default grabber).

                        // Remove configs from the grabber's list.
                        if(configs_it != m_service_to_config.end())
                        {
                            const auto& excluded_configs = configs_it->second;
                            const auto is_excluded_config = [&excluded_configs](const std::string& _cfg_name) -> bool
                                                            {
                                                                return std::find(
                                                                    excluded_configs.begin(),
                                                                    excluded_configs.end(),
                                                                    _cfg_name
                                                                ) != excluded_configs.end();
                                                            };

                            // Remove the ones excluded by the grabber proxy.
                            selectable_configs.erase(
                                std::remove_if(
                                    selectable_configs.begin(),
                                    selectable_configs.end(),
                                    is_excluded_config
                                ),
                                selectable_configs.end()
                            );
                        }
                    }

                    // Fill the description list with the available config's descriptions.
                    for(const auto& config : selectable_configs)
                    {
                        const auto desc = config.empty() ? srv_factory->get_service_description(extension)
                                                         : srv_config_registry->get_config_desc(config);
                        desc_to_extension[desc] = std::make_pair(extension, config);
                        descriptions.push_back(desc);
                    }
                }

                std::string selected_desc;
                if(descriptions.empty())
                {
                    const std::string msg = "No video grabber implementation found.\n";
                    sight::ui::dialog::message::show(
                        "Error",
                        msg,
                        sight::ui::dialog::message::icons::warning
                    );
                    return;
                }

                if(descriptions.size() == 1)
                {
                    /// Select the only remaining description.
                    selected_desc = descriptions[0];
                }
                else
                {
                    // Sort the description list.
                    std::sort(std::begin(descriptions), std::end(descriptions));

                    sight::ui::dialog::selector selector;
                    selector.set_title(m_gui_title);
                    selector.set_choices(descriptions);
                    const auto& choices = selector.show();
                    selected_desc = choices.empty() ? std::string() : choices.front();
                }

                std::tie(m_grabber_impl, m_grabber_config) = desc_to_extension[selected_desc];
            }

            if(m_grabber_impl.empty())
            {
                const std::string msg = "No video grabber chosen, aborting...\n";
                sight::ui::dialog::message::show("Warning", msg);
                return;
            }

            m_services.resize(impl_to_num_tl[m_grabber_impl]);

            std::size_t srv_count = 0;
            for(auto& srv : m_services)
            {
                srv = this->register_service<grabber>(m_grabber_impl);

                auto camera_input = m_camera.lock();
                auto camera       = std::dynamic_pointer_cast<const data::camera>(camera_input.get_shared());
                if(camera)
                {
                    srv->set_input(camera, CAMERA_INPUT);
                }
                else
                {
                    auto camera_set = std::dynamic_pointer_cast<const data::camera_set>(camera_input.get_shared());
                    if(camera_set)
                    {
                        #ifdef DEBUG
                        const std::size_t num_cameras = camera_set->size();
                        SIGHT_ASSERT(
                            "Not enough cameras in series to emulate the grabber",
                            srv_count < num_cameras
                        );
                        #endif

                        srv->set_input(camera_set->get_camera(srv_count), CAMERA_INPUT);
                    }
                }

                std::size_t input_tl_count = 0;
                const auto proxy_config    = this->get_config();
                auto inouts_cfg            = proxy_config.equal_range("inout");
                for(auto it_cfg = inouts_cfg.first ; it_cfg != inouts_cfg.second ; ++it_cfg)
                {
                    const auto key = it_cfg->second.get<std::string>("<xmlattr>.key");
                    SIGHT_ASSERT("Missing 'key' tag.", !key.empty());

                    auto inout = this->inout(key).lock();
                    if(inout)
                    {
                        if(key == grabber::FRAMETL_INOUT)
                        {
                            auto frame_tl = std::dynamic_pointer_cast<data::frame_tl>(inout.get_shared());
                            if(m_services.size() > 1)
                            {
                                if(input_tl_count == srv_count)
                                {
                                    // We are emulating a grabber with several ones, reuse the first TL slot
                                    srv->set_inout(frame_tl, grabber::FRAMETL_INOUT);
                                    break;
                                }
                            }
                            else
                            {
                                srv->set_inout(frame_tl, grabber::FRAMETL_INOUT);
                            }
                        }
                    }

                    ++input_tl_count;
                }

                if(!m_grabber_config.empty())
                {
                    const auto& srv_config_registry = service::extension::config::get_default();
                    const auto srv_cfg              =
                        srv_config_registry->get_service_config(m_grabber_config, m_grabber_impl);
                    srv->set_config(srv_cfg);
                    srv->configure();
                }

                srv->set_worker(this->worker());
                srv->start();

                m_connections.connect(srv, grabber::POSITION_MODIFIED_SIG, this->get_sptr(), slots::MODIFY_POSITION);
                m_connections.connect(srv, grabber::DURATION_MODIFIED_SIG, this->get_sptr(), slots::MODIFY_DURATION);
                m_connections.connect(srv, grabber::CAMERA_STARTED_SIG, this->get_sptr(), slots::FWD_START_CAMERA);
                m_connections.connect(srv, grabber::CAMERA_STOPPED_SIG, this->get_sptr(), slots::FWD_STOP_CAMERA);
                m_connections.connect(srv, grabber::FRAME_PRESENTED_SIG, this->get_sptr(), slots::FWD_PRESENT_FRAME);

                m_connections.connect(srv, notifier::signals::NOTIFIED, this->get_sptr(), slots::FWD_NOTIFY);

                m_connections.connect(
                    srv,
                    grabber::PARAMETER_CHANGED_SIG,
                    this->get_sptr(),
                    slots::FWD_SET_PARAMETER
                );

                m_connections.connect(
                    srv,
                    grabber::JOB_CREATED_SIG,
                    this->get_sptr(),
                    slots::FWD_CREATE_JOB
                );

                m_connections.connect(
                    srv,
                    grabber::FPS_CHANGED_SIG,
                    this->get_sptr(),
                    grabber::FORWARD_FPS_CHANGED_SLOT
                );

                ++srv_count;
            }
        }
    }

    for(auto& srv : m_services)
    {
        srv->start_camera();
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::pause_camera()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->pause_camera();
        }
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::stop_camera()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->stop_camera();
        }
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::toggle_loop_mode()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->toggle_loop_mode();
        }
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::set_position(std::int64_t _position)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->set_position(_position);
        }
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::previous_image()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->previous_image();
        }
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::next_image()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->next_image();
        }
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::set_step(int _step, std::string _key)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->set_parameter(_step, _key);
        }
    }
}

//------------------------------------------------------------------------------

void grabber_proxy::set_parameter(ui::parameter_t _value, std::string _key)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->set_parameter(_value, _key);
        }
    }
}

//------------------------------------------------------------------------------

void grabber_proxy::request_settings()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->request_settings();
        }
    }
}

//------------------------------------------------------------------------------

void grabber_proxy::optimize()
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->optimize();
        }
    }
}

//------------------------------------------------------------------------------

void grabber_proxy::add_roi_center(sight::data::point::sptr _p)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->add_roi_center(_p);
        }
    }
}

//------------------------------------------------------------------------------

void grabber_proxy::remove_roi_center(sight::data::point::sptr _p)
{
    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->remove_roi_center(_p);
        }
    }
}

//-----------------------------------------------------------------------------

void grabber_proxy::reconfigure()
{
    m_connections.disconnect();

    for(auto& srv : m_services)
    {
        if(srv != nullptr)
        {
            srv->stop_camera();
            this->unregister_service(srv);
            srv.reset();
        }
    }

    m_services.clear();

    m_grabber_impl = "";
}

//-----------------------------------------------------------------------------

void grabber_proxy::modify_position(int64_t _position)
{
    auto sig = this->signal<position_modified_signal_t>(POSITION_MODIFIED_SIG);
    sig->async_emit(static_cast<std::int64_t>(_position));
}

//-----------------------------------------------------------------------------

void grabber_proxy::modify_duration(int64_t _duration)
{
    auto sig = this->signal<duration_modified_signal_t>(DURATION_MODIFIED_SIG);
    sig->async_emit(static_cast<std::int64_t>(_duration));
}

//-----------------------------------------------------------------------------

void grabber_proxy::fwd_start_camera()
{
    auto sig = this->signal<camera_started_signal_t>(CAMERA_STARTED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void grabber_proxy::fwd_stop_camera()
{
    auto sig = this->signal<camera_stopped_signal_t>(CAMERA_STOPPED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void grabber_proxy::fwd_present_frame()
{
    auto sig = this->signal<frame_presented_signal_t>(FRAME_PRESENTED_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void grabber_proxy::fwd_notify(service::notification _notification)
{
    notifier::m_notified_sig->async_emit(std::move(_notification));
}

//------------------------------------------------------------------------------

void grabber_proxy::fwd_set_parameter(ui::parameter_t _value, std::string _key)
{
    auto sig = this->signal<grabber::parameter_changed_t>(grabber::PARAMETER_CHANGED_SIG);
    sig->async_emit(_value, _key);
}

//------------------------------------------------------------------------------

void grabber_proxy::fwd_create_job(sight::core::jobs::base::sptr _job)
{
    auto sig = this->signal<grabber::job_created_signal_t>(grabber::JOB_CREATED_SIG);
    sig->async_emit(_job);
}

//------------------------------------------------------------------------------

void grabber_proxy::forward_fps_changed(double _fps)
{
    auto sig = this->signal<grabber::fps_changed_signal_t>(grabber::FPS_CHANGED_SIG);
    sig->async_emit(_fps);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
