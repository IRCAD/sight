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
 * @brief   Defines the menu item for IHM.
 */
class SIGHT_UI_CLASS_API menu_item : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(menu_item, ui::object);

    SIGHT_UI_API virtual void clean()             = 0;
    SIGHT_UI_API virtual void destroy_container() = 0;
};

} // namespace sight::ui::container
