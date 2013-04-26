/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

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
{}

//-----------------------------------------------------------------------------

MenuBarLayoutManager::~MenuBarLayoutManager()
{}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::createLayout( ::fwGui::container::fwMenuBar::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiQt::container::QtMenuBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenuBar to QtMenuBarContainer failed", m_parent);

    QMenuBar* menuBar = m_parent->getQtMenuBar();

    BOOST_FOREACH ( std::string name, m_menuNames)
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
    ::fwGuiQt::container::QtMenuContainer::sptr menuContainer = ::fwGuiQt::container::QtMenuContainer::dynamicCast(fwMenu);
    QMenu *menu = menuContainer->getQtMenu();
    menu->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void MenuBarLayoutManager::menuIsEnabled(::fwGui::container::fwMenu::sptr fwMenu, bool isEnabled)
{
    ::fwGuiQt::container::QtMenuContainer::sptr menuContainer = ::fwGuiQt::container::QtMenuContainer::dynamicCast(fwMenu);
    QMenu *menu = menuContainer->getQtMenu();
    menu->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



