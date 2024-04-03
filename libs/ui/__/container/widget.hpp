/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

namespace sight::ui::container
{

/**
 * @brief   Defines the generic container for IHM.
 *
 */
class SIGHT_UI_CLASS_API widget : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(widget, ui::object);

    SIGHT_UI_API virtual void clean()              = 0;
    SIGHT_UI_API virtual void destroy_container()  = 0;
    SIGHT_UI_API virtual bool is_shown_on_screen() = 0;

    SIGHT_UI_API virtual void set_visible(bool _is_visible) = 0;
    SIGHT_UI_API virtual void set_enabled(bool _is_enabled) = 0;
};

} // namespace sight::ui::container
