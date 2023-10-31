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

#include <core/com/signal.hpp>
#include <core/tools/failed.hpp>

#include <service/base.hpp>

#include <filesystem>

namespace sight::io::service
{

/**
 * @brief Writer service API. It manages extension points definition and extension configuration
 *
 * @section Signals Signals
 * - \b prefixSet(): Emitted when the prefix has been correctly updated,
 * - \b baseFolderSet(): Emitted When the base folder has been correctly updated.
 *
 * @section Slots Slots
 * - \b setPrefix(std::string): When connected to a string-emitting signal (or function),
 *   this slot will concatenate the string as a prefix of the output file/folder.
 * - \b setBaseFolder(std::string): When connected to a string-emitting signal (or function),
 *   this slot will set the base folder used by writers.
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
 * @li The service is configured with methods set_config(cfg) and configure()
 * @li The method start() initialize the service
 * @li To write the object use update() method
 * @li Finally we must call stop() before deleting the service
 */
class IO_CLASS_API writer : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(writer, sight::service::base);

    /// Enum to define a dialog policy
    enum class dialog_policy : uint8_t
    {
        never   = 0,  /// Never use show the dialog
        once    = 1,  /// Show only once, store the location as long as the service is started
        always  = 2,  /// Always show the location dialog
        invalid = 255 /// Used for error management
    };

    /**
     * @name Signals API
     * @{
     */

    IO_API static const core::com::signals::key_t PREFIX_SET_SIG;
    IO_API static const core::com::signals::key_t BASE_FOLDER_SET_SIG;
    using void_signal_t = core::com::signal<void ()>;

    /**
     * @name Slots API
     * @{
     */
    IO_API static const core::com::slots::key_t SET_PREFIX;
    IO_API static const core::com::slots::key_t SET_BASE_FOLDER;
    /// @}

    /**
     * @brief Configure the image path (by default does nothing).
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IO_API virtual void open_location_dialog() = 0;

    /**
     * @brief This method must be implemented by concrete service writers
     * that use path file system to write data.
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
    IO_API std::filesystem::path get_file() const;

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
    IO_API std::filesystem::path get_folder() const;

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
     * @brief Slot: Inserts a path prefix generated via a signal sent to the service.
     *
     */
    IO_API void set_prefix(std::string _prefix);

    /**
     * @brief Slot: Sets the output base folder.
     *
     */
    IO_API virtual void set_base_folder(std::string _path);

    /// Returns if a location has been defined ( by the configuration process or directly by user )
    IO_API bool has_location_defined() const;

    /// Returns if reading has been cancelled by user
    IO_API bool has_failed() const;

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

    IO_API writer() noexcept;

    IO_API ~writer() noexcept override;

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
    IO_API void configuring() override;

    /**
     * @brief Title of the window that will open when the `open_location_dialog` slot is called
     */
    std::string m_window_title;

    /// Defines whether writing was performed correctly, or if user has cancelled the process.
    bool m_write_failed {false};

    /// Generic input data
    data::ptr<data::object, data::access::in> m_data {this, sight::io::service::DATA_KEY};

private:

    /// Triggers an update of the base folder, and outputs it via a reference.
    // We need to check for potential updates, notably in the case
    // where the user updates an associated preference during runtime.
    void update_base_folder(std::string& /*outBaseFolder*/) const;

    /// Value to store file or folder paths
    io::service::locations_t m_locations;

    /// Prefix to be inserted
    std::string m_current_prefix;

    /// Base folder
    std::string m_base_folder;
};

} //namespace sight::io::service
