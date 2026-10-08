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

#include "io/__/service/reader.hpp"

#include <core/runtime/path.hpp>

#include <service/base.hpp>

#include <algorithm>

namespace sight::io::service
{

//-----------------------------------------------------------------------------

reader::reader(const std::string& _default_window_title) noexcept :
    has_monitors(has_signals::signals()),
    m_window_title(this, WINDOW_TITLE_KEY, _default_window_title)
{
    new_signal<signals::void_t>(signals::FAILED);
    new_signal<signals::void_t>(signals::SUCCEEDED);

    new_slot(slots::OPEN_LOCATION_DIALOG, &reader::open_location_dialog, this);
    new_slot(slots::UPDATE_DEFAULT_LOCATIONS, &reader::update_default_locations, this);
}

//-----------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string> > reader::get_supported_extensions()
{
    return {};
}

//-----------------------------------------------------------------------------

const std::filesystem::path& reader::get_file() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::file));
    SIGHT_THROW_IF("Exactly one file must be defined in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void reader::set_file(const std::filesystem::path& _file)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::file));
    m_locations.clear();
    m_locations.push_back(_file);
}

//-----------------------------------------------------------------------------

const io::service::locations_t& reader::get_files() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::files));
    SIGHT_THROW_IF("At least one file must be define in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void reader::set_files(const io::service::locations_t& _files)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->get_path_type() & io::service::files));
    m_locations = _files;
}

//-----------------------------------------------------------------------------

const std::filesystem::path& reader::get_folder() const
{
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->get_path_type() & io::service::folder));
    SIGHT_THROW_IF("Exactly one folder must be define in location", m_locations.size() != 1);
    return m_locations.front();
}

//-----------------------------------------------------------------------------

void reader::set_folder(const std::filesystem::path& _folder)
{
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->get_path_type() & io::service::folder));
    m_locations.clear();
    m_locations.push_back(_folder);
}

//-----------------------------------------------------------------------------

const io::service::locations_t& reader::get_locations() const
{
    SIGHT_THROW_IF("At least one path must be define in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void reader::clear_locations()
{
    m_locations.clear();
}

//-----------------------------------------------------------------------------

void reader::configuring()
{
    /// @todo check if this is still needed or useful or not a static assert at build time...
    SIGHT_ASSERT(
        "Generic configuring method is only available for io service that use paths.",
        !(this->get_path_type() & io::service::type_not_defined)
    );

    SIGHT_ASSERT(
        "This reader cannot manage both io::service::files and io::service::file.",
        (this->get_path_type() & io::service::files) == 0 || (this->get_path_type() & io::service::file) == 0
    );

    this->update_default_locations();
}

//------------------------------------------------------------------------------

sight::service::base::connections_t reader::auto_connections() const
{
    return {
        {m_files, data::signals::MODIFIED, slots::UPDATE_DEFAULT_LOCATIONS},
        {m_folder, data::signals::MODIFIED, slots::UPDATE_DEFAULT_LOCATIONS},
        {m_resources, data::signals::MODIFIED, slots::UPDATE_DEFAULT_LOCATIONS}
    };
}

//------------------------------------------------------------------------------

void reader::update_default_locations()
{
    io::service::locations_t files;

    if(const auto files_property = *m_files; !files_property.empty())
    {
        boost::split(files, files_property, boost::is_any_of(";"), boost::token_compress_on);
    }

    std::vector<std::string> resources;

    if(const auto resources_property = *m_resources; !resources_property.empty())
    {
        boost::split(resources, resources_property, boost::is_any_of(";"), boost::token_compress_on);
    }

    this->update_locations(files, *m_folder, resources);
}

//------------------------------------------------------------------------------

void reader::update_locations(
    const io::service::locations_t& _files,
    const std::filesystem::path& _folder,
    const std::vector<std::string>& _resources
)
{
    // Assertion checks
    SIGHT_ASSERT(
        "This reader does not manage files and a file path is given in the configuration",
        (this->get_path_type() & io::service::file || this->get_path_type() & io::service::files) || (_files.empty())
    );

    // Populate m_locations with files
    if((this->get_path_type() & io::service::file) != 0)
    {
        if(!_files.empty())
        {
            SIGHT_ASSERT(
                "This reader single file, but there is more than one file.",
                _files.size() == 1
            );

            this->set_file(_files.front());
        }
        else if(!_resources.empty())
        {
            SIGHT_ASSERT(
                "This reader single file, but there is more than one resource.",
                _resources.size() == 1
            );

            const auto file = core::runtime::get_resource_file_path(_resources.front());
            this->set_file(file);
        }
    }
    else if((this->get_path_type() & io::service::files) != 0)
    {
        io::service::locations_t files(_files);

        std::ranges::transform(
            _resources,
            std::back_inserter(files),
            [](const std::string& _resource)
            {
                return core::runtime::get_resource_file_path(_resource);
            });

        this->set_files(files);
    }

    // Populate m_locations with folder
    if((this->get_path_type() & io::service::folder) != 0)
    {
        if(!_folder.empty())
        {
            this->set_folder(_folder);
        }
        else if(!_resources.empty())
        {
            SIGHT_ASSERT(
                "This reader single folder, but there is more than one resource.",
                _resources.size() == 1
            );

            const auto& resource = _resources.front();

            // Look in the module
            auto folder = core::runtime::get_module_resource_file_path(resource);

            if(folder.empty())
            {
                // If not found in a module, look into libraries
                folder = core::runtime::get_library_resource_file_path(resource);
                SIGHT_ASSERT(
                    "Resource '" + resource + "' has not been found in any module or library",
                    !folder.empty()
                );
            }

            this->set_folder(folder);
        }
    }
}

//-----------------------------------------------------------------------------

io::service::path_type_t reader::get_path_type() const
{
    return io::service::type_not_defined;
}

//-----------------------------------------------------------------------------

bool reader::has_location_defined() const
{
    return !m_locations.empty();
}

//-----------------------------------------------------------------------------

bool reader::has_failed() const
{
    return m_read_failed;
}

} // namespace sight::io::service
