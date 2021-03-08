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

#include "ui/qt/layoutManager/MenuLayoutManager.hpp"

#include "ui/qt/ActionCallback.hpp"
#include "ui/qt/container/QtMenuContainer.hpp"
#include "ui/qt/container/QtMenuItemContainer.hpp"

#include <ui/base/registry/macros.hpp>

#include <QAction>
#include <QActionGroup>
#include <QMenu>

#include <functional>

fwGuiRegisterMacro( sight::ui::qt::layoutManager::MenuLayoutManager,
                    sight::ui::base::layoutManager::IMenuLayoutManager::REGISTRY_KEY );

namespace sight::ui::qt
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

MenuLayoutManager::MenuLayoutManager(ui::base::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

MenuLayoutManager::~MenuLayoutManager()
{
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::createLayout( ui::base::container::fwMenu::sptr parent )
{

    m_parent = ui::qt::container::QtMenuContainer::dynamicCast(parent);
    SIGHT_ASSERT("dynamicCast fwMenu to QtMenuContainer failed", m_parent);

    QMenu* menu = m_parent->getQtMenu();

    QActionGroup* actionGroup  = 0;
    unsigned int menuItemIndex = 0;
    for ( ui::base::layoutManager::IMenuLayoutManager::ActionInfo actionInfo : m_actionInfo)
    {
        ui::qt::container::QtMenuItemContainer::sptr menuItem = ui::qt::container::QtMenuItemContainer::New();

        QAction* action = menu->addAction( QString::fromStdString(actionInfo.m_name) );

        action->setSeparator(actionInfo.m_isSeparator);

        if (!actionInfo.m_icon.empty())
        {
            SIGHT_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " + actionInfo.m_icon.string());
            QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
            action->setIcon(icon);
        }
        if (actionInfo.m_type == ui::base::layoutManager::IMenuLayoutManager::QUIT)
        {
            action->setMenuRole(QAction::QuitRole);
        }
        else if (actionInfo.m_type == ui::base::layoutManager::IMenuLayoutManager::ABOUT)
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
            ui::qt::container::QtMenuContainer::sptr menu = ui::qt::container::QtMenuContainer::New();
            QMenu* qtMenu                                 = new QMenu();
            menu->setQtMenu(qtMenu);
            action->setMenu(qtMenu);
            m_menus.push_back(menu);
        }

        menuItem->setQtMenuItem(action);

        if(!actionInfo.m_isSeparator && !actionInfo.m_isMenu )
        {
            m_menuItems.push_back(menuItem);
            SIGHT_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
            ui::base::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

            ui::qt::ActionCallback::sptr qtCallback = ui::qt::ActionCallback::dynamicCast(callback);
            SIGHT_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", qtCallback);

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

void MenuLayoutManager::menuItemSetVisible(ui::base::container::fwMenuItem::sptr fwMenuItem, bool isVisible)
{
    ui::qt::container::QtMenuItemContainer::sptr menuItemContainer =
        ui::qt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::menuItemSetEnabled(ui::base::container::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    ui::qt::container::QtMenuItemContainer::sptr menuItemContainer =
        ui::qt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::menuItemSetChecked(ui::base::container::fwMenuItem::sptr fwMenuItem, bool isChecked)
{
    ui::qt::container::QtMenuItemContainer::sptr menuItemContainer =
        ui::qt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace sight::ui::qt
