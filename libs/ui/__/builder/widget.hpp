/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::builder
{

/**
 * @brief   Defines the interface class  for the container builder.
 */
class UI_CLASS_API widget : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(widget, ui::object);

    using registry_key_t = std::string;

    UI_API const static registry_key_t REGISTRY_KEY;

    /// Constructor. Do nothing.
    UI_API widget();

    /// Destructor. Do nothing.
    UI_API ~widget() override;

    /**
     * @brief Returns the builded menu bar.
     */
    UI_API virtual ui::container::widget::sptr get_container();

    /**
     * @brief Configure the layout before creation. Does nothing.
     */
    UI_API virtual void initialize(const ui::config_t& _configuration);

    /**
     * @brief Instantiate layout with parent container.
     * @pre layout must be initialized before.
     * @pre parent container must be instanced.
     */
    UI_API virtual void create_container(ui::container::widget::sptr _parent) = 0;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    UI_API virtual void destroy_container() = 0;

    /// Replace parent container
    UI_API virtual void set_parent(ui::container::widget::sptr _parent) = 0;

protected:

    /// Container.
    ui::container::widget::sptr m_container;
};

} // namespace sight::ui::builder
