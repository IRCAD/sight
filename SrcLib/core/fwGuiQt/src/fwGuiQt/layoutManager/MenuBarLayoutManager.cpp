/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <QMenu>
#include <QMenuBar>
#include <QString>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/container/QtMenuContainer.hpp"
#include "fwGuiQt/layoutManager/MenuBarLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IMenuBarLayoutManager,
        ::fwGui::layoutManager::MenuBarLayoutManager,
         ::fwGui::layoutManager::IMenuBarLayoutManager::RegistryKeyType,
          ::fwGui::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY );

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

MenuBarLayoutManager::MenuBarLayoutManager()
{}

//-----------------------------------------------------------------------------

MenuBarLayoutManager::~MenuBarLayoutManager()
{}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::createLayout( ::fwGui::fwMenuBar::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiQt::container::QtMenuBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenuBar to QtMenuBarContainer failed", m_parent);

    QMenuBar* menuBar = m_parent->getQtMenuBar();

    BOOST_FOREACH ( std::string name, m_menuNames)
    {
        ::fwGuiQt::container::QtMenuContainer::NewSptr menu;
        QMenu *qtMenu = menuBar->addMenu(QString::fromStdString( name ));
        menu->setQtMenu(qtMenu);
        m_menus.push_back(menu);
    }
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::destroyLayout()
{
    QMenuBar* menuBar = m_parent->getQtMenuBar();
    menuBar->clear();
    m_menus.clear();
}

//-----------------------------------------------------------------------------


void MenuBarLayoutManager::menuIsVisible(::fwGui::fwMenu::sptr fwMenu, bool isVisible)
{
    ::fwGuiQt::container::QtMenuContainer::sptr menuContainer = ::fwGuiQt::container::QtMenuContainer::dynamicCast(fwMenu);
    QMenu *menu = menuContainer->getQtMenu();
    menu->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::menuIsEnabled(::fwGui::fwMenu::sptr fwMenu, bool isEnabled)
{
    ::fwGuiQt::container::QtMenuContainer::sptr menuContainer = ::fwGuiQt::container::QtMenuContainer::dynamicCast(fwMenu);
    QMenu *menu = menuContainer->getQtMenu();
    menu->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



