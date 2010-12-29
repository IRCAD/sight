/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QMenu>
#include <QToolButton>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/ActionCallback.hpp"
#include "fwGuiQt/container/QtToolBarContainer.hpp"
#include "fwGuiQt/container/QtMenuContainer.hpp"
#include "fwGuiQt/container/QtMenuItemContainer.hpp"
#include "fwGuiQt/layoutManager/ToolBarLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IToolBarLayoutManager,
        ::fwGui::layoutManager::ToolBarLayoutManager,
         ::fwGui::layoutManager::IToolBarLayoutManager::RegistryKeyType,
          ::fwGui::layoutManager::IToolBarLayoutManager::REGISTRY_KEY );

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

ToolBarLayoutManager::ToolBarLayoutManager()
{}

//-----------------------------------------------------------------------------

ToolBarLayoutManager::~ToolBarLayoutManager()
{}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::createLayout( ::fwGui::container::fwToolBar::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiQt::container::QtToolBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwToolBar to QtToolBarContainer failed", m_parent);

    QToolBar* toolBar = m_parent->getQtToolBar();

    QActionGroup * actionGroup = 0;
    unsigned int menuItemIndex = 0;
    BOOST_FOREACH ( ::fwGui::layoutManager::IToolBarLayoutManager::ActionInfo actionInfo, m_actionInfo)
    {
        if (actionInfo.m_isSeparator)
        {
            QWidget * widget = new QWidget();
            if (actionInfo.m_size > 0)
            {
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
            QWidget* spacer = new QWidget();
            spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            toolBar->addWidget(spacer);
            actionGroup = 0;
        }
        else if (actionInfo.m_isMenu)
        {
            ::fwGuiQt::container::QtMenuContainer::NewSptr menu;
            QMenu* qtMenu = new QMenu(toolBar);
            menu->setQtMenu(qtMenu);

            QToolButton * toolButton = new QToolButton();
            toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            toolButton->setMenu(qtMenu);
            toolButton->setPopupMode(QToolButton::InstantPopup);
            toolButton->setText(QString::fromStdString(actionInfo.m_name));
            if (!actionInfo.m_icon.empty())
            {
                QIcon icon(QString::fromStdString(actionInfo.m_icon));
                toolButton->setIcon(icon);
                toolButton->setToolTip(QString::fromStdString(actionInfo.m_name));
                toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            }
            toolBar->addWidget(toolButton);
            m_menus.push_back(menu);
        }
        else
        {
            ::fwGuiQt::container::QtMenuItemContainer::NewSptr menuItem;
            QAction *action;
            if (!actionInfo.m_icon.empty())
            {
                QIcon icon(QString::fromStdString(actionInfo.m_icon));
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

            menuItem->setQtMenuItem(action);

            m_menuItems.push_back(menuItem);
            OSLM_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
            ::fwGui::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

            ::fwGuiQt::ActionCallback::sptr qtCallback = ::fwGuiQt::ActionCallback::dynamicCast(callback);
            SLM_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", qtCallback);

            QObject::connect( action, SIGNAL(triggered(bool)), qtCallback.get(), SLOT(executeQt(bool)));
            QObject::connect( action, SIGNAL(toggled(bool)), qtCallback.get(), SLOT(checkQt(bool)));
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
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer = ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction *action = menuItemContainer->getQtMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetEnabled(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer = ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction *action = menuItemContainer->getQtMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetChecked(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isChecked)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer = ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction *action = menuItemContainer->getQtMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



