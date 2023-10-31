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
#include "io/__/service/io_types.hpp"

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
    enum class dialog_policy : uint8_t
    {
        never   = 0,  /// Never show the dialog
        once    = 1,  /// Show only once, store the location as long as the service is started
        always  = 2,  /// Always show the location dialog
        invalid = 255 /// Used for error management
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
    IO_API virtual void open_location_dialog() = 0;
    /**
     * @brief   returns  (filename) extension
     */
    IO_API virtual std::vector<std::string> get_supported_extensions();

    /**
     * @brief   returns  the title of selector dialog box
     */
    IO_API virtual std::string get_selector_dialog_title();

    /**
     * @brief This method must be implemented by concrete service readers
     * that use path file system to read data.
     *
     * This method returns supported path format (file, files or folder).
     * A reader can support file and folder, or files and folder, but not
     * file and files ( because files include file concept ).
     */
    IO_API virtual io::service::path_type_t get_path_type() const;

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
    IO_API void set_file(const std::filesystem::path& _file);

    /**
     * @brief Returns file paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FILES mode
     */
    IO_API const io::service::locations_t& get_files() const;

    /**
     * @brief Sets file paths
     * @pre exception if service does not support FILES mode
     */
    IO_API void set_files(const io::service::locations_t& _files);

    /**
     * @brief Returns folder path set by the user or set during service configuration
     * @pre exception if a folder path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FOLDER mode
     */
    IO_API const std::filesystem::path& get_folder() const;

    /**
     * @brief Clear any location set by the set_file/set_files/set_folder setter
     */
    IO_API void clear_locations();

    /**
     * @brief Returns file/files/folder paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     */
    IO_API const io::service::locations_t& get_locations() const;

    /**
     * @brief Sets folder path
     * @pre exception if service does not support FOLDER mode
     */
    IO_API void set_folder(const std::filesystem::path& _folder);

    /**
     * @brief Slot: Sets the folder when a path is configured in FILE or FILES mode
     * This is ignored if a path is not configured
     *
     * @pre exception if service does not support FILE or FILES mode
     */
    IO_API void set_file_folder(std::filesystem::path _folder);

    /// Returns if a location has been defined ( by the configuration process or directly by user )
    IO_API bool has_location_defined() const;

    /// Returns if reading has failed.
    IO_API bool has_failed() const;

    //@}

    /// Convenience function to convert from DialogPolicy enum value to string
    constexpr static std::string_view dialog_policy_to_string(dialog_policy _policy) noexcept
    {
        switch(_policy)
        {
            case dialog_policy::once:
                return "once";

            case dialog_policy::always:
                return "always";

            default:
                return "never";
        }
    }

    /// Convenience function to convert from string to DialogPolicy enum value
    constexpr static dialog_policy string_to_dialog_policy(std::string_view _policy) noexcept
    {
        if(constexpr auto never = dialog_policy_to_string(dialog_policy::never);
           _policy == never || _policy.empty() || _policy == "default")
        {
            return dialog_policy::never;
        }

        if(constexpr auto once = dialog_policy_to_string(dialog_policy::once); _policy == once)
        {
            return dialog_policy::once;
        }

        if(constexpr auto always = dialog_policy_to_string(dialog_policy::always); _policy == always)
        {
            return dialog_policy::always;
        }

        // Error case
        return dialog_policy::invalid;
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
     * @brief Title of the window that will open when the `open_location_dialog` slot is called
     */
    std::string m_window_title;

    /// Defines whether reading was performed correctly, or if it has failed or if user has cancelled the process.
    bool m_read_failed {false};

    /// Generic output data
    data::ptr<data::object, data::access::inout> m_data {this, sight::io::service::DATA_KEY};

private:

    /// Slot to read folder
    void read_folder(std::filesystem::path _folder);
    /// Slot to read file
    void read_file(std::filesystem::path _file);
    /// Slot to read files
    void read_files(io::service::locations_t _files);

    /// Value to stock file or folder paths
    io::service::locations_t m_locations;
};

} //namespace sight::io::service
