/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <sight/io/__/config.hpp>

#include "io/__/service/io_types.hpp"

#include <core/com/signal.hpp>
#include <core/tools/failed.hpp>

#include <data/string.hpp>

#include <service/base.hpp>

#include <filesystem>

namespace sight::io::service
{

/**
 * @brief Writer service API. It manages extension points definition and extension configuration
 *
 * This abstract class defines the API for all writer services. It provides the basic methods to write data to files or
 * folders. The writer service can be configured using properties. Subclasses should implement the
 * open_location_dialog(), get_path_type() and get_supported_extensions() methods. In updating(), which should perform
 * the "write" from m_data object, m_write_failed must be set to the correct value.
 *
 * @section Signals Signals
 * - \b prefix_set(): Emitted when the prefix has been correctly updated,
 * - \b base_folder_set(): Emitted When the base folder has been correctly updated.
 *
 * @section Slots Slots
 * - \b open_location_dialog() : Open a dialog to select a file or a folder.
 * - \b set_prefix(std::string): When connected to a string-emitting signal (or function),
 *   this slot will concatenate the string as a prefix of the output file/folder.
 * - \b set_base_folder(std::string): When connected to a string-emitting signal (or function),
 *   this slot will set the base folder used by writers.
 *
 * @subsection Configuration Configuration
 * - \b file: @deprecated default file path.
 * - \b folder: @deprecated default folder path.
 * - \b baseFolder: path or preference key specifying the root output path.
 *      If set, it will override user setting the path via GUI input,
 *      leading to a service that automatically saves data without user queries.
 *      If the output is a file, then the output will be built as follows:
 *        <base_folder>(/<prefix>)^{0,1}/<file>, where file is the value in the file tag.
 *      If the output is a folder, then the output will be built as follows:
 *        <base_folder>(/<prefix>)^{0,1}/<folder>, where folder is the value in the folder tag.
 *
 * @subsection Properties Properties
 * - \b window_title (optional) : The window title that can be used for open_location_dialog. This abstract class
 *                   defines a default that can be overriden by calling the appropriate constructor, but the XML
 *                   property definition have the precedence in all cases.
 * - \b files : The file(s) to write. Depending of the path_type_t, it can be a single file or multiple files.
 * - \b folder : The folder to open. Used when the path_type_t is "folder".
 */
class SIGHT_IO_CLASS_API writer : public sight::service::base
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
     * @name Slots API
     * @{
     */
    struct slots
    {
        using key_t = sight::core::com::slots::key_t;
        static inline const key_t OPEN_LOCATION_DIALOG     = "open_location_dialog";
        static inline const key_t SET_PREFIX               = "set_prefix";
        static inline const key_t SET_BASE_FOLDER          = "set_base_folder";
        static inline const key_t UPDATE_DEFAULT_LOCATIONS = "update_default_locations";
    };
    //@}

    /**
     * @name Signals API
     * @{
     */
    struct signals
    {
        using void_signal_t = core::com::signal<void ()>;
        using key_t         = core::com::signals::key_t;
        static inline const key_t PREFIX_SET      = "prefix_set";
        static inline const key_t BASE_FOLDER_SET = "base_folder_set";
    };
    //@}

    /**
     * @brief Configure the image path (by default does nothing).
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    SIGHT_IO_API virtual void open_location_dialog() = 0;

    /**
     * @brief This method must be implemented by concrete service writers
     * that use path file system to write data.
     *
     * This method returns supported path format (file, files or folder).
     * A reader can support file and folder, or files and folder, but not
     * file and files ( because files include file concept ).
     */
    SIGHT_IO_API virtual io::service::path_type_t get_path_type() const;

    /**
     * @brief Returns the file path set by the user or set during service configuration
     * @pre exception if a file path is not defined  ( m_locations.empty() )
     * @pre exception if service does not support FILE mode
     */
    SIGHT_IO_API std::filesystem::path get_file() const;

    /**
     * @brief Sets file path
     * @pre exception if service does not support FILE mode
     */
    SIGHT_IO_API void set_file(const std::filesystem::path& _file);

    /**
     * @brief Returns file paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FILES mode
     */
    SIGHT_IO_API const io::service::locations_t& get_files() const;

    /**
     * @brief Sets file paths
     * @pre exception if service does not support FILES mode
     */
    SIGHT_IO_API void set_files(const io::service::locations_t& _files);

    /**
     * @brief Returns folder path set by the user or set during service configuration
     * @pre exception if a folder path is not defined ( m_locations.empty() )
     * @pre exception if service does not support FOLDER mode
     */
    SIGHT_IO_API std::filesystem::path get_folder() const;

    /**
     * @brief Clear any location set by the set_file/set_files/set_folder setter
     */
    SIGHT_IO_API void clear_locations();

    /**
     * @brief Returns file/files/folder paths set by the user or set during service configuration
     * @pre exception if a file path is not defined ( m_locations.empty() )
     */
    SIGHT_IO_API const io::service::locations_t& get_locations() const;

    /**
     * @brief Sets folder path
     * @pre exception if service does not support FOLDER mode
     */
    SIGHT_IO_API void set_folder(const std::filesystem::path& _folder);

    /**
     * @brief Slot: Inserts a path prefix generated via a signal sent to the service.
     *
     */
    SIGHT_IO_API void set_prefix(std::string _prefix);

    /**
     * @brief Slot: Sets the output base folder.
     *
     */
    SIGHT_IO_API virtual void set_base_folder(std::string _path);

    /// Returns if a location has been defined ( by the configuration process or directly by user )
    SIGHT_IO_API bool has_location_defined() const;

    /// Returns if reading has been cancelled by user
    SIGHT_IO_API bool has_failed() const;

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

    SIGHT_IO_API writer(const std::string& _default_window_title = s_DEFAULT_WINDOW_TITLE) noexcept;

    SIGHT_IO_API ~writer() noexcept override = default;

    /**
     * @brief This method proposes to parse xml configuration to retrieve file/files/folder paths.
     * @warning Using XML configuration is deprecated, use the properties instead.
     */
    SIGHT_IO_API void configuring() override;

    /// Defines the auto-connection between the file/folder properties and the 'set_file/set_folder'
    SIGHT_IO_API connections_t auto_connections() const override;

    /// Defines whether writing was performed correctly, or if user has cancelled the process.
    bool m_write_failed {false};

    /// Generic input data
    data::ptr<data::object, data::access::in> m_data {this, sight::io::service::DATA_KEY};

    /// Window title for the file dialog
    sight::data::property<sight::data::string> m_window_title {this, WINDOW_TITLE_KEY, s_DEFAULT_WINDOW_TITLE};

private:

    /**
     * @brief SLOT: Reads values from the file / folder properties and updates m_locations.
     */
    void update_default_locations();

    /**
     * @brief fill m_locations
     *
     * @param _files
     * @param _folder
     */
    void update_locations(
        const io::service::locations_t& _files,
        const std::filesystem::path& _folder
    );

    /// Triggers an update of the base folder, and outputs it via a reference.
    // We need to check for potential updates, notably in the case
    // where the user updates an associated preference during runtime.
    void update_base_folder(std::string& _out_base_folder) const;

    /// Default window title
    inline static const std::string s_DEFAULT_WINDOW_TITLE = "Choose a file";

    /// Value to store file or folder paths
    io::service::locations_t m_locations;

    /// Prefix to be inserted
    std::string m_current_prefix;

    /// Base folder
    std::string m_base_folder;

    sight::data::property<sight::data::string> m_files {this, FILES_KEY, std::string()};
    sight::data::property<sight::data::string> m_folder {this, FOLDER_KEY, std::string()};
};

} //namespace sight::io::service
