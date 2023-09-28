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

void menubar::createLayout(ui::container::menubar::sptr parent, const std::string& id)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::menubar>(parent);
    SIGHT_ASSERT("dynamicCast menubar to menubar failed", m_parent);

    const QString qId = QString::fromStdString(id);

    QMenuBar* menuBar = m_parent->getQtMenuBar();
    menuBar->setObjectName(qId);

    for(const std::string& name : m_menuNames)
    {
        ui::qt::container::menu::sptr menu = ui::qt::container::menu::make();
        QMenu* qtMenu                      = menuBar->addMenu(QString::fromStdString(name));
        qtMenu->setObjectName(qId + '/' + name.c_str());
        menu->setQtMenu(qtMenu);
        m_menus.push_back(menu);
    }
}

//-----------------------------------------------------------------------------

void menubar::destroyLayout()
{
    this->destroyMenus();
    m_menus.clear();
    m_parent->clean();
}

//-----------------------------------------------------------------------------

void menubar::menuIsVisible(ui::container::menu::sptr _menu, bool isVisible)
{
    auto menuContainer = std::dynamic_pointer_cast<ui::qt::container::menu>(_menu);
    QMenu* menu        = menuContainer->getQtMenu();
    menu->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void menubar::menuIsEnabled(ui::container::menu::sptr _menu, bool isEnabled)
{
    auto menuContainer = std::dynamic_pointer_cast<ui::qt::container::menu>(_menu);
    QMenu* menu        = menuContainer->getQtMenu();
    menu->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
