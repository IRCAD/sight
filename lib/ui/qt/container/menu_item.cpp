/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/qt/container/menu_item.hpp"

#include <QAction>

namespace sight::ui::qt::container
{

//-----------------------------------------------------------------------------

void menu_item::clean()
{
    SIGHT_ASSERT("The qt menu_item is not yet initialized, cleaning is thus impossible", m_menu_item);
}

//-----------------------------------------------------------------------------

void menu_item::destroy_container()
{
    SIGHT_ASSERT("The qt menu_item is not yet initialized, cleaning is thus impossible", m_menu_item);
}

//-----------------------------------------------------------------------------

void menu_item::set_qt_menu_item(QAction* _menu_item)
{
    this->m_menu_item = _menu_item;
}

//-----------------------------------------------------------------------------

QAction* menu_item::get_qt_menu_item()
{
    return this->m_menu_item;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::container
