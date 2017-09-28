/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/layoutManager/MenuLayoutManager.hpp"

#include "fwGuiQt/ActionCallback.hpp"
#include "fwGuiQt/container/QtMenuContainer.hpp"
#include "fwGuiQt/container/QtMenuItemContainer.hpp"

#include <fwGui/registry/macros.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/lambda/lambda.hpp>

#include <QAction>
#include <QActionGroup>
#include <QMenu>

#include <functional>

fwGuiRegisterMacro( ::fwGui::layoutManager::MenuLayoutManager,
                    ::fwGui::layoutManager::IMenuLayoutManager::REGISTRY_KEY );

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

MenuLayoutManager::MenuLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

MenuLayoutManager::~MenuLayoutManager()
{
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::createLayout( ::fwGui::container::fwMenu::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiQt::container::QtMenuContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenu to QtMenuContainer failed", m_parent);

    QMenu* menu = m_parent->getQtMenu();

    QActionGroup* actionGroup  = 0;
    unsigned int menuItemIndex = 0;
    for ( ::fwGui::layoutManager::IMenuLayoutManager::ActionInfo actionInfo : m_actionInfo)
    {
        ::fwGuiQt::container::QtMenuItemContainer::sptr menuItem = ::fwGuiQt::container::QtMenuItemContainer::New();

        QAction* action = menu->addAction( QString::fromStdString(actionInfo.m_name) );

        action->setSeparator(actionInfo.m_isSeparator);

        if (!actionInfo.m_icon.empty())
        {
            QIcon icon(QString::fromStdString(actionInfo.m_icon));
            action->setIcon(icon);
        }
        if (actionInfo.m_type == ::fwGui::layoutManager::IMenuLayoutManager::QUIT)
        {
            action->setMenuRole(QAction::QuitRole);
        }
        else if (actionInfo.m_type == ::fwGui::layoutManager::IMenuLayoutManager::ABOUT)
        {
            action->setMenuRole(QAction::AboutRole);
        }
        else
        {
            action->setMenuRole(QAction::NoRole);
        }

        action->setCheckable(actionInfo.m_isCheckable || actionInfo.m_isRadio);

        if (actionInfo.m_isRadio)
        {
            if (!actionGroup)
            {
                actionGroup = new QActionGroup(menu);
            }
            actionGroup->addAction(action);
        }

        // create shortcut
        if( !actionInfo.m_shortcut.empty() )
        {
            action->setShortcut(QKeySequence(QString::fromStdString(actionInfo.m_shortcut)));
        }

        if (actionInfo.m_isMenu)
        {
            ::fwGuiQt::container::QtMenuContainer::sptr menu = ::fwGuiQt::container::QtMenuContainer::New();
            QMenu* qtMenu = new QMenu();
            menu->setQtMenu(qtMenu);
            action->setMenu(qtMenu);
            m_menus.push_back(menu);
        }

        menuItem->setQtMenuItem(action);

        if(!actionInfo.m_isSeparator && !actionInfo.m_isMenu )
        {
            m_menuItems.push_back(menuItem);
            OSLM_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
            ::fwGui::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

            ::fwGuiQt::ActionCallback::sptr qtCallback = ::fwGuiQt::ActionCallback::dynamicCast(callback);
            SLM_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", qtCallback);

            QObject::connect( action, SIGNAL(triggered(bool)), qtCallback.get(), SLOT(executeQt(bool)));
            QObject::connect( action, SIGNAL(toggled(bool)), qtCallback.get(), SLOT(checkQt(bool)));
            menuItemIndex++;
        }
        else
        {
            actionGroup = 0;
        }
    }
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::destroyLayout()
{
    this->destroyActions();
    m_menuItems.clear();
    m_parent->clean();
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::menuItemSetVisible(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isVisible)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer =
        ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::menuItemSetEnabled(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer =
        ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::menuItemSetChecked(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isChecked)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer =
        ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui

