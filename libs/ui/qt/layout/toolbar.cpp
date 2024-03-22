/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "ui/qt/container/toolbar.hpp"
#include "ui/qt/layout/toolbar.hpp"

#include "ui/qt/action_callback.hpp"
#include "ui/qt/container/menu.hpp"
#include "ui/qt/container/menu_item.hpp"
#include "ui/qt/container/widget.hpp"

#include <ui/__/macros.hpp>
#include <ui/qt/widget/accordion_menu.hpp>

#include <QActionGroup>
#include <qboxlayout.h>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include <functional>

SIGHT_REGISTER_GUI(sight::ui::qt::layout::toolbar, sight::ui::layout::toolbar_manager::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void toolbar::create_layout(ui::container::toolbar::sptr _parent, const std::string& _id)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::toolbar>(_parent);
    SIGHT_ASSERT("dynamicCast toolbar to toolbar failed", m_parent);

    const QString q_id = QString::fromStdString(_id);

    QToolBar* tool_bar = m_parent->get_qt_tool_bar();
    tool_bar->setObjectName(q_id);

    if(m_style == "ToolButtonTextOnly")
    {
        tool_bar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }
    else if(m_style == "ToolButtonTextBesideIcon")
    {
        tool_bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    else if(m_style == "ToolButtonTextUnderIcon")
    {
        tool_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
    else if(m_style == "ToolButtonFollowStyle")
    {
        tool_bar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    }
    else
    {
        tool_bar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    [[maybe_unused]] QActionGroup* action_group   = nullptr;
    widget::accordion_menu* accordion_layout      = nullptr;
    [[maybe_unused]] unsigned int menu_item_index = 0;
    for(ui::layout::toolbar_manager::action_info& action_info : m_action_info)
    {
        if(action_info.m_accordion == ui::layout::toolbar_manager::accordion::no)
        {
            accordion_layout = nullptr;
        }
        else if(action_info.m_accordion == ui::layout::toolbar_manager::accordion::first)
        {
            accordion_layout = new widget::accordion_menu(tool_bar);
            tool_bar->addWidget(accordion_layout);
        }

        if(action_info.m_is_separator)
        {
            if(action_info.m_size > 0)
            {
                auto* widget = new QWidget(tool_bar);
                widget->setObjectName(q_id + '/' + action_info.m_name.c_str());
                widget->setMinimumWidth(action_info.m_size);
                tool_bar->addWidget(widget);
            }
            else
            {
                tool_bar->addSeparator();
            }

            action_group = nullptr;
        }
        else if(action_info.m_is_spacer)
        {
            auto* spacer = new QWidget(tool_bar);
            spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            if(accordion_layout != nullptr)
            {
                accordion_layout->add_widget(spacer);
            }
            else
            {
                tool_bar->addWidget(spacer);
            }

            action_group = nullptr;
        }
        else if(action_info.m_is_menu)
        {
            ui::qt::container::menu::sptr menu = ui::qt::container::menu::make();
            auto* qt_menu                      = new QMenu(tool_bar);
            menu->set_qt_menu(qt_menu);

            auto* tool_button = new QToolButton(tool_bar);
            tool_button->setObjectName(q_id + '/' + action_info.m_name.c_str());
            if(tool_bar->orientation() == Qt::Horizontal)
            {
                tool_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            }
            else
            {
                tool_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            }

            tool_button->setMenu(qt_menu);
            tool_button->setPopupMode(QToolButton::InstantPopup);
            tool_button->setText(QString::fromStdString(action_info.m_name));
            tool_button->setObjectName(q_id + '/' + action_info.m_name.c_str());

            if(!action_info.m_icon.empty())
            {
                QIcon icon(QString::fromStdString(action_info.m_icon.string()));
                tool_button->setIcon(icon);
                tool_button->setToolTip(QString::fromStdString(action_info.m_name));
                tool_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            }

            if(accordion_layout != nullptr)
            {
                accordion_layout->add_widget(tool_button);
            }
            else
            {
                tool_bar->addWidget(tool_button);
            }

            m_menus.push_back(menu);
        }
        else if(action_info.m_is_editor)
        {
            ui::qt::container::widget::sptr container = ui::qt::container::widget::make();
            auto* widget                              = new QWidget(tool_bar);
            widget->setObjectName(q_id + '/' + action_info.m_name.c_str());
            container->set_qt_container(widget);

            if(tool_bar->orientation() == Qt::Horizontal)
            {
                widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            }
            else
            {
                widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            }

            widget->adjustSize();
            if(accordion_layout != nullptr)
            {
                accordion_layout->add_widget(widget);
            }
            else
            {
                tool_bar->addWidget(widget);
            }

            m_containers.push_back(container);
        }
        else
        {
            ui::qt::container::menu_item::sptr menu_item = ui::qt::container::menu_item::make();
            QAction* action                              = nullptr;
            if(accordion_layout != nullptr)
            {
                auto* tool_button = new QToolButton;
                tool_button->setToolButtonStyle(tool_bar->toolButtonStyle());
                tool_button->setIconSize(tool_bar->iconSize());

                action = new QAction(tool_bar);
                tool_button->setDefaultAction(action);
                action->setText(QString::fromStdString(action_info.m_name));
                action->setToolTip(QString::fromStdString(action_info.m_name));
                if(!action_info.m_icon.empty())
                {
                    action->setIcon(QIcon(QString::fromStdString(action_info.m_icon.string())));
                }

                if(!action_info.m_icon2.empty())
                {
                    QIcon icon = action->icon();
                    icon.addFile(QString::fromStdString(action_info.m_icon2.string()), {}, QIcon::Normal, QIcon::On);
                    icon.addFile(QString::fromStdString(action_info.m_icon2.string()), {}, QIcon::Active, QIcon::On);
                    action->setIcon(icon);
                }

                if(tool_bar->orientation() == Qt::Horizontal)
                {
                    tool_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
                    tool_bar->setMinimumHeight(std::max(tool_bar->minimumHeight(), tool_button->sizeHint().height()));
                }
                else
                {
                    tool_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                    // The +5 is to take into account the margin of the toolbar
                    tool_bar->setMinimumWidth(std::max(tool_bar->minimumWidth(), tool_button->sizeHint().width() + 5));
                }

                accordion_layout->add_widget(tool_button);
            }
            else
            {
                if(!action_info.m_icon.empty())
                {
                    QIcon icon(QString::fromStdString(action_info.m_icon.string()));
                    if(!action_info.m_icon2.empty())
                    {
                        icon.addFile(
                            QString::fromStdString(action_info.m_icon2.string()),
                            QSize(),
                            QIcon::Normal,
                            QIcon::On
                        );
                        icon.addFile(
                            QString::fromStdString(action_info.m_icon2.string()),
                            QSize(),
                            QIcon::Active,
                            QIcon::On
                        );
                    }

                    action = tool_bar->addAction(icon, QString::fromStdString(action_info.m_name));
                }
                else
                {
                    action = tool_bar->addAction(QString::fromStdString(action_info.m_name));
                }
            }

            action->setObjectName(q_id + '/' + action_info.m_name.c_str());
            action->setCheckable(action_info.m_is_checkable || action_info.m_is_radio);

            if(action_info.m_is_radio)
            {
                if(action_group == nullptr)
                {
                    action_group = new QActionGroup(tool_bar);
                }

                action_group->addAction(action);
            }

            // create shortcut
            QList<QKeySequence> total_sequence;
            for(auto const& shortcut_str : action_info.m_shortcuts)
            {
                total_sequence.append(QKeySequence(QString::fromStdString(shortcut_str)));
            }

            if(!total_sequence.empty())
            {
                action->setShortcuts(total_sequence);
            }

            action->setAutoRepeat(false);

            menu_item->set_qt_menu_item(action);

            m_menu_items.push_back(menu_item);
            SIGHT_ASSERT("No callback found for menu: " << action_info.m_name, menu_item_index < m_callbacks.size());
            ui::menu_item_callback::sptr callback = m_callbacks.at(menu_item_index);

            ui::qt::action_callback::sptr qt_callback = std::dynamic_pointer_cast<ui::qt::action_callback>(callback);
            SIGHT_ASSERT("dynamicCast menu_item_callback to action_callback failed", qt_callback);

            QObject::connect(action, &QAction::triggered, qt_callback.get(), &ui::qt::action_callback::execute_qt);
            QObject::connect(action, &QAction::toggled, qt_callback.get(), &ui::qt::action_callback::check_qt);
            menu_item_index++;
        }
    }

    // Parse all QToolButton and resize to the greater one.
    if(m_unify_button_size)
    {
        const auto list = tool_bar->findChildren<QToolButton*>();
        QSize max;
        for(auto* const tb : list)
        {
            max = max.expandedTo(tb->sizeHint());
        }

        for(auto* const tb : list)
        {
            tb->setFixedSize(max);
        }
    }

    m_toggle_toolbar_visibility_action = tool_bar->toggleViewAction();
    m_toggle_toolbar_visibility_action->setVisible(false);
}

//-----------------------------------------------------------------------------

void toolbar::destroy_layout()
{
    this->destroy_actions();
    m_parent->clean();
    m_menu_items.clear();
}

//-----------------------------------------------------------------------------

void toolbar::menu_item_set_visible(ui::container::menu_item::sptr _menu_item, bool _is_visible)
{
    ui::qt::container::menu_item::sptr menu_item_container =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(_menu_item);
    QAction* action = menu_item_container->get_qt_menu_item();
    action->setVisible(_is_visible);
}

//-----------------------------------------------------------------------------

void toolbar::menu_item_set_enabled(ui::container::menu_item::sptr _menu_item, bool _is_enabled)
{
    ui::qt::container::menu_item::sptr menu_item_container =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(_menu_item);
    QAction* action = menu_item_container->get_qt_menu_item();
    action->setEnabled(_is_enabled);
}

//-----------------------------------------------------------------------------

void toolbar::menu_item_set_checked(ui::container::menu_item::sptr _menu_item, bool _is_checked)
{
    ui::qt::container::menu_item::sptr menu_item_container =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(_menu_item);
    QAction* action = menu_item_container->get_qt_menu_item();
    action->setChecked(_is_checked);
}

//-----------------------------------------------------------------------------

void toolbar::set_visible(bool _is_visible)
{
    if(m_toggle_toolbar_visibility_action != nullptr)
    {
        m_toggle_toolbar_visibility_action->setChecked(!_is_visible);
        m_toggle_toolbar_visibility_action->trigger();
    }
}

} // namespace sight::ui::qt::layout
