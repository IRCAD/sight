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

#include "ui/__/dialog/location_base.hpp"

#include <core/base.hpp>

namespace sight::ui::dialog
{

/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 *
 * Use the Delegate design pattern. The specific implementation selection is ensured by SIGHT_REGISTER_GUI.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries.
 */
class SIGHT_UI_CLASS_API location : public location_base
{
public:

    SIGHT_DECLARE_CLASS(location, ui::dialog::location_base);

    /// Will instantiate the concrete implementation
    SIGHT_UI_API location();

    SIGHT_UI_API ~location() override;

    /**
     * @brief Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    SIGHT_UI_API core::location::base::sptr show() override;

    /// allow to set option to the file dialog mode=READ/WRITE, check=FILE_MUST_EXIST
    SIGHT_UI_API void set_option(location::options _option)
    override;

    /// Set the type of location for the dialog (SINGLE_FILE, FOLDER, MULTI_FILES)
    SIGHT_UI_API void set_type(location::types _type) override;

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] _filter_name a string that will be displayed as filter name
     * @param[in] _wildcard_list a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    SIGHT_UI_API void add_filter(const std::string& _filter_name, const std::string& _wildcard_list) override;

    /// Set the title for the dialog
    SIGHT_UI_API void set_title(const std::string& _title) override;

    /// Returns the title for the dialog
    SIGHT_UI_API const std::string& get_title() override;

    /// Set the initial location for the dialog
    SIGHT_UI_API void set_default_location(core::location::base::sptr _loc) override;

    /// Gets the default location for the dialog (from preferences or specified by user)
    SIGHT_UI_API core::location::base::sptr get_default_location() override;

    /// Save the specified default location for the dialog in preferences (if available)
    SIGHT_UI_API void save_default_location(core::location::base::sptr _loc) override;

    /// Gets the current extension file selection
    SIGHT_UI_API std::string get_current_selection() const override;

protected:

    ui::dialog::location_base::sptr m_implementation;
};

} // namespace sight::ui::dialog
