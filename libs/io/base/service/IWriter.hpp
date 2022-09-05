/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include "io/base/config.hpp"
#include "io/base/service/ioTypes.hpp"

#include <core/com/Signal.hpp>

#include <service/IService.hpp>

#include <filesystem>

namespace sight::io::base::service
{

/**
 * @brief Writer service API. It manages extension points definition and extension configuration
 *
 * @section Slots Slots
 * - \b setPrefix(std::string): When connected to a string-emitting signal (or function),
 *   this slot will concatenate the string as a prefix of the output file/folder.
 *
 * @subsection Configuration Configuration
 * - \b file: default file path.
 * - \b folder: default folder path.
 * - \b baseFolder: path or preference key specifying the root output path.
 *      If set, it will override user setting the path via GUI input,
 *      leading to a service that automatically saves data without user queries.
 *      If the output is a file, then the output will be built as follows:
 *        <base_folder>(/<prefix>)^{0,1}/<file>, where file is the value in the file tag.
 *      If the output is a folder, then the output will be built as follows:
 *        <base_folder>(/<prefix>)^{0,1}/<folder>, where folder is the value in the folder tag.
 *
 * This class represents the base interface for writer services.
 * Use the base service methods :
 * @li The service is configured with methods setConfiguration(cfg) and configure()
 * @li The method start() initialize the service
 * @li To write the object use update() method
 * @li Finally we must call stop() before deleting the service
 */
class IO_BASE_CLASS_API IWriter : public sight::service::IService
{
public:

    SIGHT_DECLARE_SERVICE(IWriter, sight::service::IService);

    /// Enum to define a dialog policy
    enum class DialogPolicy : uint8_t
    {
        NEVER   = 0,     /// Never use show the dialog
        ONCE    = 1,     /// Show only once, store the location as long as the service is started
        ALWAYS  = 2,     /// Always show the location dialog
        DEFAULT = NEVER, /// Default behavior if nothing is set
        INVALID = 255    /// Used for error management
    };

    /**
     * @name Signals API
     * @{
     */

    IO_BASE_API static const core::com::Signals::SignalKeyType s_PREFIX_SET_SIG;
    typedef core::com::Signal<void ()> PrefixSetSignalType;

    /**
     * @name Slots API
     * @{
     */
    IO_BASE_API static const core::com::Slots::SlotKeyType s_SET_PREFIX;
    /// @}

    /**
     * @brief Configure the image path (by default does nothing).
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IO_BASE_API virtual void openLocationDialog() = 0;

    /**
     * @brief This method must be implemented by concrete service writers
     * that use path file system to write data.
     *
     * This method returns supported path format (file, files or folder).
     * A reader can support file and folder, or files and folder, but not
     * file and files ( because files include file concept ).
     */
    IO_BASE_API virtual io::base::service::IOPathType getIOPathType() const;

    /**
     * @brief Returns the file path set by the user or set during service configuration
     * @pre exception if a file path is not defined  ( m_locations.empty() )
     * @pre exception if service does not support FILE mode
     */
    IO_BASE_API std::filesystem::path getFile() const;

    /**
     * @brief Sets file path
     * @pre exception if service does not support FILE mode
     */
    IO_BASE_API void setFile(const std::filesystem::path& file);

    /**
     * @brief Returns file paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FILES mode
     */
    IO_BASE_API const io::base::service::LocationsType& getFiles() const;

    /**
     * @brief Sets file paths
     * @pre exception if service does not support FILES mode
     */
    IO_BASE_API void setFiles(const io::base::service::LocationsType& files);

    /**
     * @brief Returns folder path set by the user or set during service configuration
     * @pre exception if a folder path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FOLDER mode
     */
    IO_BASE_API std::filesystem::path getFolder() const;

    /**
     * @brief Clear any location set by the setFile/setFiles/setFolder setter
     */
    IO_BASE_API void clearLocations();

    /**
     * @brief Returns file/files/folder paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     */
    IO_BASE_API const io::base::service::LocationsType& getLocations() const;

    /**
     * @brief Sets folder path
     * @pre exception if service does not support FOLDER mode
     */
    IO_BASE_API void setFolder(const std::filesystem::path& folder);

    /**
     * @brief Slot: Inserts a path prefix generated via a signal sent to the service.
     *
     */
    IO_BASE_API void setPrefix(std::string prefix);

    /// Returns if a location has been defined ( by the configuration process or directly by user )
    IO_BASE_API bool hasLocationDefined() const;

    /// Returns if reading has been cancelled by user
    IO_BASE_API bool hasFailed() const;

    /// Convenience function to convert from DialogPolicy enum value to string
    constexpr static std::string_view dialogPolicyToString(DialogPolicy policy) noexcept
    {
        switch(policy)
        {
            case DialogPolicy::NEVER:
                return "never";

            case DialogPolicy::ONCE:
                return "once";

            case DialogPolicy::ALWAYS:
                return "always";

            default:
                return "default";
        }
    }

    /// Convenience function to convert from string to DialogPolicy enum value
    constexpr static DialogPolicy stringToDialogPolicy(std::string_view policy) noexcept
    {
        if(constexpr auto NEVER = dialogPolicyToString(DialogPolicy::NEVER); policy == NEVER)
        {
            return DialogPolicy::NEVER;
        }

        if(constexpr auto ONCE = dialogPolicyToString(DialogPolicy::ONCE); policy == ONCE)
        {
            return DialogPolicy::ONCE;
        }

        if(constexpr auto ALWAYS = dialogPolicyToString(DialogPolicy::ALWAYS); policy == ALWAYS)
        {
            return DialogPolicy::ALWAYS;
        }

        if(policy.empty() || policy == "default")
        {
            return DialogPolicy::DEFAULT;
        }

        // Error case
        return DialogPolicy::INVALID;
    }

protected:

    IO_BASE_API IWriter() noexcept;

    IO_BASE_API ~IWriter() noexcept override;

    /**
     * @brief This method proposes to parse xml configuration to retrieve
     * file/files/folder paths.
     *
     * You can implement your configuring method if you wan check another
     * information or information not correspond to a path on filesystem,
     * else you must use this generic approach.
     *
     * Sample configuration for a file:
     * @code{.xml}
       <service ... >
        <file>/home/user/myFile.jpg<file/>
       </service>
       @endcode
     * Sample configuration for many files:
     * @code{.xml}
       <service ... >
        <file>/home/user/myFile01.jpg<file/>
        <file>/home/user/myFile02.jpg<file/>
        <file>/home/user/myFile03.jpg<file/>
       </service>
       @endcode
     * Sample configuration for a folder:
     * @code{.xml}
       <service ... >
        <folder>/home/user/myFolder<folder/>
       </service>
       @endcode
     * You may specify the title of the modal file selection window with the `windowTitle` config attribute as such:
     * @code{.xml}
       <service ... >
        <windowTitle>Example window title</windowTitle>
       </service>
       @endcode
     */
    IO_BASE_API void configuring() override;

    /**
     * @brief Title of the window that will open when the `openLocationDialog` slot is called
     */
    std::string m_windowTitle;

    /// Defines whether writing was performed correctly, or if user has cancelled the process.
    bool m_writeFailed {false};

    /// Generic input data
    data::ptr<data::Object, data::Access::in> m_data {this, sight::io::base::service::s_DATA_KEY};

private:

    /// Triggers an update of the base folder, and outputs it via a reference.
    // We need to check for potential updates, notably in the case
    // where the user updates an associated preference during runtime.
    void updateBaseFolder(std::string& /*outBaseFolder*/) const;

    /// Value to store file or folder paths
    io::base::service::LocationsType m_locations;

    /// Prefix to be inserted
    std::string m_currentPrefix;
};

} //namespace sight::io::base::service
