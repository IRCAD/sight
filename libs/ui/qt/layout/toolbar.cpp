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

#include "ui/qt/container/toolbar.hpp"
#include "ui/qt/layout/toolbar.hpp"

#include "ui/qt/ActionCallback.hpp"
#include "ui/qt/container/menu.hpp"
#include "ui/qt/container/menu_item.hpp"
#include "ui/qt/container/widget.hpp"

#include <ui/__/macros.hpp>
#include <ui/qt/widget/AccordionMenu.hpp>

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

void toolbar::createLayout(ui::container::toolbar::sptr parent, const std::string& id)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::toolbar>(parent);
    SIGHT_ASSERT("dynamicCast toolbar to toolbar failed", m_parent);

    const QString qId = QString::fromStdString(id);

    QToolBar* toolBar = m_parent->getQtToolBar();
    toolBar->setObjectName(qId);

    if(m_style == "ToolButtonTextOnly")
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }
    else if(m_style == "ToolButtonTextBesideIcon")
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    else if(m_style == "ToolButtonTextUnderIcon")
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
    else if(m_style == "ToolButtonFollowStyle")
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    }
    else
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    [[maybe_unused]] QActionGroup* actionGroup  = nullptr;
    widget::AccordionMenu* accordionLayout      = nullptr;
    [[maybe_unused]] unsigned int menuItemIndex = 0;
    for(ui::layout::toolbar_manager::ActionInfo& actionInfo : m_actionInfo)
    {
        if(actionInfo.m_accordion == ui::layout::toolbar_manager::Accordion::NO)
        {
            accordionLayout = nullptr;
        }
        else if(actionInfo.m_accordion == ui::layout::toolbar_manager::Accordion::FIRST)
        {
            accordionLayout = new widget::AccordionMenu(toolBar);
            toolBar->addWidget(accordionLayout);
        }

        if(actionInfo.m_isSeparator)
        {
            if(actionInfo.m_size > 0)
            {
                auto* widget = new QWidget(toolBar);
                widget->setObjectName(qId + '/' + actionInfo.m_name.c_str());
                widget->setMinimumWidth(actionInfo.m_size);
                toolBar->addWidget(widget);
            }
            else
            {
                toolBar->addSeparator();
            }

            actionGroup = nullptr;
        }
        else if(actionInfo.m_isSpacer)
        {
            auto* spacer = new QWidget(toolBar);
            spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            if(accordionLayout != nullptr)
            {
                accordionLayout->addWidget(spacer);
            }
            else
            {
                toolBar->addWidget(spacer);
            }

            actionGroup = nullptr;
        }
        else if(actionInfo.m_isMenu)
        {
            ui::qt::container::menu::sptr menu = ui::qt::container::menu::make();
            auto* qtMenu                       = new QMenu(toolBar);
            menu->setQtMenu(qtMenu);

            auto* toolButton = new QToolButton(toolBar);
            toolButton->setObjectName(qId + '/' + actionInfo.m_name.c_str());
            if(toolBar->orientation() == Qt::Horizontal)
            {
                toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            }
            else
            {
                toolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            }

            toolButton->setMenu(qtMenu);
            toolButton->setPopupMode(QToolButton::InstantPopup);
            toolButton->setText(QString::fromStdString(actionInfo.m_name));
            toolButton->setObjectName(qId + '/' + actionInfo.m_name.c_str());

            if(!actionInfo.m_icon.empty())
            {
                QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
                toolButton->setIcon(icon);
                toolButton->setToolTip(QString::fromStdString(actionInfo.m_name));
                toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            }

            if(accordionLayout != nullptr)
            {
                accordionLayout->addWidget(toolButton);
            }
            else
            {
                toolBar->addWidget(toolButton);
            }

            m_menus.push_back(menu);
        }
        else if(actionInfo.m_isEditor)
        {
            ui::qt::container::widget::sptr container = ui::qt::container::widget::make();
            auto* widget                              = new QWidget(toolBar);
            widget->setObjectName(qId + '/' + actionInfo.m_name.c_str());
            container->setQtContainer(widget);

            if(toolBar->orientation() == Qt::Horizontal)
            {
                widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            }
            else
            {
                widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            }

            widget->adjustSize();
            if(accordionLayout != nullptr)
            {
                accordionLayout->addWidget(widget);
            }
            else
            {
                toolBar->addWidget(widget);
            }

            m_containers.push_back(container);
        }
        else
        {
            ui::qt::container::menu_item::sptr menuItem = ui::qt::container::menu_item::make();
            QAction* action                             = nullptr;
            if(accordionLayout != nullptr)
            {
                auto* toolButton = new QToolButton;
                toolButton->setToolButtonStyle(toolBar->toolButtonStyle());
                toolButton->setIconSize(toolBar->iconSize());

                action = new QAction(toolBar);
                toolButton->setDefaultAction(action);
                action->setText(QString::fromStdString(actionInfo.m_name));
                action->setToolTip(QString::fromStdString(actionInfo.m_name));
                if(!actionInfo.m_icon.empty())
                {
                    action->setIcon(QIcon(QString::fromStdString(actionInfo.m_icon.string())));
                }

                if(!actionInfo.m_icon2.empty())
                {
                    QIcon icon = action->icon();
                    icon.addFile(QString::fromStdString(actionInfo.m_icon2.string()), {}, QIcon::Normal, QIcon::On);
                    icon.addFile(QString::fromStdString(actionInfo.m_icon2.string()), {}, QIcon::Active, QIcon::On);
                    action->setIcon(icon);
                }

                if(toolBar->orientation() == Qt::Horizontal)
                {
                    toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
                    toolBar->setMinimumHeight(std::max(toolBar->minimumHeight(), toolButton->sizeHint().height()));
                }
                else
                {
                    toolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                    // The +5 is to take into account the margin of the toolbar
                    toolBar->setMinimumWidth(std::max(toolBar->minimumWidth(), toolButton->sizeHint().width() + 5));
                }

                accordionLayout->addWidget(toolButton);
            }
            else
            {
                if(!actionInfo.m_icon.empty())
                {
                    QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
                    if(!actionInfo.m_icon2.empty())
                    {
                        icon.addFile(
                            QString::fromStdString(actionInfo.m_icon2.string()),
                            QSize(),
                            QIcon::Normal,
                            QIcon::On
                        );
                        icon.addFile(
                            QString::fromStdString(actionInfo.m_icon2.string()),
                            QSize(),
                            QIcon::Active,
                            QIcon::On
                        );
                    }

                    action = toolBar->addAction(icon, QString::fromStdString(actionInfo.m_name));
                }
                else
                {
                    action = toolBar->addAction(QString::fromStdString(actionInfo.m_name));
                }
            }

            action->setObjectName(qId + '/' + actionInfo.m_name.c_str());
            action->setCheckable(actionInfo.m_isCheckable || actionInfo.m_isRadio);

            if(actionInfo.m_isRadio)
            {
                if(actionGroup == nullptr)
                {
                    actionGroup = new QActionGroup(toolBar);
                }

                actionGroup->addAction(action);
            }

            // create shortcut
            if(!actionInfo.m_shortcut.empty())
            {
                action->setShortcut(QKeySequence(QString::fromStdString(actionInfo.m_shortcut)));
            }

            menuItem->setQtMenuItem(action);

            m_menuItems.push_back(menuItem);
            SIGHT_ASSERT("No callback found for menu: " << actionInfo.m_name, menuItemIndex < m_callbacks.size());
            ui::menu_item_callback::sptr callback = m_callbacks.at(menuItemIndex);

            ui::qt::ActionCallback::sptr qtCallback = std::dynamic_pointer_cast<ui::qt::ActionCallback>(callback);
            SIGHT_ASSERT("dynamicCast menu_item_callback to ActionCallback failed", qtCallback);

            QObject::connect(action, SIGNAL(triggered(bool)), qtCallback.get(), SLOT(executeQt(bool)));
            QObject::connect(action, SIGNAL(toggled(bool)), qtCallback.get(), SLOT(checkQt(bool)));
            menuItemIndex++;
        }
    }

    // Parse all QToolButton and resize to the greater one.
    if(m_unifyButtonSize)
    {
        int max         = -1;
        const auto list = toolBar->findChildren<QToolButton*>();
        for(auto* const tb : list)
        {
            // Check width if horizontal toolbar, height for vertical.
            const auto size =
                (toolBar->orientation() == Qt::Horizontal) ? tb->sizeHint().width() : tb->sizeHint().height();
            max = std::max(max, size);
        }

        for(auto* const tb : list)
        {
            // Set minimum width on horizontal toolbar, set minimum height for vertical.
            (toolBar->orientation() == Qt::Horizontal) ? tb->setMinimumWidth(max) : tb->setMaximumHeight(max);
        }
    }

    m_toggleToolbarVisibilityAction = toolBar->toggleViewAction();
    m_toggleToolbarVisibilityAction->setVisible(false);
}

//-----------------------------------------------------------------------------

void toolbar::destroyLayout()
{
    this->destroyActions();
    m_parent->clean();
    m_menuItems.clear();
}

//-----------------------------------------------------------------------------

void toolbar::menuItemSetVisible(ui::container::menu_item::sptr menu_item, bool isVisible)
{
    ui::qt::container::menu_item::sptr menuItemContainer =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(menu_item);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void toolbar::menuItemSetEnabled(ui::container::menu_item::sptr menu_item, bool isEnabled)
{
    ui::qt::container::menu_item::sptr menuItemContainer =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(menu_item);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void toolbar::menuItemSetChecked(ui::container::menu_item::sptr menu_item, bool isChecked)
{
    ui::qt::container::menu_item::sptr menuItemContainer =
        std::dynamic_pointer_cast<ui::qt::container::menu_item>(menu_item);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

void toolbar::setVisible(bool isVisible)
{
    if(m_toggleToolbarVisibilityAction != nullptr)
    {
        m_toggleToolbarVisibilityAction->setChecked(!isVisible);
        m_toggleToolbarVisibilityAction->trigger();
    }
}

} // namespace sight::ui::qt::layout
