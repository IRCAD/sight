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

void menu::create_layout(ui::container::menu::sptr _parent, const std::string& _id)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::menu>(_parent);
    SIGHT_ASSERT("dynamicCast menu to menu failed", m_parent);

    const QString q_id = QString::fromStdString(_id);

    QMenu* menu = m_parent->get_qt_menu();
    menu->setObjectName(q_id);

    QActionGroup* action_group   = nullptr;
    unsigned int menu_item_index = 0;
    for(const ui::layout::menu_manager::action_info& action_info : m_action_info)
    {
        ui::qt::container::menu_item::sptr menu_item = ui::qt::container::menu_item::make();

        QAction* action = menu->addAction(QString::fromStdString(action_info.m_name));
        action->setObjectName(q_id + '/' + action_info.m_name.c_str());

        action->setSeparator(action_info.m_is_separator);

        if(!action_info.m_icon.empty())
        {
            QIcon icon(QString::fromStdString(action_info.m_icon.string()));
            action->setIcon(icon);
        }

        if(action_info.m_type == ui::layout::menu_manager::quit)
        {
            action->setMenuRole(QAction::QuitRole);
        }
        else if(action_info.m_type == ui::layout::menu_manager::about)
        {
            action->setMenuRole(QAction::AboutRole);
        }
        else
        {
            action->setMenuRole(QAction::NoRole);
        }

        action->setCheckable(action_info.m_is_checkable || action_info.m_is_radio);

        if(action_info.m_is_radio)
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

        if(action_info.m_is_menu)
        {
            ui::qt::container::menu::sptr menu_container = ui::qt::container::menu::make();
            auto* qt_menu                                = new QMenu();
            menu_container->set_qt_menu(qt_menu);
            action->setMenu(qt_menu);
            m_menus.push_back(menu_container);
        }

        menu_item->set_qt_menu_item(action);

        if(!action_info.m_is_separator && !action_info.m_is_menu)
        {
            m_menu_items.push_back(menu_item);
            SIGHT_ASSERT("No callback found for menu" << action_info.m_name, menu_item_index < m_callbacks.size());
            ui::menu_item_callback::sptr callback = m_callbacks.at(menu_item_index);

            ui::qt::action_callback::sptr qt_callback = std::dynamic_pointer_cast<ui::qt::action_callback>(callback);
            SIGHT_ASSERT("dynamicCast menu_item_callback to action_callback failed", qt_callback);

            QObject::connect(action, &QAction::triggered, qt_callback.get(), &ui::qt::action_callback::execute_qt);
            QObject::connect(action, &QAction::toggled, qt_callback.get(), &ui::qt::action_callback::check_qt);
            menu_item_index++;
        }
        else
        {
            action_group = nullptr;
        }
    }
}

//-----------------------------------------------------------------------------

void menu::destroy_layout()
{
    this->destroy_actions();
    m_menu_items.clear();
    m_parent->clean();
}

//-----------------------------------------------------------------------------

void menu::menu_item_set_visible(ui::container::menu_item::sptr _menu_item, bool _is_visible)
{
    ui::qt::container::menu_item::sptr menu_item_container =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(_menu_item);
    QAction* action = menu_item_container->get_qt_menu_item();
    action->setVisible(_is_visible);
}

//-----------------------------------------------------------------------------

void menu::menu_item_set_enabled(ui::container::menu_item::sptr _menu_item, bool _is_enabled)
{
    ui::qt::container::menu_item::sptr menu_item_container =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(_menu_item);
    QAction* action = menu_item_container->get_qt_menu_item();
    action->setEnabled(_is_enabled);
}

//-----------------------------------------------------------------------------

void menu::menu_item_set_checked(ui::container::menu_item::sptr _menu_item, bool _is_checked)
{
    ui::qt::container::menu_item::sptr menu_item_container =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(_menu_item);
    QAction* action = menu_item_container->get_qt_menu_item();
    action->setChecked(_is_checked);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
