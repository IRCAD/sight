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

#include "ui/qt/config.hpp"

#include <core/base.hpp>

#include <ui/base/dialog/ILocationDialog.hpp>

class QString;

/**
 * @brief       The namespace sight::ui::qt contains classes which provide the implementation of the Gui using Qt
 * library.
 */
namespace sight::ui::qt
{

namespace dialog
{

/**
 * @brief   Defines the generic file/folder dialog.
 *
 */
class UI_QT_CLASS_API LocationDialog : public ui::base::dialog::ILocationDialog
{
public:

    SIGHT_DECLARE_CLASS(LocationDialog, ui::base::dialog::ILocationDialog, ui::base::factory::New<LocationDialog>);

    UI_QT_API LocationDialog(ui::base::GuiBaseObject::Key key);

    UI_QT_API core::location::ILocation::sptr show() override;

    UI_QT_API void setType(ui::base::dialog::ILocationDialog::Types type) override;

    UI_QT_API ui::base::dialog::ILocationDialog& setOption(ui::base::dialog::ILocationDialog::Options option) override;

    // Example ( addFilter("images","*.png *.jpg");
    UI_QT_API void addFilter(const std::string& filterName, const std::string& wildcardList) override;

protected:

    ui::base::dialog::ILocationDialog::Options m_style;
    ui::base::dialog::ILocationDialog::Types m_type;
    std::vector<std::pair<std::string, std::string> > m_filters;

    /// helper to transform m_filters into qt encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    QString fileFilters();

    /// Gets the current extension file selection
    UI_QT_API std::string getCurrentSelection() const override;

    std::string m_wildcard;
};

} // namespace dialog

} // namespace sight::ui::qt
