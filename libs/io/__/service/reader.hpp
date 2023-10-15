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

#pragma once

#include "io/__/config.hpp"
#include "io/__/service/ioTypes.hpp"

#include <core/tools/failed.hpp>

#include <service/base.hpp>

#include <filesystem>

namespace sight::io::service
{

/**
 * @brief Reader service API. It manages extension points definition and extension configuration
 *
 * @section Slots Slots
 * - \b setFileFolder(const std::filesystem::path&) : Sets the folder when a path is configured in FILE or
 * FILES mode
 *
 * This class represents the base interface for reader services.
 * Use the base service methods :
 * @li The service is configured with methods set_config(cfg) and configure()
 * @li The method start() initialize the service
 * @li To read the object use update() method
 * @li Finally we must call stop() before deleting the service
 * @todo ACH : remove some methods : getSupportedExtensions ? getSelectorDialogTitle ?
 */
class IO_CLASS_API reader : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(reader, sight::service::base);

    /// Enum to define a password policy
    enum class DialogPolicy : uint8_t
    {
        NEVER   = 0,  /// Never show the dialog
        ONCE    = 1,  /// Show only once, store the location as long as the service is started
        ALWAYS  = 2,  /// Always show the location dialog
        INVALID = 255 /// Used for error management
    };

    /**
     * @name Slots API
     * @{
     */
    IO_API static const core::com::slots::key_t SET_FILE_FOLDER;
    /// @}

    /**
     * @name    Specific service methods for reading
     */
    //@{

    /**
     * @brief Configure the image path (by default does nothing).
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IO_API virtual void openLocationDialog() = 0;
    /**
     * @brief   returns  (filename) extension
     */
    IO_API virtual std::vector<std::string> getSupportedExtensions();

    /**
     * @brief   returns  the title of selector dialog box
     */
    IO_API virtual std::string getSelectorDialogTitle();

    /**
     * @brief This method must be implemented by concrete service readers
     * that use path file system to read data.
     *
     * This method returns supported path format (file, files or folder).
     * A reader can support file and folder, or files and folder, but not
     * file and files ( because files include file concept ).
     */
    IO_API virtual io::service::IOPathType getIOPathType() const;

    /**
     * @brief Returns the file path set by the user or set during service configuration
     * @pre exception if a file path is not defined  ( m_locations.empty() )
     * @pre exception if service does not support FILE mode
     */
    IO_API const std::filesystem::path& get_file() const;

    /**
     * @brief Sets file path
     * @pre exception if service does not support FILE mode
     */
    IO_API void set_file(const std::filesystem::path& file);

    /**
     * @brief Returns file paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FILES mode
     */
    IO_API const io::service::LocationsType& get_files() const;

    /**
     * @brief Sets file paths
     * @pre exception if service does not support FILES mode
     */
    IO_API void set_files(const io::service::LocationsType& files);

    /**
     * @brief Returns folder path set by the user or set during service configuration
     * @pre exception if a folder path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FOLDER mode
     */
    IO_API const std::filesystem::path& get_folder() const;

    /**
     * @brief Clear any location set by the set_file/set_files/set_folder setter
     */
    IO_API void clearLocations();

    /**
     * @brief Returns file/files/folder paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     */
    IO_API const io::service::LocationsType& getLocations() const;

    /**
     * @brief Sets folder path
     * @pre exception if service does not support FOLDER mode
     */
    IO_API void set_folder(const std::filesystem::path& folder);

    /**
     * @brief Slot: Sets the folder when a path is configured in FILE or FILES mode
     * This is ignored if a path is not configured
     *
     * @pre exception if service does not support FILE or FILES mode
     */
    IO_API void setFileFolder(std::filesystem::path folder);

    /// Returns if a location has been defined ( by the configuration process or directly by user )
    IO_API bool hasLocationDefined() const;

    /// Returns if reading has failed.
    IO_API bool hasFailed() const;

    //@}

    /// Convenience function to convert from DialogPolicy enum value to string
    constexpr static std::string_view dialogPolicyToString(DialogPolicy policy) noexcept
    {
        switch(policy)
        {
            case DialogPolicy::ONCE:
                return "once";

            case DialogPolicy::ALWAYS:
                return "always";

            default:
                return "never";
        }
    }

    /// Convenience function to convert from string to DialogPolicy enum value
    constexpr static DialogPolicy stringToDialogPolicy(std::string_view policy) noexcept
    {
        if(constexpr auto NEVER = dialogPolicyToString(DialogPolicy::NEVER);
           policy == NEVER || policy.empty() || policy == "default")
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

        // Error case
        return DialogPolicy::INVALID;
    }

protected:

    IO_API reader() noexcept;

    IO_API ~reader() noexcept override;

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
     *  <service ... >
     *      <file>/home/user/myFile.jpg</file>
     *  </service>
     * @endcode
     * Sample configuration for many files:
     * @code{.xml}
     *  <service ... >
     *      <file>/home/user/myFile01.jpg</file>
     *      <file>/home/user/myFile02.jpg</file>
     *      <file>/home/user/myFile03.jpg</file>
     *  </service>
     * @endcode
     * Sample configuration for a folder:
     * @code{.xml}
     *  <service ... >
     *      <folder>/home/user/myFolder</folder>
     *  </service>
     * @endcode
     * Sample configuration for a resource (file stored in a module or a library):
     * @code{.xml}
     *  <service ... >
     *      <resource>module/myFile.jpg</resource>
     *  </service>
     * Sample configuration for many resources (files stored in a module or a library):
     * @code{.xml}
     *  <service ... >
     *      <resource>module/myFile01.jpg</resource>
     *      <resource>module/myFile02.jpg</resource>
     *      <resource>library/myFile03.jpg</resource>
     *  </service>
     * Sample configuration for a resource folder (module or library path):
     * @code{.xml}
     *  <service ... >
     *      <resource>module</resource>
     *  </service>
     *
     * You may specify the title of the modal file selection window with the `windowTitle` config attribute as such:
     * @code{.xml}
     *  <service ... >
     *      <windowTitle>Example window title</windowTitle>
     *  </service>
     * @endcode
     */
    IO_API void configuring() override;

    /**
     * @brief Title of the window that will open when the `openLocationDialog` slot is called
     */
    std::string m_windowTitle;

    /// Defines whether reading was performed correctly, or if it has failed or if user has cancelled the process.
    bool m_readFailed {false};

    /// Generic output data
    data::ptr<data::object, data::Access::inout> m_data {this, sight::io::service::s_DATA_KEY};

private:

    /// Slot to read folder
    void readFolder(std::filesystem::path folder);
    /// Slot to read file
    void readFile(std::filesystem::path file);
    /// Slot to read files
    void readFiles(io::service::LocationsType files);

    /// Value to stock file or folder paths
    io::service::LocationsType m_locations;
};

} //namespace sight::io::service
