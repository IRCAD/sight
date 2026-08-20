/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "core/thread/worker.hpp"

#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <io/__/service/reader.hpp>
#include <io/__/service/writer.hpp>

#include <service/extension/config.hpp>
#include <service/extension/factory.hpp>
#include <service/op.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/selector.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace sight::module::ui::io
{

namespace io = sight::io;
//------------------------------------------------------------------------------

static  void append_extensions(
    const std::vector<sight::ui::dialog::location_base::filter_t>& _filters,
    std::vector<std::string>& _extensions
)
{
    for(const auto& filter : _filters)
    {
        std::istringstream wildcard_stream(filter.second);
        std::string wildcard;

        while(wildcard_stream >> wildcard)
        {
            std::erase(wildcard, '*');

            if(!wildcard.empty()
               && std::ranges::find(_extensions, wildcard) == _extensions.end())
            {
                _extensions.push_back(std::move(wildcard));
            }
        }
    }
}

namespace
{

struct service_filter
{
    sight::ui::dialog::location_base::filter_t filter;
    std::string service_id;
};

} // namespace
//------------------------------------------------------------------------------

selector::selector() :
    has_monitors(has_signals::signals()),
    m_sig_failed(new_signal<signals::failed_t>(signals::FAILED)),
    m_sig_succeeded(new_signal<signals::succeeded_t>(signals::SUCCEEDED)),
    m_slot_forward_notification(new_slot(slots::FORWARD_NOTIFICATION, &selector::forward_notification, this))
{
}

//------------------------------------------------------------------------------

void selector::configuring()
{
    const config_t srv_config = this->get_config();

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
    // Run notification forwarding on the default worker so it is processed while the reader/writer is running.
    m_slot_forward_notification->set_worker(sight::core::thread::get_default_worker());
}

//------------------------------------------------------------------------------

void selector::stopping()
{
}

//------------------------------------------------------------------------------

io::service::reader::sptr selector::create_and_configure_reader(const std::string& _service_id)
{
    auto reader = service::add<io::service::reader>(_service_id);

    {
        auto obj = m_read.lock().get_shared();
        SIGHT_ASSERT(
            "The inout key '" + io::service::READER_DATA_KEY + "' is not correctly set.",
            obj
        );
        reader->set_inout(obj, io::service::READER_DATA_KEY);
    }

    reader->set_worker(this->worker());

    if(m_service_to_config.contains(_service_id))
    {
        const auto srv_cfg = service::extension::config::get_default()->get_service_config(
            m_service_to_config.at(_service_id),
            _service_id
        );

        SIGHT_ASSERT(
            "No service configuration of type service::extension::config was found",
            !srv_cfg.empty()
        );

        reader->set_config(srv_cfg);
    }

    reader->configure();

    return reader;
}

//------------------------------------------------------------------------------

io::service::writer::sptr selector::create_and_configure_writer(const std::string& _service_id)
{
    auto writer = service::add<io::service::writer>(_service_id);

    {
        auto obj = m_write.lock().get_shared();
        SIGHT_ASSERT(
            "The input key '" + io::service::WRITER_DATA_KEY + "' is not correctly set.",
            obj
        );
        writer->set_input(obj, io::service::WRITER_DATA_KEY);
    }

    writer->set_worker(this->worker());

    if(m_service_to_config.contains(_service_id))
    {
        const auto srv_cfg = service::extension::config::get_default()->get_service_config(
            m_service_to_config.at(_service_id),
            _service_id
        );

        SIGHT_ASSERT(
            "No service configuration of type service::extension::config was found",
            !srv_cfg.empty()
        );

        writer->set_config(srv_cfg);
    }

    writer->configure();

    return writer;
}

//------------------------------------------------------------------------------

void selector::select_file_reader(const std::vector<std::pair<std::string, std::string> >& _available_services)
{
    std::vector<service_filter> supported_filters;
    std::vector<std::string> supported_extensions;
    std::size_t file_reader_count = 0;
    std::string single_file_reader_id;

    for(const auto& service : _available_services)
    {
        const auto& service_id = service.first;
        auto reader            = create_and_configure_reader(service_id);

        const auto path_type = reader->get_path_type();

        if(((path_type& io::service::file) != 0)
           || ((path_type& io::service::files) != 0))
        {
            ++file_reader_count;
            single_file_reader_id = service_id;
            const auto filters = reader->get_supported_extensions();
            append_extensions(filters, supported_extensions);

            for(const auto& filter : filters)
            {
                supported_filters.push_back({.filter = filter, .service_id = service_id});
            }
        }

        service::unregister_service(reader);
    }

    std::ranges::sort(supported_extensions);

    if(file_reader_count > 0 && !supported_extensions.empty())
    {
        // Reader-specific aggregate filters would hide formats exposed by the other readers
        std::erase_if(
            supported_filters,
            [](const service_filter& _filter)
            {
                return _filter.filter.first.starts_with("All supported");
            });

        std::string wildcard_list;
        for(const auto& extension : supported_extensions)
        {
            if(!wildcard_list.empty())
            {
                wildcard_list += ' ';
            }

            wildcard_list += '*' + extension;
        }

        supported_filters.push_back(
            {.filter     = {"All supported files", std::move(wildcard_list)},
             .service_id = file_reader_count == 1 ? single_file_reader_id : ""
            });
    }

    if(supported_filters.empty())
    {
        m_sig_failed->async_emit();
        return;
    }

    std::ranges::sort(supported_filters, {}, &service_filter::filter);
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog;

    dialog.set_title("Choose a file");
    dialog.set_default_location(default_directory);
    dialog.set_type(sight::ui::dialog::location::single_file);
    dialog.set_option(sight::ui::dialog::location::read);
    dialog.set_option(sight::ui::dialog::location::file_must_exist);

    for(const auto& filter_info : supported_filters)
    {
        dialog.add_filter(filter_info.filter.first, filter_info.filter.second);
    }

    const auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog.show());

    if(!result)
    {
        m_sig_failed->async_emit();
        return;
    }

    const auto selected_filter = dialog.get_current_filter();
    const auto service_it      = std::ranges::find(
        supported_filters,
        selected_filter,
        &service_filter::filter
    );

    if(service_it == supported_filters.end())
    {
        sight::ui::dialog::message::show(
            "Unsupported file filter",
            "The selected file filter is not supported."
        );

        m_sig_failed->async_emit();
        return;
    }

    const std::filesystem::path selected_file = result->get_file();

    default_directory->set_folder(selected_file.parent_path());
    dialog.save_default_location(default_directory);

    // Retrieve and configure the reader associated with the selected filter.
    std::string service_id = service_it->service_id;
    if(service_id.empty())
    {
        for(const auto& service : _available_services)
        {
            auto reader          = create_and_configure_reader(service.first);
            const auto path_type = reader->get_path_type();

            if(((path_type& io::service::file) != 0)
               || ((path_type& io::service::files) != 0))
            {
                const auto filters = reader->get_supported_extensions();
                std::vector<std::string> extensions;
                append_extensions(filters, extensions);
                const auto filename = selected_file.filename().string();

                if(std::ranges::any_of(
                       extensions,
                       [&filename](const std::string& _extension)
                    {
                        return filename.ends_with(_extension);
                    }))
                {
                    service_id = service.first;
                }
            }

            service::unregister_service(reader);

            if(!service_id.empty())
            {
                break;
            }
        }
    }

    if(service_id.empty())
    {
        sight::ui::dialog::message::show(
            "Unsupported file",
            "No reader supports the selected file extension."
        );

        m_sig_failed->async_emit();
        return;
    }

    auto reader = create_and_configure_reader(service_id);

    const auto path_type = reader->get_path_type();

    if((path_type& io::service::file) != 0)
    {
        reader->set_file(selected_file);
    }
    else if((path_type& io::service::files) != 0)
    {
        reader->set_files({selected_file});
    }
    else
    {
        SIGHT_WARN("The selected reader does not support files.");
        service::unregister_service(reader);
        m_sig_failed->async_emit();
        return;
    }

    if(const auto signal = reader->signal(
           core::notification::has_notifications::signals::NOTIFICATION_CREATED
    ); signal)
    {
        signal->connect(m_slot_forward_notification);
    }

    try
    {
        reader->start().get();

        {
            sight::ui::busy_cursor cursor;
            reader->update().get();
        }

        reader->stop().get();

        const bool failed = reader->has_failed();

        service::unregister_service(reader);

        if(failed)
        {
            m_sig_failed->async_emit();
        }
        else
        {
            m_sig_succeeded->async_emit();
        }
    }
    catch(const std::exception& e)
    {
        const std::string msg = "Failed to read : \n" + std::string(e.what());

        sight::ui::dialog::message::show("Reader Error", msg);

        reader->stop().get();
        service::unregister_service(reader);
        m_sig_failed->async_emit();
    }
}

