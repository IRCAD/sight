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

#include "ui/qt/container/menu.hpp"
#include "ui/qt/layout/menu.hpp"

#include "ui/qt/ActionCallback.hpp"
#include "ui/qt/container/menu_item.hpp"

#include <ui/__/macros.hpp>

#include <QAction>
#include <QActionGroup>
#include <QMenu>

#include <functional>

SIGHT_REGISTER_GUI(sight::ui::qt::layout::menu, sight::ui::layout::menu_manager::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void menu::createLayout(ui::container::menu::sptr parent, const std::string& id)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::menu>(parent);
    SIGHT_ASSERT("dynamicCast menu to menu failed", m_parent);

    const QString qId = QString::fromStdString(id);

    QMenu* menu = m_parent->getQtMenu();
    menu->setObjectName(qId);

    QActionGroup* actionGroup  = nullptr;
    unsigned int menuItemIndex = 0;
    for(const ui::layout::menu_manager::ActionInfo& actionInfo : m_actionInfo)
    {
        ui::qt::container::menu_item::sptr menuItem = ui::qt::container::menu_item::make();

        QAction* action = menu->addAction(QString::fromStdString(actionInfo.m_name));
        action->setObjectName(qId + '/' + actionInfo.m_name.c_str());

        action->setSeparator(actionInfo.m_isSeparator);

        if(!actionInfo.m_icon.empty())
        {
            QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
            action->setIcon(icon);
        }

        if(actionInfo.m_type == ui::layout::menu_manager::QUIT)
        {
            action->setMenuRole(QAction::QuitRole);
        }
        else if(actionInfo.m_type == ui::layout::menu_manager::ABOUT)
        {
            action->setMenuRole(QAction::AboutRole);
        }
        else
        {
            action->setMenuRole(QAction::NoRole);
        }

        action->setCheckable(actionInfo.m_isCheckable || actionInfo.m_isRadio);

        if(actionInfo.m_isRadio)
        {
            if(actionGroup == nullptr)
            {
                actionGroup = new QActionGroup(menu);
            }

            actionGroup->addAction(action);
        }

        // create shortcut
        if(!actionInfo.m_shortcut.empty())
        {
            action->setShortcut(QKeySequence(QString::fromStdString(actionInfo.m_shortcut)));
        }

        if(actionInfo.m_isMenu)
        {
            ui::qt::container::menu::sptr menuContainer = ui::qt::container::menu::make();
            auto* qtMenu                                = new QMenu();
            menuContainer->setQtMenu(qtMenu);
            action->setMenu(qtMenu);
            m_menus.push_back(menuContainer);
        }

        menuItem->setQtMenuItem(action);

        if(!actionInfo.m_isSeparator && !actionInfo.m_isMenu)
        {
            m_menuItems.push_back(menuItem);
            SIGHT_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
            ui::menu_item_callback::sptr callback = m_callbacks.at(menuItemIndex);

            ui::qt::ActionCallback::sptr qtCallback = std::dynamic_pointer_cast<ui::qt::ActionCallback>(callback);
            SIGHT_ASSERT("dynamicCast menu_item_callback to ActionCallback failed", qtCallback);

            QObject::connect(action, SIGNAL(triggered(bool)), qtCallback.get(), SLOT(executeQt(bool)));
            QObject::connect(action, SIGNAL(toggled(bool)), qtCallback.get(), SLOT(checkQt(bool)));
            menuItemIndex++;
        }
        else
        {
            actionGroup = nullptr;
        }
    }
}

//-----------------------------------------------------------------------------

void menu::destroyLayout()
{
    this->destroyActions();
    m_menuItems.clear();
    m_parent->clean();
}

//-----------------------------------------------------------------------------

void menu::menuItemSetVisible(ui::container::menu_item::sptr menu_item, bool isVisible)
{
    ui::qt::container::menu_item::sptr menuItemContainer =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(menu_item);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void menu::menuItemSetEnabled(ui::container::menu_item::sptr menu_item, bool isEnabled)
{
    ui::qt::container::menu_item::sptr menuItemContainer =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(menu_item);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void menu::menuItemSetChecked(ui::container::menu_item::sptr menu_item, bool isChecked)
{
    ui::qt::container::menu_item::sptr menuItemContainer =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(menu_item);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
