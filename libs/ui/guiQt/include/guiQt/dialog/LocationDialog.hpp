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

#include "guiQt/config.hpp"

#include <core/base.hpp>

#include <gui/dialog/ILocationDialog.hpp>

class QString;

/**
 * @brief       The namespace sight::guiQt contains classes which provide the implementation of the Gui using Qt
 * library.
 */
namespace sight::guiQt
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder dialog for IHM.
 *
 */
class GUIQT_CLASS_API LocationDialog : public gui::dialog::ILocationDialog
{

public:

    fwCoreClassMacro(LocationDialog, gui::dialog::ILocationDialog, gui::factory::New< LocationDialog >)

    GUIQT_API LocationDialog(gui::GuiBaseObject::Key key);

    GUIQT_API data::location::ILocation::sptr show() override;

    GUIQT_API void setType( gui::dialog::ILocationDialog::Types type ) override;

    GUIQT_API gui::dialog::ILocationDialog& setOption( gui::dialog::ILocationDialog::Options option) override;

    // Example ( addFilter("images","*.png *.jpg");
    GUIQT_API void addFilter(const std::string& filterName, const std::string& wildcardList ) override;

protected:

    gui::dialog::ILocationDialog::Options m_style;
    gui::dialog::ILocationDialog::Types m_type;
    std::vector< std::pair< std::string, std::string > > m_filters;

    /// helper to transform m_filters into qt encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    QString fileFilters();
    /// Gets the current extension file selection
    GUIQT_API std::string getCurrentSelection() const override;

    std::string m_wildcard;

};
} // namespace dialog
} // namespace sight::guiQt