//------------------------------------------------------------------------------

void selector::select_folder_reader(const std::vector<std::pair<std::string, std::string> >& _available_services)
{
    std::string extension_id = _available_services[0].first;

    bool extension_selection_is_canceled = false;

    if(_available_services.size() > 1)
    {
        std::vector<std::string> available_extensions_selector;
        available_extensions_selector.reserve(_available_services.size());

        for(const auto& service : _available_services)
        {
            available_extensions_selector.push_back(service.second);
        }

        sight::ui::dialog::selector picker;

        picker.set_title("Reader to use");
        picker.set_choices(available_extensions_selector);

        if(const auto& choices = picker.show(); !choices.empty())
        {
            const auto& choice = choices.front();

            bool extension_id_found = false;

            for(const auto& [service_id, description] : _available_services)
            {
                if(description == choice)
                {
                    extension_id       = service_id;
                    extension_id_found = true;
                    break;
                }
            }

            if(!extension_id_found)
            {
                m_sig_failed->async_emit();
                return;
            }
        }
        else
        {
            extension_selection_is_canceled = true;
        }
    }

    if(extension_selection_is_canceled)
    {
        m_sig_failed->async_emit();
        return;
    }

    auto reader = create_and_configure_reader(extension_id);

    if(const auto signal = reader->signal(
           core::notification::has_notifications::signals::NOTIFICATION_CREATED
    ); signal)
    {
        signal->connect(m_slot_forward_notification);
    }

    try
    {
        reader->start().get();

        reader->open_location_dialog();

        {
            sight::ui::busy_cursor cursor;
            reader->update().get();
        }

        reader->stop().get();

        const bool failed = reader->has_failed();

        service::unregister_service(reader);

        if(failed)
        {
            m_sig_failed->async_emit();
        }
        else
        {
            m_sig_succeeded->async_emit();
        }
    }
    catch(const std::exception& e)
    {
        const std::string msg = "Failed to read : \n" + std::string(e.what());

        sight::ui::dialog::message::show("Reader Error", msg);

        reader->stop().get();
        service::unregister_service(reader);
        m_sig_failed->async_emit();
    }
}

