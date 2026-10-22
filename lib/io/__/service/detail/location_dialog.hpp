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

#pragma once

#include "io/__/service/io_types.hpp"

#include <core/location/multiple_files.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/failed.hpp>

#include <ui/__/dialog/location.hpp>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace sight::io::service::detail
{

//------------------------------------------------------------------------------

template<typename IOService, typename FilterCallback>
void open_location_dialog(
    IOService& _service,
    const std::string& _title,
    bool _read,
    FilterCallback _on_filter_selected
)
{
    const auto path_type                    = _service.get_path_type();
    ui::dialog::location::types dialog_type = ui::dialog::location::single_file;

    if(path_type == io::service::file)
    {
        dialog_type = ui::dialog::location::single_file;
    }
    else if(path_type == io::service::files)
    {
        dialog_type = ui::dialog::location::multi_files;
    }
    else if(path_type == io::service::folder)
    {
        dialog_type = ui::dialog::location::folder;
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed("Cannot open a location dialog for the service path type.")
        );
    }

    ui::dialog::location dialog;
    dialog.set_title(_title);
    dialog.set_preferences_key(_service.get_classname());
    dialog.set_default_location(std::make_shared<core::location::single_folder>());
    dialog.set_type(dialog_type);
    dialog.set_option(_read ? ui::dialog::location::read : ui::dialog::location::write);
    if(_read)
    {
        dialog.set_option(ui::dialog::location::file_must_exist);
    }

    if(path_type != io::service::folder)
    {
        for(const auto& [description, wildcard] : _service.get_supported_extensions())
        {
            dialog.add_filter(description, wildcard);
        }
    }

    const auto selected_location = dialog.show();
    if(!selected_location)
    {
        _service.clear_locations();
        return;
    }

    if(path_type == io::service::file)
    {
        _on_filter_selected(dialog.get_selected_extensions());
    }

    std::filesystem::path selected_path;
    if(path_type == io::service::file)
    {
        const auto file = std::dynamic_pointer_cast<core::location::single_file>(selected_location);
        SIGHT_THROW_EXCEPTION_IF(core::tools::failed("The location dialog did not return a file."), !file);
        selected_path = file->get_file();
        _service.set_file(selected_path);
    }
    else if(path_type == io::service::files)
    {
        const auto files = std::dynamic_pointer_cast<core::location::multiple_files>(selected_location);
        SIGHT_THROW_EXCEPTION_IF(core::tools::failed("The location dialog did not return files."), !files);
        const auto& paths = files->get_files();
        if(paths.empty())
        {
            _service.clear_locations();
            return;
        }

        selected_path = paths.front();
        _service.set_files(paths);
    }
    else
    {
        const auto folder = std::dynamic_pointer_cast<core::location::single_folder>(selected_location);
        SIGHT_THROW_EXCEPTION_IF(core::tools::failed("The location dialog did not return a folder."), !folder);
        selected_path = folder->get_folder();
        _service.set_folder(selected_path);
    }

    auto default_location = std::make_shared<core::location::single_folder>();
    default_location->set_folder(selected_path.parent_path());
    dialog.save_default_location(default_location);
}

//------------------------------------------------------------------------------

template<typename IOService>
void open_location_dialog(IOService& _service, const std::string& _title, bool _read)
{
    open_location_dialog(_service, _title, _read, [](const std::vector<std::string>&){});
}

} // namespace sight::io::service::detail
