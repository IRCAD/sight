/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "selector.hpp"

#include <app/extension/config.hpp>

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

#include <service/extension/factory.hpp>
#include <service/op.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <sstream>
#include <string>

namespace sight::module::ui::io
{

namespace io = sight::io;

//------------------------------------------------------------------------------

static const core::com::signals::key_t JOB_CREATED_SIGNAL   = "job_created";
static const core::com::signals::key_t JOB_FAILED_SIGNAL    = "jobFailed";
static const core::com::signals::key_t JOB_SUCCEEDED_SIGNAL = "job_succeeded";

static const core::com::slots::key_t FORWARD_JOB_SLOT = "forwardJob";

//------------------------------------------------------------------------------

selector::selector() :
    m_sig_job_created(new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL)),
    m_sig_job_failed(new_signal<job_failed_signal_t>(JOB_FAILED_SIGNAL)),
    m_sig_job_succeeded(new_signal<job_succeeded_signal_t>(JOB_SUCCEEDED_SIGNAL)),
    m_slot_forward_job(new_slot(FORWARD_JOB_SLOT, &selector::forward_job, this))
{
}

//------------------------------------------------------------------------------

selector::~selector() noexcept =
    default;

//------------------------------------------------------------------------------

void selector::configuring()
{
    const config_t srv_config = this->get_config();

    const std::string mode = srv_config.get<std::string>("type.<xmlattr>.mode", "reader");
    SIGHT_ASSERT(
        "The xml attribute <mode> must be 'reader' (to open file) or 'writer' (to write a new file).",
        mode == "writer" || mode == "reader"
    );
    m_mode = (mode == "writer") ? writer_mode : reader_mode;
    SIGHT_DEBUG("mode => " + mode);

    const std::string selection_mode = srv_config.get<std::string>("selection.<xmlattr>.mode", "exclude");
    SIGHT_ASSERT(
        "The xml attribute <mode> must be 'include' (to add the selection to selector list ) or "
        "'exclude' (to exclude the selection of the selector list).",
        selection_mode == "exclude" || selection_mode == "include"
    );
    m_services_are_excluded = (selection_mode == "exclude");
    SIGHT_DEBUG("selection mode => " + selection_mode);

    for(const auto& it_selection : boost::make_iterator_range(srv_config.equal_range("addSelection")))
    {
        const auto service = it_selection.second.get<std::string>("<xmlattr>.service");
        m_selected_services.push_back(service);
        SIGHT_DEBUG("add selection => " + service);

        const std::string config_id = it_selection.second.get<std::string>("<xmlattr>.config", "");
        if(!config_id.empty())
        {
            m_service_to_config[service] = config_id;
            SIGHT_DEBUG(std::string("add config '") + config_id + "' for service '" + service + "'");
        }
    }

    for(const auto& it_cfg : boost::make_iterator_range(srv_config.equal_range("config")))
    {
        const auto service   = it_cfg.second.get<std::string>("<xmlattr>.service");
        const auto config_id = it_cfg.second.get<std::string>("<xmlattr>.id");

        m_service_to_config[service] = config_id;
        SIGHT_DEBUG(std::string("add config '") + config_id + "' for service '" + service + "'");
    }
}

//------------------------------------------------------------------------------

void selector::starting()
{
}

//------------------------------------------------------------------------------

void selector::stopping()
{
}

//------------------------------------------------------------------------------

