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

#include "ui/__/config.hpp"
#include "ui/__/container/toolbar.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::builder
{

/**
 * @brief   Defines the interface class  for the toolbar builder.
 */
class UI_CLASS_API toolbar : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(toolbar, ui::object);

    using registry_key_t = std::string;

    UI_API const static registry_key_t REGISTRY_KEY;

    enum alignment
    {
        top,
        bottom,
        right,
        left
    };

    /// Constructor. Do nothing.
    UI_API toolbar();

    /// Destructor. Do nothing.
    UI_API ~toolbar() override = default;

    /**
     * @brief Returns the builded tool bar.
     */
    UI_API virtual ui::container::toolbar::sptr get_tool_bar();

    /**
     * @brief Initialize the tool bar.
     *
     * Example of configuration
     * @code{.xml}
        <toolbar align="top" backgroundColor="default" spacing="10">
            <toolBitmapSize height= "50" width="50" />
        </toolbar>
       @endcode
        - \b align : toolbar alignment (top, bottom, left, right).
        - \b backgroundColor (optional) : (hexa) background color.
        - \b spacing (optional) : (int) spacing between each button, in pixels.
     *  - \<toolBitmapSize height= "50" width="50"/\> : give the size of the icon.
     */

    UI_API virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief Instantiate layout with parent toolbar.
     * @pre layout must be initialized before.
     * @pre parent toolbar must be instanced.
     */
    UI_API virtual void create_tool_bar(ui::container::widget::sptr _parent) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_API virtual void destroy_tool_bar() = 0;

protected:

    /// tool_bar.
    ui::container::toolbar::sptr m_tool_bar;

    std::pair<int, int> m_tool_bitmap_size;

    int m_spacing {0};

    alignment m_alignment {top};

    /// Background color. Use an empty string to use the default background color, else, set an hexadecimal value.
    std::string m_background_color;
};

} // namespace sight::ui::builder
