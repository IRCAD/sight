/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <sight/ui/__/config.hpp>

#include "ui/__/object.hpp"

#include <core/location/base.hpp>

#include <filesystem>
#include <string>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 */
class SIGHT_UI_CLASS_API location_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(location_base, ui::object);

    enum options
    {
        none            = 0,
        read            = 1 << 1,
        write           = 1 << 2,
        file_must_exist = 1 << 3
    };

    enum types
    {
        single_file,
        folder,
        multi_files
    };

    using factory_registry_key_t = std::string;

    /// this *unique* key should  be used *for all* factory for specific location(qt,wx,...)
    SIGHT_UI_API static const factory_registry_key_t REGISTRY_KEY;

    SIGHT_UI_API static const std::string SOFTWARE_UI;
    SIGHT_UI_API static const std::string DLG_DEFAULT_FILE;
    SIGHT_UI_API static const std::string DLG_DEFAULT_DIRECTORY;

    SIGHT_UI_API ~location_base() override;

    SIGHT_UI_API location_base();

    /// set the title for the dialog
    SIGHT_UI_API virtual void set_title(const std::string& _title);

    /// returns the title for the dialog
    SIGHT_UI_API virtual const std::string& get_title();

    /// set the initial location for the dialog
    SIGHT_UI_API virtual void set_default_location(core::location::base::sptr _loc);

    /// get the default location for the dialog (from preferences or specified by user)
    SIGHT_UI_API virtual core::location::base::sptr get_default_location();

    /// save the specified default location for the dialog in preferences (if available)
    SIGHT_UI_API virtual void save_default_location(core::location::base::sptr _loc);

    /// set the type of location for the dialog (SINGLE_FILE, FOLDER, MULTI_FILES)
    SIGHT_UI_API virtual void set_type(types _type) = 0;

    /// allow to set option to the file dialog mode=READ/WRITE , check=FILE_MUST_EXIST
    SIGHT_UI_API virtual void set_option(options _option) = 0;

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] _filter_name a string that will be displayed as filter name
     * @param[in] _wildcard_list a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    SIGHT_UI_API virtual void add_filter(const std::string& _filter_name, const std::string& _wildcard_list) = 0;

    /**
     * Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    SIGHT_UI_API virtual core::location::base::sptr show() = 0;

    /// Gets the current extension file selection
    SIGHT_UI_API virtual std::string get_current_selection() const = 0;

    /// Helpers that splits the extension getCurrentSelection()
    /// @return a vector of selected extensions
    SIGHT_UI_API std::vector<std::string> get_selected_extensions() const;

protected:

    SIGHT_UI_API void read_config();

    SIGHT_UI_API void write_config();

private:

    std::string m_title;
    core::location::base::sptr m_default_locaction;
};

} // namespace sight::ui::dialog