void selector::updating()
{
    std::vector<std::string> available_extensions_id;
    {
        auto obj_lock          = m_data.lock();
        data::object::sptr obj = obj_lock.get_shared();

        // Retrieve implementation of type io::service::reader for this object
        if(m_mode == reader_mode)
        {
            SIGHT_ASSERT("An inout key '" + io::service::DATA_KEY + "' must be defined.", obj);
            const auto classname = obj->get_classname();

            available_extensions_id =
                service::extension::factory::get()->get_implementation_id_from_object_and_type(
                    classname,
                    "sight::io::service::reader"
                );
        }
        else // m_mode == WRITER_MODE
        {
            SIGHT_ASSERT("The inout key '" + io::service::DATA_KEY + "' is not correctly set.", obj);

            available_extensions_id =
                service::extension::factory::get()->get_implementation_id_from_object_and_type(
                    obj->get_classname(),
                    "sight::io::service::writer"
                );
        }
    }

    // filter available extensions and replace id by service description
    std::vector<std::pair<std::string, std::string> > available_extensions_map;
    std::vector<std::string> available_extensions_selector;

    for(const std::string& service_id : available_extensions_id)
    {
        bool service_is_selected_by_user =
            std::find(
                m_selected_services.begin(),
                m_selected_services.end(),
                service_id
            ) != m_selected_services.end();

        // Test if the service is considered here as available by users, if yes push in availableExtensionsSelector
        // excluded mode => add services that are not selected by users
        // included mode => add services selected by users
        if((m_services_are_excluded && !service_is_selected_by_user)
           || (!m_services_are_excluded && service_is_selected_by_user))
        {
            // Add this service
            std::string info_user =
                service::extension::factory::get()->get_service_description(service_id);

            auto iter = m_service_to_config.find(service_id);
            if(iter != m_service_to_config.end())
            {
                info_user = service::extension::config::get_default()->get_config_desc(iter->second);
            }

            if(!info_user.empty())
            {
                available_extensions_map.emplace_back(service_id, info_user);
                available_extensions_selector.push_back(info_user);
            }
            else
            {
                available_extensions_map.emplace_back(service_id, service_id);
                available_extensions_selector.push_back(service_id);
            }
        }
    }

    // Sort available services (lexical string sort)
    std::sort(available_extensions_selector.begin(), available_extensions_selector.end());

    // Test if we have an extension
    if(!available_extensions_map.empty())
    {
        std::string extension_id             = available_extensions_map[0].first;
        bool extension_selection_is_canceled = false;

        // Selection of extension when availableExtensions.size() > 1
        if(available_extensions_selector.size() > 1)
        {
            sight::ui::dialog::selector selector;

            if(m_mode != reader_mode)
            {
                selector.set_title("Writer to use");
            }
            else
            {
                selector.set_title("Reader to use");
            }

            selector.set_choices(available_extensions_selector);

            if(const auto& choices = selector.show(); !choices.empty())
            {
                const auto& choice      = choices.front();
                bool extension_id_found = false;

                using pair_t = std::pair<std::string, std::string>;
                for(const pair_t& pair : available_extensions_map)
                {
                    if(pair.second == choice)
                    {
                        extension_id       = pair.first;
                        extension_id_found = true;
                    }
                }

                if(!extension_id_found)
                {
                    m_sig_job_failed->async_emit();
                }

                SIGHT_ASSERT("Problem to find the selected string.", extension_id_found);
            }
            else
            {
                extension_selection_is_canceled = true;
            }
        }

        if(!extension_selection_is_canceled)
        {
            // Get config
            bool has_config_for_service = false;
            service::config_t srv_cfg;
            if(m_service_to_config.find(extension_id) != m_service_to_config.end())
            {
                has_config_for_service = true;
                srv_cfg                = service::extension::config::get_default()->get_service_config(
                    m_service_to_config[extension_id],
                    extension_id
                );
                SIGHT_ASSERT(
                    "No service configuration of type service::extension::config was found",
                    !srv_cfg.empty()
                );
            }

            // Configure and start service
            if(m_mode == reader_mode)
            {
                auto reader = service::add<io::service::reader>(extension_id);
                {
                    auto obj = m_data.lock();
                    reader->set_inout(obj.get_shared(), io::service::DATA_KEY);
                }

                reader->set_worker(this->worker());

                if(has_config_for_service)
                {
                    reader->set_config(srv_cfg);
                }

                reader->configure();

                auto job_created_signal_t = reader->signal("job_created");
                if(job_created_signal_t)
                {
                    job_created_signal_t->connect(m_slot_forward_job);
                }

                try
                {
                    reader->start();
                    reader->open_location_dialog();

                    sight::ui::cursor cursor;
                    cursor.set_cursor(sight::ui::cursor_base::busy);
                    reader->update();
                    cursor.set_default_cursor();

                    reader->stop();
                    service::unregister_service(reader);
                }
                catch(std::exception& e)
                {
                    std::string msg = "Failed to read : \n" + std::string(e.what());
                    sight::ui::dialog::message::show("Reader Error", msg);
                    m_sig_job_failed->async_emit();
                }
                if(reader->has_failed())
                {
                    m_sig_job_failed->async_emit();
                }
                else
                {
                    m_sig_job_succeeded->async_emit();
                }
            }
            else
            {
                auto writer = service::add<io::service::writer>(extension_id);
                {
                    auto obj = m_data.lock();
                    writer->set_input(obj.get_shared(), io::service::DATA_KEY);
                }

                writer->set_worker(this->worker());

                if(has_config_for_service)
                {
                    writer->set_config(srv_cfg);
                }

                writer->configure();

                auto job_created_signal_t = writer->signal("job_created");
                if(job_created_signal_t)
                {
                    job_created_signal_t->connect(m_slot_forward_job);
                }

                try
                {
                    writer->start();
                    writer->open_location_dialog();

                    sight::ui::cursor cursor;
                    cursor.set_cursor(sight::ui::cursor_base::busy);
                    writer->update();
                    cursor.set_default_cursor();

                    writer->stop();
                    service::unregister_service(writer);
                }
                catch(std::exception& e)
                {
                    std::string msg = "Failed to write : \n" + std::string(e.what());
                    sight::ui::dialog::message::show("Writer Error", msg);
                    m_sig_job_failed->async_emit();
                }

                if(writer->has_failed())
                {
                    m_sig_job_failed->async_emit();
                }
                else
                {
                    m_sig_job_succeeded->async_emit();
                }
            }
        }
        else
        {
            m_sig_job_failed->async_emit();
        }
    }
    else
    {
        SIGHT_WARN("selector::load : availableExtensions is empty.");
        if(m_mode == reader_mode)
        {
            sight::ui::dialog::message message_box;
            message_box.set_title("Reader not found");
            message_box.set_message("There are no available readers for this data type.");
            message_box.set_icon(sight::ui::dialog::message::warning);
            message_box.add_button(sight::ui::dialog::message::ok);
            message_box.show();
        }
        else // m_mode == WRITER_MODE
        {
            sight::ui::dialog::message message_box;
            message_box.set_title("Writer not found");
            message_box.set_message("There are no available writers for this data type.");
            message_box.set_icon(sight::ui::dialog::message::warning);
            message_box.add_button(sight::ui::dialog::message::ok);
            message_box.show();
        }

        m_sig_job_failed->async_emit();
    }
}

//------------------------------------------------------------------------------

void selector::info(std::ostream& _sstream)
{
    // Update message
    _sstream << "selector";
}

//------------------------------------------------------------------------------

void selector::set_io_mode(io_mode _mode)
{
    m_mode = _mode;
}

//------------------------------------------------------------------------------

void selector::forward_job(core::jobs::base::sptr _job)
{
    m_sig_job_created->emit(_job);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::io