//------------------------------------------------------------------------------

void selector::update_reader(const std::vector<std::pair<std::string, std::string> >& _available_services)
{
    if(_available_services.empty())
    {
        m_sig_failed->async_emit();
        return;
    }

    bool has_file_reader = false;

    for(const auto& service : _available_services)
    {
        const auto& service_id = service.first;
        auto reader            = create_and_configure_reader(service_id);

        const auto path_type = reader->get_path_type();

        if(((path_type& io::service::file) != 0)
           || ((path_type& io::service::files) != 0))
        {
            has_file_reader = true;
        }

        service::unregister_service(reader);

        if(has_file_reader)
        {
            break;
        }
    }

    if(has_file_reader)
    {
        select_file_reader(_available_services);
    }
    else
    {
        select_folder_reader(_available_services);
    }
}

//------------------------------------------------------------------------------

void selector::update_writer(
    const std::vector<std::pair<std::string, std::string> >& _available_extensions_map,
    const std::vector<std::string>& _available_extensions_selector
)
{
    std::vector<service_filter> supported_filters;
    bool has_file_writer              = false;
    bool all_file_writers_are_exposed = true;

    for(const auto& service : _available_extensions_map)
    {
        const auto& service_id = service.first;
        auto writer            = create_and_configure_writer(service_id);
        const auto path_type   = writer->get_path_type();

        if((path_type& io::service::file) != 0)
        {
            has_file_writer = true;
            const auto filters = writer->get_supported_extensions();

            if(filters.empty())
            {
                all_file_writers_are_exposed = false;
            }
            else
            {
                for(const auto& filter : filters)
                {
                    supported_filters.push_back({.filter = filter, .service_id = service_id});
                }
            }
        }

        service::unregister_service(writer);
    }

    if(has_file_writer && all_file_writers_are_exposed && !supported_filters.empty())
    {
        static auto default_directory = std::make_shared<core::location::single_folder>();

        sight::ui::dialog::location dialog;
        dialog.set_title("Choose a file");
        dialog.set_default_location(default_directory);
        dialog.set_type(sight::ui::dialog::location::single_file);
        dialog.set_option(sight::ui::dialog::location::write);

        std::ranges::sort(supported_filters, {}, &service_filter::filter);

        for(const auto& filter_info : supported_filters)
        {
            dialog.add_filter(filter_info.filter.first, filter_info.filter.second);
        }

        const auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog.show());
        if(!result)
        {
            m_sig_failed->async_emit();
            return;
        }

        const auto selected_filter = dialog.get_current_filter();
        const auto service_it      = std::ranges::find(
            supported_filters,
            selected_filter,
            &service_filter::filter
        );
        const auto selected_extensions = dialog.get_selected_extensions();
        if(service_it == supported_filters.end() || selected_extensions.empty())
        {
            m_sig_failed->async_emit();
            return;
        }

        auto selected_file = result->get_file();

        // If the user omitted the suffix, use the first extension from the
        // selected filter, as the former writer-specific dialogs did.
        if(!std::ranges::any_of(
               selected_extensions,
               [&selected_file](const std::string& _extension)
            {
                return selected_file.filename().string().ends_with(_extension);
            }))
        {
            selected_file += selected_extensions.front();
        }

        default_directory->set_folder(selected_file.parent_path());
        dialog.save_default_location(default_directory);

        auto writer = create_and_configure_writer(service_it->service_id);
        writer->set_file(selected_file);

        if(const auto signal = writer->signal(
               core::notification::has_notifications::signals::NOTIFICATION_CREATED
        ); signal)
        {
            signal->connect(m_slot_forward_notification);
        }

        try
        {
            writer->start().get();

            {
                sight::ui::busy_cursor cursor;
                writer->update().get();
            }

            writer->stop().get();
            const bool failed = writer->has_failed();
            service::unregister_service(writer);

            if(failed)
            {
                m_sig_failed->async_emit();
            }
            else
            {
                m_sig_succeeded->async_emit();
            }
        }
        catch(const std::exception& e)
        {
            const std::string msg = "Failed to write : \n" + std::string(e.what());
            sight::ui::dialog::message::show("Writer Error", msg);
            writer->stop().get();
            service::unregister_service(writer);
            m_sig_failed->async_emit();
        }

        return;
    }

    std::string extension_id = _available_extensions_map[0].first;

    bool extension_selection_is_canceled = false;

    if(_available_extensions_selector.size() > 1)
    {
        sight::ui::dialog::selector picker;

        picker.set_title("Writer to use");
        picker.set_choices(_available_extensions_selector);

        if(const auto& choices = picker.show(); !choices.empty())
        {
            const auto& choice      = choices.front();
            bool extension_id_found = false;

            for(const auto& [service_id, description] : _available_extensions_map)
            {
                if(description == choice)
                {
                    extension_id       = service_id;
                    extension_id_found = true;
                    break;
                }
            }

            if(!extension_id_found)
            {
                m_sig_failed->async_emit();
                return;
            }
        }
        else
        {
            extension_selection_is_canceled = true;
        }
    }

    if(extension_selection_is_canceled)
    {
        m_sig_failed->async_emit();
        return;
    }

    auto writer = create_and_configure_writer(extension_id);

    if(const auto signal = writer->signal(
           core::notification::has_notifications::signals::NOTIFICATION_CREATED
    ); signal)
    {
        signal->connect(m_slot_forward_notification);
    }

    try
    {
        writer->start().get();

        writer->open_location_dialog();

        {
            sight::ui::busy_cursor cursor;
            writer->update().get();
        }

        writer->stop().get();
        const bool failed = writer->has_failed();
        service::unregister_service(writer);

        if(failed)
        {
            m_sig_failed->async_emit();
        }
        else
        {
            m_sig_succeeded->async_emit();
        }
    }
    catch(const std::exception& e)
    {
        const std::string msg = "Failed to write : \n" + std::string(e.what());

        sight::ui::dialog::message::show("Writer Error", msg);

        writer->stop().get();
        service::unregister_service(writer);
        m_sig_failed->async_emit();
    }
}

