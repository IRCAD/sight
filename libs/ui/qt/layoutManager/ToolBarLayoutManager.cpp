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

#include "ui/qt/layoutManager/ToolBarLayoutManager.hpp"

#include "ui/qt/ActionCallback.hpp"
#include "ui/qt/container/QtContainer.hpp"
#include "ui/qt/container/QtMenuContainer.hpp"
#include "ui/qt/container/QtMenuItemContainer.hpp"
#include "ui/qt/container/QtToolBarContainer.hpp"

#include <ui/base/registry/macros.hpp>

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include <functional>

fwGuiRegisterMacro(
    sight::ui::qt::layoutManager::ToolBarLayoutManager,
    sight::ui::base::layoutManager::IToolBarLayoutManager::REGISTRY_KEY
);

namespace sight::ui::qt
{

namespace layoutManager
{

//-----------------------------------------------------------------------------

ToolBarLayoutManager::ToolBarLayoutManager(ui::base::GuiBaseObject::Key)
{
}

//-----------------------------------------------------------------------------

ToolBarLayoutManager::~ToolBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::createLayout(ui::base::container::fwToolBar::sptr parent)
{
    m_parent = ui::qt::container::QtToolBarContainer::dynamicCast(parent);
    SIGHT_ASSERT("dynamicCast fwToolBar to QtToolBarContainer failed", m_parent);

    QToolBar* toolBar = m_parent->getQtToolBar();

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

    QActionGroup* actionGroup  = 0;
    unsigned int menuItemIndex = 0;
    for(ui::base::layoutManager::IToolBarLayoutManager::ActionInfo& actionInfo : m_actionInfo)
    {
        if(actionInfo.m_isSeparator)
        {
            if(actionInfo.m_size > 0)
            {
                QWidget* widget = new QWidget(toolBar);
                widget->setMinimumWidth(actionInfo.m_size);
                toolBar->addWidget(widget);
            }
            else
            {
                toolBar->addSeparator();
            }

            actionGroup = 0;
        }
        else if(actionInfo.m_isSpacer)
        {
            QWidget* spacer = new QWidget(toolBar);
            spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            toolBar->addWidget(spacer);
            actionGroup = 0;
        }
        else if(actionInfo.m_isMenu)
        {
            ui::qt::container::QtMenuContainer::sptr menu = ui::qt::container::QtMenuContainer::New();
            QMenu* qtMenu                                 = new QMenu(toolBar);
            menu->setQtMenu(qtMenu);

            QToolButton* toolButton = new QToolButton(toolBar);
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

            if(!actionInfo.m_icon.empty())
            {
                QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
                toolButton->setIcon(icon);
                toolButton->setToolTip(QString::fromStdString(actionInfo.m_name));
                toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            }

            toolBar->addWidget(toolButton);
            m_menus.push_back(menu);
        }
        else if(actionInfo.m_isEditor)
        {
            ui::qt::container::QtContainer::sptr container = ui::qt::container::QtContainer::New();
            QWidget* widget                                = new QWidget(toolBar);
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
            toolBar->addWidget(widget);

            m_containers.push_back(container);
        }
        else
        {
            ui::qt::container::QtMenuItemContainer::sptr menuItem = ui::qt::container::QtMenuItemContainer::New();
            QAction* action;
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

            action->setCheckable(actionInfo.m_isCheckable || actionInfo.m_isRadio);

            if(actionInfo.m_isRadio)
            {
                if(!actionGroup)
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
            ui::base::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

            ui::qt::ActionCallback::sptr qtCallback = ui::qt::ActionCallback::dynamicCast(callback);
            SIGHT_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", qtCallback);

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
        for(const auto tb : list)
        {
            // Check width if horizontal toolbar, height for vertical.
            const auto size =
                (toolBar->orientation() == Qt::Horizontal) ? tb->sizeHint().width() : tb->sizeHint().height();
            max = std::max(max, size);
        }

        for(const auto tb : list)
        {
            // Set minimum width on horizontal toolbar, set minimum height for vertical.
            (toolBar->orientation() == Qt::Horizontal) ? tb->setMinimumWidth(max) : tb->setMaximumHeight(max);
        }
    }
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::destroyLayout()
{
    this->destroyActions();
    m_parent->clean();
    m_menuItems.clear();
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetVisible(ui::base::container::fwMenuItem::sptr fwMenuItem, bool isVisible)
{
    ui::qt::container::QtMenuItemContainer::sptr menuItemContainer =
        ui::qt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetEnabled(ui::base::container::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    ui::qt::container::QtMenuItemContainer::sptr menuItemContainer =
        ui::qt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetChecked(ui::base::container::fwMenuItem::sptr fwMenuItem, bool isChecked)
{
    ui::qt::container::QtMenuItemContainer::sptr menuItemContainer =
        ui::qt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager

} // namespace sight::ui::qt
