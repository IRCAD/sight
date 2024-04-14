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

#include "ui/qt/container/toolbar.hpp"

#include <QAction>
#include <QMenu>
#include <QToolBar>

namespace sight::ui::qt::container
{

//-----------------------------------------------------------------------------

void toolbar::clean()
{
    SIGHT_ASSERT("The Qt toolBar is not yet initialized, cleaning is thus impossible", m_tool_bar);

    m_tool_bar->clear();
}

//-----------------------------------------------------------------------------

void toolbar::destroy_container()
{
    SIGHT_ASSERT("The Qt toolBar is not yet initialized, cleaning is thus impossible", m_tool_bar);

    delete m_tool_bar;
}

//-----------------------------------------------------------------------------

void toolbar::set_qt_tool_bar(QToolBar* _tool_bar)
{
    this->m_tool_bar = _tool_bar;
}

//-----------------------------------------------------------------------------

QToolBar* toolbar::get_qt_tool_bar()
{
    return this->m_tool_bar;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::container