//------------------------------------------------------------------------------

void selector::updating()
{
    std::vector<std::string> available_services_id;
    const auto read      = m_read.lock().get_shared();
    const auto write     = m_write.lock().get_shared();
    const bool is_reader = read != nullptr;

    SIGHT_ASSERT(
        "Exactly one of the keys 'data.read' or 'data.write' must be defined.",
        is_reader != (write != nullptr)
    );

    {
        const auto& obj = is_reader ? read : write;

        if(is_reader)
        {
            available_services_id =
                service::extension::factory::get()
                ->get_implementation_id_from_object_and_type(
                    obj->get_classname(),
                    "sight::io::service::reader"
                );
        }
        else
        {
            available_services_id =
                service::extension::factory::get()
                ->get_implementation_id_from_object_and_type(
                    obj->get_classname(),
                    "sight::io::service::writer"
                );
        }
    }

    std::vector<std::pair<std::string, std::string> > available_services;

    for(const std::string& service_id : available_services_id)
    {
        const bool service_is_selected =
            std::ranges::find(m_selected_services, service_id) != m_selected_services.end();

        if((m_services_are_excluded && !service_is_selected)
           || (!m_services_are_excluded && service_is_selected))
        {
            std::string description = service::extension::factory::get()->get_service_description(service_id);

            const auto config_it = m_service_to_config.find(service_id);

            if(config_it != m_service_to_config.end())
            {
                description = service::extension::config::get_default()->get_config_desc(config_it->second);
            }

            if(description.empty())
            {
                description = service_id;
            }

            available_services.emplace_back(service_id, description);
        }
    }

    if(available_services.empty())
    {
        SIGHT_WARN("selector::load : available services is empty.");

        sight::ui::dialog::message message_box;

        if(is_reader)
        {
            message_box.set_title("Reader not found");
            message_box.set_message("There are no available readers for this data type.");
        }
        else
        {
            message_box.set_title("Writer not found");
            message_box.set_message("There are no available writers for this data type.");
        }

        message_box.set_icon(sight::ui::dialog::message::warning);
        message_box.add_button(sight::ui::dialog::message::ok);
        message_box.show();

        m_sig_failed->async_emit();
        return;
    }

    if(is_reader)
    {
        this->update_reader(available_services);
    }
    else
    {
        std::vector<std::string> available_extensions_selector;

        available_extensions_selector.reserve(available_services.size());
        for(const auto& [service_id, description] : available_services)
        {
            available_extensions_selector.push_back(description);
        }

        this->update_writer(available_services, available_extensions_selector);
    }
}

//------------------------------------------------------------------------------

void selector::info(std::ostream& _sstream)
{
    // Write the service name.
    _sstream << "selector";
}

//------------------------------------------------------------------------------

void selector::forward_notification(core::notification::base::sptr _notification)
{
    this->async_emit(core::notification::has_notifications::signals::NOTIFICATION_CREATED, _notification);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::io
