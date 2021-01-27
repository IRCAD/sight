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

#include "guiQt/layoutManager/MenuBarLayoutManager.hpp"

#include "guiQt/container/QtMenuContainer.hpp"

#include <gui/registry/macros.hpp>

#include <QMenu>
#include <QMenuBar>
#include <QString>

fwGuiRegisterMacro( ::sight::gui::layoutManager::MenuBarLayoutManager,
                    ::sight::gui::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY );

namespace sight::gui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

MenuBarLayoutManager::MenuBarLayoutManager(gui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

MenuBarLayoutManager::~MenuBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::createLayout( gui::container::fwMenuBar::sptr parent )
{

    m_parent = guiQt::container::QtMenuBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenuBar to QtMenuBarContainer failed", m_parent);

    QMenuBar* menuBar = m_parent->getQtMenuBar();

    for ( std::string name : m_menuNames)
    {
        guiQt::container::QtMenuContainer::sptr menu = guiQt::container::QtMenuContainer::New();
        QMenu* qtMenu                                = menuBar->addMenu(QString::fromStdString( name ));
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

void MenuBarLayoutManager::menuIsVisible(gui::container::fwMenu::sptr fwMenu, bool isVisible)
{
    guiQt::container::QtMenuContainer::sptr menuContainer = guiQt::container::QtMenuContainer::dynamicCast(
        fwMenu);
    QMenu* menu = menuContainer->getQtMenu();
    menu->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::menuIsEnabled(gui::container::fwMenu::sptr fwMenu, bool isEnabled)
{
    guiQt::container::QtMenuContainer::sptr menuContainer = guiQt::container::QtMenuContainer::dynamicCast(
        fwMenu);
    QMenu* menu = menuContainer->getQtMenu();
    menu->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace sight::gui
