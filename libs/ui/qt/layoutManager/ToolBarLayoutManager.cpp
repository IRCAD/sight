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

#include "ui/qt/layoutManager/ToolBarLayoutManager.hpp"

#include "ui/qt/ActionCallback.hpp"
#include "ui/qt/container/QtContainer.hpp"
#include "ui/qt/container/QtMenuContainer.hpp"
#include "ui/qt/container/QtMenuItemContainer.hpp"
#include "ui/qt/container/QtToolBarContainer.hpp"

#include <ui/base/registry/macros.hpp>

#include <QActionGroup>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include <functional>

SIGHT_REGISTER_GUI(
    sight::ui::qt::layoutManager::ToolBarLayoutManager,
    sight::ui::base::layoutManager::IToolBarLayoutManager::REGISTRY_KEY
);

namespace sight::ui::qt::layoutManager
{

//-----------------------------------------------------------------------------

ToolBarLayoutManager::ToolBarLayoutManager(ui::base::GuiBaseObject::Key /*unused*/)
{
}

//-----------------------------------------------------------------------------

ToolBarLayoutManager::~ToolBarLayoutManager()
= default;

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::createLayout(ui::base::container::fwToolBar::sptr parent, const std::string& id)
{
    m_parent = ui::qt::container::QtToolBarContainer::dynamicCast(parent);
    SIGHT_ASSERT("dynamicCast fwToolBar to QtToolBarContainer failed", m_parent);

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

    QActionGroup* actionGroup  = nullptr;
    unsigned int menuItemIndex = 0;
    for(ui::base::layoutManager::IToolBarLayoutManager::ActionInfo& actionInfo : m_actionInfo)
    {
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
            toolBar->addWidget(spacer);
            actionGroup = nullptr;
        }
        else if(actionInfo.m_isMenu)
        {
            ui::qt::container::QtMenuContainer::sptr menu = ui::qt::container::QtMenuContainer::New();
            auto* qtMenu                                  = new QMenu(toolBar);
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

            toolBar->addWidget(toolButton);
            m_menus.push_back(menu);
        }
        else if(actionInfo.m_isEditor)
        {
            ui::qt::container::QtContainer::sptr container = ui::qt::container::QtContainer::New();
            auto* widget                                   = new QWidget(toolBar);
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
            toolBar->addWidget(widget);

            m_containers.push_back(container);
        }
        else
        {
            ui::qt::container::QtMenuItemContainer::sptr menuItem = ui::qt::container::QtMenuItemContainer::New();
            QAction* action                                       = nullptr;
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

void ToolBarLayoutManager::setVisible(bool isVisible)
{
    if(m_toggleToolbarVisibilityAction != nullptr)
    {
        m_toggleToolbarVisibilityAction->setChecked(!isVisible);
        m_toggleToolbarVisibilityAction->trigger();
    }
}

} // namespace sight::ui::qt::layoutManager
