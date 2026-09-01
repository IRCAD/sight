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

    const auto available_services =
        sight::service::extension::factory::get()
        ->get_implementation_id_from_object_and_type(
            _data->get_classname(),
            "sight::io::service::reader"
        );

    return read_paths(_paths, _data, _notification_slot, available_services);
}

//-----------------------------------------------------------------------------

bool read_paths(
    const std::vector<std::filesystem::path>& _paths,
    const sight::data::object::sptr& _data,
    const sight::core::com::slot_base::sptr& _notification_slot,
    const std::vector<std::string>& _available_services
)
{
    SIGHT_ASSERT("The data object is not initialized.", _data);

    bool success = true;

    const auto supports_path = [](const sight::io::service::reader::sptr& _reader,
                                  const std::filesystem::path& _path)
                               {
                                   if(!std::filesystem::is_regular_file(_path))
                                   {
                                       return false;
                                   }

                                   const auto extensions = _reader->get_supported_extensions();
                                   const auto filename   = _path.filename().string();

                                   return std::ranges::any_of(
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
                               };

    const auto run_reader = [&_notification_slot](
        const sight::io::service::reader::sptr& _reader,
        const sight::core::thread::worker::sptr& _worker,
        const std::filesystem::path& _path
                             )
                            {
                                if(_notification_slot != nullptr)
                                {
                                    if(const auto signal = _reader->signal(
                                           sight::core::notification::has_monitors::signals::NOTIFICATION_CREATED
                                    ); signal)
                                    {
                                        signal->connect(_notification_slot);
                                    }
                                }

                                bool reader_success = true;

                                try
                                {
                                    _reader->start().get();
                                    _reader->update().get();

                                    if(_reader->has_failed())
                                    {
                                        SIGHT_WARN("Failed to read path: " << _path);
                                        reader_success = false;
                                    }

                                    _reader->stop().get();
                                }
                                catch(const std::exception& e)
                                {
                                    SIGHT_ERROR(
                                        "Failed to read path '" << _path << "': " << e.what()
                                    );

                                    reader_success = false;

                                    if(!_reader->stopped())
                                    {
                                        _reader->stop().get();
                                    }
                                }

                                sight::service::unregister_service(_reader);
                                _worker->stop();

                                return reader_success;
                            };

    if(_paths.size() > 1)
    {
        for(const auto& service_id : _available_services)
        {
            auto worker = sight::core::thread::worker::make();
            sight::io::service::reader::sptr reader;

            try
            {
                reader = sight::service::add<sight::io::service::reader>(service_id);
                reader->set_worker(worker);
                reader->set_inout(_data, sight::io::service::READER_DATA_KEY);
                reader->configure();
            }
            catch(const std::exception& e)
            {
                SIGHT_WARN("Unable to configure reader '" << service_id << "': " << e.what());
                if(reader)
                {
                    sight::service::unregister_service(reader);
                }

                worker->stop();
                continue;
            }

            if((reader->get_path_type() & sight::io::service::files) != 0
               && std::ranges::all_of(
                   _paths,
                   [&supports_path, &reader](const std::filesystem::path& _path)
                {
                    return supports_path(reader, _path);
                }))
            {
                reader->set_files(_paths);
                return run_reader(reader, worker, _paths.front());
            }

            sight::service::unregister_service(reader);
            worker->stop();
        }
    }

    for(const auto& path : _paths)
    {
        sight::io::service::reader::sptr selected_reader;
        sight::core::thread::worker::sptr selected_worker;

        for(const auto& service_id : _available_services)
        {
            auto worker = sight::core::thread::worker::make();

            sight::io::service::reader::sptr reader;

            try
            {
                reader = sight::service::add<sight::io::service::reader>(service_id);
                reader->set_worker(worker);
                reader->set_inout(_data, sight::io::service::READER_DATA_KEY);
                reader->configure();
            }
            catch(const std::exception& e)
            {
                SIGHT_WARN("Unable to configure reader '" << service_id << "': " << e.what());
                if(reader)
                {
                    sight::service::unregister_service(reader);
                }

                worker->stop();
                continue;
            }

            const auto path_type = reader->get_path_type();

            bool supported = false;

            if(std::filesystem::is_directory(path) && (path_type& sight::io::service::folder) != 0)
            {
                reader->set_folder(path);
                supported = true;
            }
            else if(std::filesystem::is_regular_file(path)
                    && ((path_type& sight::io::service::file) != 0
                        || (path_type& sight::io::service::files) != 0))
            {
                supported = supports_path(reader, path);

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

        success = run_reader(selected_reader, selected_worker, path) && success;
    }

    return success;
}

} // namespace sight::io::reader
