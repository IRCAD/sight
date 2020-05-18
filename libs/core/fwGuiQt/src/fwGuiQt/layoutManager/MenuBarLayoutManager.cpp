/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <QMenu>
#include <QMenuBar>
#include <QString>

#include <fwGui/registry/macros.hpp>

#include "fwGuiQt/container/QtMenuContainer.hpp"
#include "fwGuiQt/layoutManager/MenuBarLayoutManager.hpp"


fwGuiRegisterMacro( ::fwGui::layoutManager::MenuBarLayoutManager,
                    ::fwGui::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY );

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

MenuBarLayoutManager::MenuBarLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

MenuBarLayoutManager::~MenuBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::createLayout( ::fwGui::container::fwMenuBar::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiQt::container::QtMenuBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenuBar to QtMenuBarContainer failed", m_parent);

    QMenuBar* menuBar = m_parent->getQtMenuBar();

    for ( std::string name : m_menuNames)
    {
        ::fwGuiQt::container::QtMenuContainer::sptr menu = ::fwGuiQt::container::QtMenuContainer::New();
        QMenu *qtMenu = menuBar->addMenu(QString::fromStdString( name ));
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


void MenuBarLayoutManager::menuIsVisible(::fwGui::container::fwMenu::sptr fwMenu, bool isVisible)
{
    ::fwGuiQt::container::QtMenuContainer::sptr menuContainer = ::fwGuiQt::container::QtMenuContainer::dynamicCast(
        fwMenu);
    QMenu *menu = menuContainer->getQtMenu();
    menu->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::menuIsEnabled(::fwGui::container::fwMenu::sptr fwMenu, bool isEnabled)
{
    ::fwGuiQt::container::QtMenuContainer::sptr menuContainer = ::fwGuiQt::container::QtMenuContainer::dynamicCast(
        fwMenu);
    QMenu *menu = menuContainer->getQtMenu();
    menu->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



