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

#include "fwGuiQml/layoutManager/ToolBarLayoutManager.hpp"

#include "fwGuiQml/ActionCallback.hpp"
#include "fwGuiQml/container/QmlContainer.hpp"
#include "fwGuiQml/container/QmlMenuContainer.hpp"
#include "fwGuiQml/container/QmlMenuItemContainer.hpp"
#include "fwGuiQml/container/QmlToolBarContainer.hpp"

#include <fwGui/registry/macros.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/lambda/lambda.hpp>

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include <functional>

fwGuiRegisterMacro( ::fwGui::layoutManager::ToolBarLayoutManager,
                    ::fwGui::layoutManager::IToolBarLayoutManager::REGISTRY_KEY );

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

ToolBarLayoutManager::ToolBarLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

ToolBarLayoutManager::~ToolBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::createLayout( ::fwGui::container::fwToolBar::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiQml::container::QmlToolBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwToolBar to QmlToolBarContainer failed", m_parent);

    QToolBar* toolBar = m_parent->getQmlToolBar();

    QActionGroup* actionGroup  = 0;
    unsigned int menuItemIndex = 0;
    for ( ::fwGui::layoutManager::IToolBarLayoutManager::ActionInfo actionInfo : m_actionInfo)
    {
        if (actionInfo.m_isSeparator)
        {
            if (actionInfo.m_size > 0)
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
        else if (actionInfo.m_isSpacer)
        {
            QWidget* spacer = new QWidget(toolBar);
            spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            toolBar->addWidget(spacer);
            actionGroup = 0;
        }
        else if (actionInfo.m_isMenu)
        {
            ::fwGuiQml::container::QmlMenuContainer::sptr menu = ::fwGuiQml::container::QmlMenuContainer::New();
            QMenu* qmlMenu = new QMenu(toolBar);
            menu->setQmlMenu(qmlMenu);

            QToolButton* toolButton = new QToolButton(toolBar);
            if (toolBar->orientation() == Qt::Horizontal)
            {
                toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            }
            else
            {
                toolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            }
            toolButton->setMenu(qmlMenu);
            toolButton->setPopupMode(QToolButton::InstantPopup);
            toolButton->setText(QString::fromStdString(actionInfo.m_name));
            if (!actionInfo.m_icon.empty())
            {
                QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
                toolButton->setIcon(icon);
                toolButton->setToolTip(QString::fromStdString(actionInfo.m_name));
                toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            }
            toolBar->addWidget(toolButton);
            m_menus.push_back(menu);
        }
        else if (actionInfo.m_isEditor)
        {
            ::fwGuiQml::container::QmlContainer::sptr container = ::fwGuiQml::container::QmlContainer::New();
            QWidget* widget = new QWidget(toolBar);
            container->setQmlContainer(widget);

            if (toolBar->orientation() == Qt::Horizontal)
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
            ::fwGuiQml::container::QmlMenuItemContainer::sptr menuItem =
                ::fwGuiQml::container::QmlMenuItemContainer::New();
            QAction* action;
            if (!actionInfo.m_icon.empty())
            {
                QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
                if (!actionInfo.m_icon2.empty())
                {
                    icon.addFile(QString::fromStdString(actionInfo.m_icon2.string()), QSize(), QIcon::Normal,
                                 QIcon::On);
                    icon.addFile(QString::fromStdString(actionInfo.m_icon2.string()), QSize(), QIcon::Active,
                                 QIcon::On);
                }
                action = toolBar->addAction( icon, QString::fromStdString(actionInfo.m_name) );
            }
            else
            {
                action = toolBar->addAction( QString::fromStdString(actionInfo.m_name) );
            }

            action->setCheckable(actionInfo.m_isCheckable || actionInfo.m_isRadio);

            if (actionInfo.m_isRadio)
            {
                if (!actionGroup)
                {
                    actionGroup = new QActionGroup(toolBar);
                }
                actionGroup->addAction(action);
            }

            // create shortcut
            if( !actionInfo.m_shortcut.empty() )
            {
                action->setShortcut(QKeySequence(QString::fromStdString(actionInfo.m_shortcut)));
            }

            menuItem->setQmlMenuItem(action);

            m_menuItems.push_back(menuItem);
            OSLM_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
            ::fwGui::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

            ::fwGuiQml::ActionCallback::sptr qmlCallback = ::fwGuiQml::ActionCallback::dynamicCast(callback);
            SLM_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", qmlCallback);

            QObject::connect( action, SIGNAL(triggered(bool)), qmlCallback.get(), SLOT(executeQt(bool)));
            QObject::connect( action, SIGNAL(toggled(bool)), qmlCallback.get(), SLOT(checkQt(bool)));
            menuItemIndex++;
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

void ToolBarLayoutManager::menuItemSetVisible(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isVisible)
{
    ::fwGuiQml::container::QmlMenuItemContainer::sptr menuItemContainer =
        ::fwGuiQml::container::QmlMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQmlMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetEnabled(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    ::fwGuiQml::container::QmlMenuItemContainer::sptr menuItemContainer =
        ::fwGuiQml::container::QmlMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQmlMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetChecked(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isChecked)
{
    ::fwGuiQml::container::QmlMenuItemContainer::sptr menuItemContainer =
        ::fwGuiQml::container::QmlMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQmlMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui
