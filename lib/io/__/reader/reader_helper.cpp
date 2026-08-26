/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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
#include "reader_helper.hpp"

#include <core/notification/has_notifications.hpp>

#include <io/__/service/reader.hpp>

#include <service/extension/factory.hpp>
#include <service/op.hpp>

#include <algorithm>
#include <filesystem>
#include <sstream>

namespace sight::io::reader
{

//-----------------------------------------------------------------------------

bool read_paths(
    const std::vector<std::filesystem::path>& _paths,
    const sight::data::object::sptr& _data,
    const sight::core::com::slot_base::sptr& _notification_slot
)
{
    SIGHT_ASSERT("The data object is not initialized.", _data);

    const std::string object_classname = _data->get_classname();

    const auto available_services =
        sight::service::extension::factory::get()
        ->get_implementation_id_from_object_and_type(
            object_classname,
            "sight::io::service::reader"
        );

    bool success = true;

    for(const auto& path : _paths)
    {
        sight::io::service::reader::sptr selected_reader;
        sight::core::thread::worker::sptr selected_worker;

        for(const auto& service_id : available_services)
        {
            auto worker = sight::core::thread::worker::make();

            auto reader =
                sight::service::add<sight::io::service::reader>(service_id);

            reader->set_worker(worker);
            reader->set_inout(_data, sight::io::service::READER_DATA_KEY);
            reader->configure();

            const auto path_type = reader->get_path_type();

            bool supported = false;

            if(std::filesystem::is_directory(path)
               && (path_type& sight::io::service::folder) != 0)
            {
                reader->set_folder(path);
                supported = true;
            }
            else if(std::filesystem::is_regular_file(path)
                    && ((path_type& sight::io::service::file) != 0
                        || (path_type& sight::io::service::files) != 0))
            {
                const auto extensions = reader->get_supported_extensions();
                const auto filename   = path.filename().string();

                supported = std::ranges::any_of(
                    extensions,
                    [&filename](const std::pair<std::string, std::string>& _extension)
                    {
                        std::istringstream wildcard_stream(_extension.second);
                        std::string wildcard;

                        while(wildcard_stream >> wildcard)
                        {
                            std::erase(wildcard, '*');

                            if(!wildcard.empty() && filename.ends_with(wildcard))
                            {
                                return true;
                            }
                        }

                        return false;
                    });

                if(supported)
                {
                    if((path_type& sight::io::service::file) != 0)
                    {
                        reader->set_file(path);
                    }
                    else
                    {
                        reader->set_files({path});
                    }
                }
            }

            if(supported)
            {
                selected_reader = reader;
                selected_worker = worker;
                break;
            }

            sight::service::unregister_service(reader);
            worker->stop();
        }

        if(!selected_reader)
        {
            SIGHT_WARN("No reader found for path: " << path);
            success = false;
            continue;
        }

        if(_notification_slot != nullptr)
        {
            if(const auto signal = selected_reader->signal(
                   sight::core::notification::has_monitors::signals::NOTIFICATION_CREATED
            ); signal)
            {
                signal->connect(_notification_slot);
            }
        }

        try
        {
            selected_reader->start().get();
            selected_reader->update().get();

            if(selected_reader->has_failed())
            {
                SIGHT_WARN("Failed to read path: " << path);
                success = false;
            }

            selected_reader->stop().get();
        }
        catch(const std::exception& e)
        {
            SIGHT_ERROR(
                "Failed to read path '" << path << "': " << e.what()
            );

            success = false;

            if(!selected_reader->stopped())
            {
                selected_reader->stop().get();
            }
        }

        sight::service::unregister_service(selected_reader);

        selected_worker->stop();
    }

    return success;
}

} // namespace sight::io::reader
