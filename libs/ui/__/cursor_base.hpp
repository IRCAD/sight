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

namespace sight::ui
{

/**
 * @brief   Defines the generic cursor for IHM.
 */
class SIGHT_UI_CLASS_API cursor_base : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(cursor_base, ui::object);

    /// cursor type
    enum state_t
    {
        DEFAULT,
        busy,
        wait,
        cross,
        open_hand,
        closed_hand
    };

    /// Destructor. Do nothing.
    SIGHT_UI_API ~cursor_base() override = default;

    using factory_registry_key_t = std::string;
    SIGHT_UI_API static const factory_registry_key_t REGISTRY_KEY;

    /// Set the cursor
    SIGHT_UI_API virtual void set_cursor(state_t _cursor, bool _set_overriden_as_default = true) = 0;

    /// Set the default cursor
    SIGHT_UI_API virtual void set_default_cursor() = 0;
};

} // namespace sight::ui
