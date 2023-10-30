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

#include "ui/qt/layout/menubar.hpp"

#include "ui/qt/container/menu.hpp"

#include <ui/__/macros.hpp>

#include <QMenu>
#include <QMenuBar>
#include <QString>

SIGHT_REGISTER_GUI(sight::ui::qt::layout::menubar, sight::ui::layout::menubar_manager::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void menubar::create_layout(ui::container::menubar::sptr _parent, const std::string& _id)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::menubar>(_parent);
    SIGHT_ASSERT("dynamicCast menubar to menubar failed", m_parent);

    const QString q_id = QString::fromStdString(_id);

    QMenuBar* menu_bar = m_parent->get_qt_menu_bar();
    menu_bar->setObjectName(q_id);

    for(const std::string& name : m_menu_names)
    {
        ui::qt::container::menu::sptr menu = ui::qt::container::menu::make();
        QMenu* qt_menu                     = menu_bar->addMenu(QString::fromStdString(name));
        qt_menu->setObjectName(q_id + '/' + name.c_str());
        menu->set_qt_menu(qt_menu);
        m_menus.push_back(menu);
    }
}

//-----------------------------------------------------------------------------

void menubar::destroy_layout()
{
    this->destroy_menus();
    m_menus.clear();
    m_parent->clean();
}

//-----------------------------------------------------------------------------

void menubar::menu_is_visible(ui::container::menu::sptr _menu, bool _is_visible)
{
    auto menu_container = std::dynamic_pointer_cast<ui::qt::container::menu>(_menu);
    QMenu* menu         = menu_container->get_qt_menu();
    menu->setVisible(_is_visible);
}

//-----------------------------------------------------------------------------

void menubar::menu_is_enabled(ui::container::menu::sptr _menu, bool _is_enabled)
{
    auto menu_container = std::dynamic_pointer_cast<ui::qt::container::menu>(_menu);
    QMenu* menu         = menu_container->get_qt_menu();
    menu->setEnabled(_is_enabled);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
