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

#include "ui/qt/action_callback.hpp"
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

void menu::createLayout(ui::container::menu::sptr _parent, const std::string& _id)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::menu>(_parent);
    SIGHT_ASSERT("dynamicCast menu to menu failed", m_parent);

    const QString q_id = QString::fromStdString(_id);

    QMenu* menu = m_parent->getQtMenu();
    menu->setObjectName(q_id);

    QActionGroup* action_group   = nullptr;
    unsigned int menu_item_index = 0;
    for(const ui::layout::menu_manager::ActionInfo& action_info : m_actionInfo)
    {
        ui::qt::container::menu_item::sptr menu_item = ui::qt::container::menu_item::make();

        QAction* action = menu->addAction(QString::fromStdString(action_info.m_name));
        action->setObjectName(q_id + '/' + action_info.m_name.c_str());

        action->setSeparator(action_info.m_isSeparator);

        if(!action_info.m_icon.empty())
        {
            QIcon icon(QString::fromStdString(action_info.m_icon.string()));
            action->setIcon(icon);
        }

        if(action_info.m_type == ui::layout::menu_manager::QUIT)
        {
            action->setMenuRole(QAction::QuitRole);
        }
        else if(action_info.m_type == ui::layout::menu_manager::ABOUT)
        {
            action->setMenuRole(QAction::AboutRole);
        }
        else
        {
            action->setMenuRole(QAction::NoRole);
        }

        action->setCheckable(action_info.m_isCheckable || action_info.m_isRadio);

        if(action_info.m_isRadio)
        {
            if(action_group == nullptr)
            {
                action_group = new QActionGroup(menu);
            }

            action_group->addAction(action);
        }

        // create shortcut
        if(!action_info.m_shortcut.empty())
        {
            action->setShortcut(QKeySequence(QString::fromStdString(action_info.m_shortcut)));
        }

        if(action_info.m_isMenu)
        {
            ui::qt::container::menu::sptr menu_container = ui::qt::container::menu::make();
            auto* qt_menu                                = new QMenu();
            menu_container->setQtMenu(qt_menu);
            action->setMenu(qt_menu);
            m_menus.push_back(menu_container);
        }

        menu_item->setQtMenuItem(action);

        if(!action_info.m_isSeparator && !action_info.m_isMenu)
        {
            m_menuItems.push_back(menu_item);
            SIGHT_ASSERT("No callback found for menu" << action_info.m_name, menu_item_index < m_callbacks.size());
            ui::menu_item_callback::sptr callback = m_callbacks.at(menu_item_index);

            ui::qt::action_callback::sptr qt_callback = std::dynamic_pointer_cast<ui::qt::action_callback>(callback);
            SIGHT_ASSERT("dynamicCast menu_item_callback to action_callback failed", qt_callback);

            QObject::connect(action, SIGNAL(triggered(bool)), qt_callback.get(), SLOT(executeQt(bool)));
            QObject::connect(action, SIGNAL(toggled(bool)), qt_callback.get(), SLOT(checkQt(bool)));
            menu_item_index++;
        }
        else
        {
            action_group = nullptr;
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

void menu::menuItemSetVisible(ui::container::menu_item::sptr _menu_item, bool _is_visible)
{
    ui::qt::container::menu_item::sptr menu_item_container =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(_menu_item);
    QAction* action = menu_item_container->getQtMenuItem();
    action->setVisible(_is_visible);
}

//-----------------------------------------------------------------------------

void menu::menuItemSetEnabled(ui::container::menu_item::sptr _menu_item, bool _is_enabled)
{
    ui::qt::container::menu_item::sptr menu_item_container =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(_menu_item);
    QAction* action = menu_item_container->getQtMenuItem();
    action->setEnabled(_is_enabled);
}

//-----------------------------------------------------------------------------

void menu::menuItemSetChecked(ui::container::menu_item::sptr _menu_item, bool _is_checked)
{
    ui::qt::container::menu_item::sptr menu_item_container =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(_menu_item);
    QAction* action = menu_item_container->getQtMenuItem();
    action->setChecked(_is_checked);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
