/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/config.hpp"
#include "ui/__/object.hpp"

namespace sight::ui::container
{

/**
 * @brief   Defines the generic container for IHM.
 *
 */
class UI_CLASS_API widget : public ui::object
{
public:

    SIGHT_DECLARE_CLASS(widget, ui::object);

    UI_API virtual void clean()            = 0;
    UI_API virtual void destroyContainer() = 0;
    UI_API virtual bool isShownOnScreen()  = 0;

    UI_API virtual void setVisible(bool _is_visible) = 0;
    UI_API virtual void setEnabled(bool _is_enabled) = 0;
};

} // namespace sight::ui::container
