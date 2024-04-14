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

#include <sight/ui/qt/config.hpp>

#include <core/base.hpp>

#include <ui/__/dialog/location_base.hpp>

class QString;

/**
 * @brief       The namespace sight::ui::qt contains classes which provide the implementation of the Gui using Qt
 * library.
 */
namespace sight::ui::qt::dialog
{

/**
 * @brief   Defines the generic file/folder dialog.
 *
 */
class SIGHT_UI_QT_CLASS_API location : public ui::dialog::location_base
{
public:

    SIGHT_DECLARE_CLASS(location, ui::dialog::location_base, ui::factory::make<location>);

    SIGHT_UI_QT_API core::location::base::sptr show() override;

    SIGHT_UI_QT_API void set_type(location::types _type) override;

    SIGHT_UI_QT_API void set_option(location::options _option) override;

    // Example ( addFilter("images","*.png *.jpg");
    SIGHT_UI_QT_API void add_filter(const std::string& _filter_name, const std::string& _wildcard_list) override;

protected:

    location::options m_style {location::none};
    location::types m_type {location::single_file};
    std::vector<std::pair<std::string, std::string> > m_filters;

    /// helper to transform m_filters into qt encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    QString file_filters();

    /// Gets the current extension file selection
    SIGHT_UI_QT_API std::string get_current_selection() const override;

    std::string m_wildcard;
};

} // namespace sight::ui::qt::dialog
