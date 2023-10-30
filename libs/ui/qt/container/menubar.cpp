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

#include "ui/qt/container/menubar.hpp"

#include <QMenuBar>

namespace sight::ui::qt::container
{

//-----------------------------------------------------------------------------

void menubar::clean()
{
    SIGHT_ASSERT("The Qt menu_bar is not yet initialized, cleaning is thus impossible", m_menu_bar);

    m_menu_bar->clear();

    QList<QMenu*> list = m_menu_bar->findChildren<QMenu*>();
    Q_FOREACH(QMenu* m, list)
    {
        m->hide();
        delete m;
    }
}

//-----------------------------------------------------------------------------

void menubar::destroy_container()
{
    SIGHT_ASSERT("The Qt menu_bar is not yet initialized", m_menu_bar);

    QList<QMenu*> all_menu = m_menu_bar->findChildren<QMenu*>();
    SIGHT_ASSERT("menu_bar container must be empty ( " << all_menu.count() << " menus).", all_menu.empty());

    m_menu_bar->hide();
    delete m_menu_bar;
}

//-----------------------------------------------------------------------------

void menubar::set_qt_menu_bar(QMenuBar* _menu_bar)
{
    this->m_menu_bar = _menu_bar;
}

//-----------------------------------------------------------------------------

QMenuBar* menubar::get_qt_menu_bar()
{
    return this->m_menu_bar;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::container
