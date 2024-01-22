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

/**
 * @brief This file defines the interface of the base class for managing a layout geometry.
 */

#pragma once

#include "ui/__/config.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"
#include "ui/__/parameter.hpp"

namespace sight::ui::layout
{

/**
 * @brief   Defines the generic layout manager for IHM.
 */
class UI_CLASS_API view : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(view, ui::object);

    /// Constructor. Do nothing.
    UI_API view() = default;

    /// Destructor. Do nothing.
    UI_API ~view() override = default;

    /**
     * @brief Configure the layout before creation.
     */
    UI_API virtual void initialize(const ui::config_t& _configuration) = 0;

    /**
     * @brief Instantiate layout with parent container.
     * @pre layout must be initialized before.
     * @pre parent containers must be instanced.
     */
    UI_API virtual void create_layout(ui::container::widget::sptr _parent, const std::string& _id) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_API virtual void destroy_layout() = 0;

    /**
     * @brief Returns all sub containers managed by this layout.
     */
    UI_API virtual std::vector<ui::container::widget::sptr> get_sub_views();

    /// Modify a layout element, depending of the key.
    /// @param _parameter The parameter of the action.
    /// @param _key The action to perform.
    UI_API virtual void modify_layout(const ui::parameter_t& _parameter, const std::string& _key);

protected:

    /**
     * @brief Helper to destroy local sub views.
     */
    UI_API virtual void destroy_sub_views();

    /// All sub containers managed by this layout.
    std::vector<ui::container::widget::sptr> m_sub_views;
};

} // namespace sight::ui::layout
