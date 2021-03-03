/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/qt/layoutManager/MenuBarLayoutManager.hpp"

#include "ui/qt/container/QtMenuContainer.hpp"

#include <ui/base/registry/macros.hpp>

#include <QMenu>
#include <QMenuBar>
#include <QString>

fwGuiRegisterMacro( sight::ui::qt::layoutManager::MenuBarLayoutManager,
                    sight::ui::base::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY );

namespace sight::ui::qt
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

MenuBarLayoutManager::MenuBarLayoutManager(ui::base::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

MenuBarLayoutManager::~MenuBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::createLayout( ui::base::container::fwMenuBar::sptr parent )
{

    m_parent = ui::qt::container::QtMenuBarContainer::dynamicCast(parent);
    SIGHT_ASSERT("dynamicCast fwMenuBar to QtMenuBarContainer failed", m_parent);

    QMenuBar* menuBar = m_parent->getQtMenuBar();

    for ( std::string name : m_menuNames)
    {
        ui::qt::container::QtMenuContainer::sptr menu = ui::qt::container::QtMenuContainer::New();
        QMenu* qtMenu                                 = menuBar->addMenu(QString::fromStdString( name ));
        menu->setQtMenu(qtMenu);
        m_menus.push_back(menu);
    }
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::destroyLayout()
{
    this->destroyMenus();
    m_menus.clear();
    m_parent->clean();
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::menuIsVisible(ui::base::container::fwMenu::sptr fwMenu, bool isVisible)
{
    ui::qt::container::QtMenuContainer::sptr menuContainer = ui::qt::container::QtMenuContainer::dynamicCast(
        fwMenu);
    QMenu* menu = menuContainer->getQtMenu();
    menu->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::menuIsEnabled(ui::base::container::fwMenu::sptr fwMenu, bool isEnabled)
{
    ui::qt::container::QtMenuContainer::sptr menuContainer = ui::qt::container::QtMenuContainer::dynamicCast(
        fwMenu);
    QMenu* menu = menuContainer->getQtMenu();
    menu->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace sight::ui::qt
