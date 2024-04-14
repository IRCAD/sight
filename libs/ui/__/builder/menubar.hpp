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

#include "ui/__/container/menubar.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::builder
{

/**
 * @brief   Defines the interface class  for the menubar builder.
 */
class SIGHT_UI_CLASS_API menubar : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(menubar, ui::object);

    using registry_key_t = std::string;

    SIGHT_UI_API const static registry_key_t REGISTRY_KEY;

    /// Constructor. Do nothing.
    SIGHT_UI_API menubar() = default;

    /// Destructor. Do nothing.
    SIGHT_UI_API ~menubar() override = default;

    /**
     * @brief Returns the builded menu bar.
     */
    SIGHT_UI_API virtual ui::container::menubar::sptr get_menu_bar();

    /**
     * @brief Configure the layout before creation.
     */
    SIGHT_UI_API virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief Instantiate layout with parent menubar.
     * @pre layout must be initialized before.
     * @pre parent menubar must be instanced.
     */
    SIGHT_UI_API virtual void create_menu_bar(ui::container::widget::sptr _parent) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    SIGHT_UI_API virtual void destroy_menu_bar() = 0;

protected:

    /// menu_bar.
    ui::container::menubar::sptr m_menu_bar;

    /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
    std::string m_background_color;

    /// Defines a qss key to apply style
    std::string m_qss_class;
};

} // namespace sight::ui::builder
