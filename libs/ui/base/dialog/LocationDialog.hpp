/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/dialog/ILocationDialog.hpp"

#include <core/base.hpp>

namespace sight::ui::base
{

namespace dialog
{

/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 *
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries.
 */
class UI_BASE_CLASS_API LocationDialog : public ILocationDialog
{
public:

    SIGHT_DECLARE_CLASS(LocationDialog, ui::base::dialog::ILocationDialog, new LocationDialog)

    /// Will instantiate the concrete implementation
    UI_BASE_API LocationDialog();

    UI_BASE_API virtual ~LocationDialog();

    /**
     * @brief Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    UI_BASE_API core::location::ILocation::sptr show() override;

    /// allow to set option to the file dialog mode=READ/WRITE, check=FILE_MUST_EXIST
    UI_BASE_API ui::base::dialog::ILocationDialog& setOption(ui::base::dialog::ILocationDialog::Options option)
    override;

    /// Set the type of location for the dialog (SINGLE_FILE, FORLDER, MULTI_FILES)
    UI_BASE_API void setType(ui::base::dialog::ILocationDialog::Types type) override;

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] filterName a string that will be displayed as filter name
     * @param[in] wildcardList a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    UI_BASE_API void addFilter(const std::string& filterName, const std::string& wildcardList) override;

    /// Set the title for the dialog
    UI_BASE_API void setTitle(const std::string& title) override;

    /// Returns the title for the dialog
    UI_BASE_API const std::string& getTitle() override;

    /// Set the initial location for the dialog
    UI_BASE_API void setDefaultLocation(core::location::ILocation::sptr loc) override;

    /// Gets the default location for the dialog (from preferences or specified by user)
    UI_BASE_API const core::location::ILocation::sptr getDefaultLocation() override;

    /// Save the specified default location for the dialog in preferences (if available)
    UI_BASE_API void saveDefaultLocation(core::location::ILocation::sptr loc) override;

    /// Gets the current extension file selection
    UI_BASE_API std::string getCurrentSelection() const override;

protected:

    ui::base::dialog::ILocationDialog::sptr m_implementation;
};

} //namespace dialog

} // namespace sight::ui::base
