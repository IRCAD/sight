/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQml/layoutManager/MenuLayoutManager.hpp"

#include "fwGuiQml/ActionCallback.hpp"
#include "fwGuiQml/container/QmlMenuContainer.hpp"
#include "fwGuiQml/container/QmlMenuItemContainer.hpp"

#include <fwGui/registry/macros.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/lambda/lambda.hpp>

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

    m_parent = ::fwGuiQml::container::QmlMenuContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenu to QmlMenuContainer failed", m_parent);

//    QMenu* menu = m_parent->getQmlMenu();

//    QActionGroup* actionGroup  = 0;
    unsigned int menuItemIndex = 0;
    for ( ::fwGui::layoutManager::IMenuLayoutManager::ActionInfo actionInfo : m_actionInfo)
    {
        ::fwGuiQml::container::QmlMenuItemContainer::sptr menuItem = ::fwGuiQml::container::QmlMenuItemContainer::New();

//        QAction* action = menu->addAction( QString::fromStdString(actionInfo.m_name) );

//        action->setSeparator(actionInfo.m_isSeparator);

//        if (!actionInfo.m_icon.empty())
//        {
//            QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
//            action->setIcon(icon);
//        }
//        if (actionInfo.m_type == ::fwGui::layoutManager::IMenuLayoutManager::QUIT)
//        {
//            action->setMenuRole(QAction::QuitRole);
//        }
//        else if (actionInfo.m_type == ::fwGui::layoutManager::IMenuLayoutManager::ABOUT)
//        {
//            action->setMenuRole(QAction::AboutRole);
//        }
//        else
//        {
//            action->setMenuRole(QAction::NoRole);
//        }

//        action->setCheckable(actionInfo.m_isCheckable || actionInfo.m_isRadio);

//        if (actionInfo.m_isRadio)
//        {
//            if (!actionGroup)
//            {
//                actionGroup = new QActionGroup(menu);
//            }
//            actionGroup->addAction(action);
//        }

        // create shortcut
//        if( !actionInfo.m_shortcut.empty() )
//        {
//            action->setShortcut(QKeySequence(QString::fromStdString(actionInfo.m_shortcut)));
//        }

//        if (actionInfo.m_isMenu)
//        {
//            ::fwGuiQml::container::QmlMenuContainer::sptr menu = ::fwGuiQml::container::QmlMenuContainer::New();
//            QMenu* qmlMenu = new QMenu();
//            menu->setQmlMenu(qmlMenu);
//            action->setMenu(qmlMenu);
//            m_menus.push_back(menu);
//        }

//        menuItem->setQmlMenuItem(action);

//        if(!actionInfo.m_isSeparator && !actionInfo.m_isMenu )
//        {
//            m_menuItems.push_back(menuItem);
//            OSLM_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
//            ::fwGui::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

//            ::fwGuiQml::ActionCallback::sptr qmlCallback = ::fwGuiQml::ActionCallback::dynamicCast(callback);
//            SLM_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", qmlCallback);

//            QObject::connect( action, SIGNAL(triggered(bool)), qmlCallback.get(), SLOT(executeQt(bool)));
//            QObject::connect( action, SIGNAL(toggled(bool)), qmlCallback.get(), SLOT(checkQt(bool)));
//            menuItemIndex++;
//        }
//        else
//        {
//            actionGroup = 0;
//        }
    }
}

//-----------------------------------------------------------------------------

//void MenuLayoutManager::destroyLayout()
//{
//    this->destroyActions();
//    m_menuItems.clear();
//    m_parent->clean();
//}

//-----------------------------------------------------------------------------

//void MenuLayoutManager::menuItemSetVisible(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isVisible)
//{
//    ::fwGuiQml::container::QmlMenuItemContainer::sptr menuItemContainer =
//        ::fwGuiQml::container::QmlMenuItemContainer::dynamicCast(fwMenuItem);
//    QAction* action = menuItemContainer->getQmlMenuItem();
//    action->setVisible(isVisible);
//}

//-----------------------------------------------------------------------------

//void MenuLayoutManager::menuItemSetEnabled(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isEnabled)
//{
//    ::fwGuiQml::container::QmlMenuItemContainer::sptr menuItemContainer =
//        ::fwGuiQml::container::QmlMenuItemContainer::dynamicCast(fwMenuItem);
//    QAction* action = menuItemContainer->getQmlMenuItem();
//    action->setEnabled(isEnabled);
//}

//-----------------------------------------------------------------------------

//void MenuLayoutManager::menuItemSetChecked(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isChecked)
//{
//    ::fwGuiQml::container::QmlMenuItemContainer::sptr menuItemContainer =
//        ::fwGuiQml::container::QmlMenuItemContainer::dynamicCast(fwMenuItem);
//    QAction* action = menuItemContainer->getQmlMenuItem();
//    action->setChecked(isChecked);
//}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui
