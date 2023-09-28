/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "io/__/service/writer.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <ui/__/Preferences.hpp>

namespace sight::io::service
{

// Public signal
const core::com::signals::key_t writer::PREFIX_SET_SIG      = "prefixSet";
const core::com::signals::key_t writer::BASE_FOLDER_SET_SIG = "baseFolderSet";

// Public slot
const core::com::slots::key_t writer::SET_PREFIX      = "setPrefix";
const core::com::slots::key_t writer::SET_BASE_FOLDER = "setBaseFolder";

// Private slot
static const core::com::slots::key_t OPEN_LOCATION_DIALOG = "openLocationDialog";

//-----------------------------------------------------------------------------

writer::writer() noexcept
{
    new_signal<VoidSignalType>(PREFIX_SET_SIG);
    new_signal<VoidSignalType>(BASE_FOLDER_SET_SIG);

    new_slot(OPEN_LOCATION_DIALOG, &writer::openLocationDialog, this);
    new_slot(SET_PREFIX, &writer::setPrefix, this);
    new_slot(SET_BASE_FOLDER, &writer::setBaseFolder, this);
}

//-----------------------------------------------------------------------------

writer::~writer() noexcept =
    default;

//-----------------------------------------------------------------------------

std::filesystem::path writer::get_file() const
{
    // WARNING: Pay attention here that no canonical paths are processed when concatenating paths
    // In this case, the behavior is to replace the previous path.

    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::service::FILE));

    // Make sure the base folder is in sync (also re-reads file/folder entries to respect the developer choices)
    std::string baseFolder;
    this->updateBaseFolder(baseFolder);

    // Pre-pend the base folder by default, whether it is unset and empty
    // Or pre-filled by the user
    std::filesystem::path location = std::filesystem::path(baseFolder);

    // Only use the prefix if we have a base folder,
    // We don't want to modify an user defined path (set through the GUI)
    if(!baseFolder.empty() && !m_currentPrefix.empty())
    {
        location /= std::filesystem::path(m_currentPrefix);
    }

    SIGHT_THROW_IF("Exactly one file must be defined in location", m_locations.size() != 1);

    location /= m_locations.front();

    return location;
}

//-----------------------------------------------------------------------------

void writer::set_file(const std::filesystem::path& file)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::service::FILE));
    m_locations.clear();
    m_locations.push_back(file);
}

//-----------------------------------------------------------------------------

const io::service::LocationsType& writer::get_files() const
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::service::FILES));
    SIGHT_THROW_IF("At least one file must be defined in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void writer::set_files(const io::service::LocationsType& files)
{
    SIGHT_THROW_IF("This reader doesn't manage files", !(this->getIOPathType() & io::service::FILES));
    m_locations = files;
}

//-----------------------------------------------------------------------------

std::filesystem::path writer::get_folder() const
{
    // WARNING: Pay attention here that no canonical paths are processed when concatenating paths
    // In this case, the behavior is to replace the previous path.

    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->getIOPathType() & io::service::FOLDER));

    // Make sure the base folder is in sync (also re-reads file/folder entries to respect the developer choices)
    std::string baseFolder;
    this->updateBaseFolder(baseFolder);

    std::filesystem::path location = std::filesystem::path(baseFolder);

    // Only use the prefix if we have a base folder,
    // We don't want to modify an user defined path (set through the GUI)
    if(!baseFolder.empty() && !m_currentPrefix.empty())
    {
        location /= std::filesystem::path(m_currentPrefix);
    }

    location /= (m_locations.empty() ? "" : m_locations.front());

    return location;
}

//-----------------------------------------------------------------------------

void writer::set_folder(const std::filesystem::path& folder)
{
    SIGHT_THROW_IF("This reader doesn't manage folders", !(this->getIOPathType() & io::service::FOLDER));
    m_locations.clear();
    m_locations.push_back(folder);
}

//------------------------------------------------------------------------------

void writer::setPrefix(std::string prefix)
{
    // Record the current prefix
    m_currentPrefix = prefix;

    auto sig = this->signal<VoidSignalType>(PREFIX_SET_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void writer::setBaseFolder(std::string path)
{
    // Record the current prefix
    m_baseFolder = path;

    auto sig = this->signal<VoidSignalType>(BASE_FOLDER_SET_SIG);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

const io::service::LocationsType& writer::getLocations() const
{
    SIGHT_THROW_IF("At least one path must be defined in location", m_locations.empty());
    return m_locations;
}

//-----------------------------------------------------------------------------

void writer::clearLocations()
{
    // clearLocations is called to force the user reconfiguring a location
    // after a file has been written
    // When we use base folder, the location contains the "template" folder to be used to output data
    // So in this specific case, we don't clear it
    // if(m_baseFolder.size() == 0)
    {
        m_locations.clear();
    }
}

//-----------------------------------------------------------------------------

void writer::configuring()
{
    const auto& config = this->getConfiguration();

    SIGHT_ASSERT(
        "Generic configuring method is only available for io service that uses paths.",
        !(this->getIOPathType() & io::service::TYPE_NOT_DEFINED)
    );

    SIGHT_ASSERT(
        "This writer does not manage folders and a folder path is given in the configuration",
        (this->getIOPathType() & io::service::FOLDER)
        || (config.find("folder") == config.not_found())
    );

    SIGHT_ASSERT(
        "This writer does not manages files and a file path is given in the configuration",
        (this->getIOPathType() & io::service::FILE || this->getIOPathType() & io::service::FILES)
        || (config.find("file") == config.not_found())
    );

    m_windowTitle = config.get<std::string>("windowTitle", "");

    if((this->getIOPathType() & io::service::FILE) != 0)
    {
        SIGHT_THROW_IF("This reader cannot manage FILE and FILES.", this->getIOPathType() & io::service::FILES);
        SIGHT_THROW_IF("No more than one file must be defined in the configuration", config.count("file") > 1);
        if(const auto file = config.get_optional<std::string>("file"); file.has_value())
        {
            this->set_file(std::filesystem::path(*file));
        }
    }

    if((this->getIOPathType() & io::service::FILES) != 0)
    {
        SIGHT_THROW_IF("This reader cannot manage FILE and FILES.", this->getIOPathType() & io::service::FILE);
        io::service::LocationsType locations;
        for(const auto& elt : boost::make_iterator_range(config.equal_range("file")))
        {
            const auto location = elt.second.get_value<std::string>();
            locations.emplace_back(location);
        }

        this->set_files(locations);
    }

    if((this->getIOPathType() & io::service::FOLDER) != 0)
    {
        SIGHT_THROW_IF("No more than one folder must be defined in configuration", config.count("folder") > 1);
        const auto folder = config.get<std::string>("folder");
        this->set_folder(std::filesystem::path(folder));
    }
}

//------------------------------------------------------------------------------

void writer::updateBaseFolder(std::string& outBaseFolder) const
{
    const sight::service::config_t config = this->getConfiguration();

    std::string baseFolderCfg = m_baseFolder;

    if(baseFolderCfg.empty())
    {
        baseFolderCfg = config.get<std::string>("baseFolder", m_baseFolder);
    }

    if(!baseFolderCfg.empty())
    {
        // We need to check for potential updates to the base folder
        // In case the user updates an associated preference during runtime
        // We also re-read the xml configuration for file and folder if baseFolder was set

        sight::ui::Preferences preferences;

        // We need to provide the separator, otherwise the 2-args method will be selected
        auto [key, baseFolder] = preferences.parsed_get<std::string>(baseFolderCfg);

        // If the returned value is indeed a preference
        if(!key.empty())
        {
            if(!baseFolder.empty())
            {
                outBaseFolder = baseFolder;
            }
            else
            {
                SIGHT_INFO("No location defined for the preference " << baseFolderCfg);
            }
        }
        else
        {
            if(std::filesystem::exists(baseFolderCfg))
            {
                outBaseFolder = baseFolderCfg;
            }
            else
            {
                SIGHT_ERROR(baseFolderCfg << " is invalid. Reverting to classical selection.");
            }
        }
    }
}

//------------------------------------------------------------------------------

io::service::IOPathType writer::getIOPathType() const
{
    return io::service::TYPE_NOT_DEFINED;
}

//-----------------------------------------------------------------------------

bool writer::hasLocationDefined() const
{
    std::string baseFolder;
    updateBaseFolder(baseFolder);

    // Either we have an absolution location set by the user
    // Or we are using the baseFolder value from the xml configuration
    return ((baseFolder.empty()) && !m_locations.empty() && m_locations.front().is_absolute())
           || (!baseFolder.empty() && ((this->getIOPathType() & io::service::FILE) != 0)
               && !m_locations.empty())
           || (!baseFolder.empty() && ((this->getIOPathType() & io::service::FOLDER) != 0));
}

//-----------------------------------------------------------------------------

bool writer::hasFailed() const
{
    return m_writeFailed;
}

//-----------------------------------------------------------------------------

} // namespace sight::io::service
