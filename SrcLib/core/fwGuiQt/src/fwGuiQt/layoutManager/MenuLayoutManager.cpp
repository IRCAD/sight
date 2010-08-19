/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QAction>
#include <QActionGroup>
#include <QMenu>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/ActionCallback.hpp"
#include "fwGuiQt/container/QtMenuContainer.hpp"
#include "fwGuiQt/container/QtMenuItemContainer.hpp"
#include "fwGuiQt/layoutManager/MenuLayoutManager.hpp"


REGISTER_BINDING( ::fwGui::layoutManager::IMenuLayoutManager,
        ::fwGui::layoutManager::MenuLayoutManager,
         ::fwGui::layoutManager::IMenuLayoutManager::RegistryKeyType,
          ::fwGui::layoutManager::IMenuLayoutManager::REGISTRY_KEY );

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

MenuLayoutManager::MenuLayoutManager()
{}

//-----------------------------------------------------------------------------

MenuLayoutManager::~MenuLayoutManager()
{}

//-----------------------------------------------------------------------------

void MenuLayoutManager::createLayout( ::fwGui::fwMenu::sptr parent )
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiQt::container::QtMenuContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwMenu to QtMenuContainer failed", m_parent);

    QMenu* menu = m_parent->getQtMenu();

    QActionGroup * actionGroup = 0;
    unsigned int menuItemIndex = 0;
    BOOST_FOREACH ( ::fwGui::layoutManager::IMenuLayoutManager::ActionInfo actionInfo, m_actionInfo)
    {
        ::fwGuiQt::container::QtMenuItemContainer::NewSptr menuItem;

        QAction *action = menu->addAction( QString::fromStdString(actionInfo.m_name) );

        action->setSeparator(actionInfo.m_isSeparator);


        if (actionInfo.m_type == ::fwGui::layoutManager::IMenuLayoutManager::QUIT)
        {
            action->setMenuRole(QAction::QuitRole);
        }
        else if (actionInfo.m_type == ::fwGui::layoutManager::IMenuLayoutManager::ABOUT)
        {
            action->setMenuRole(QAction::AboutRole);
        }
        else
        {
            action->setMenuRole(QAction::NoRole);
        }

        action->setCheckable(actionInfo.m_isCheckable || actionInfo.m_isRadio);
        action->setChecked(actionInfo.m_isChecked);
        action->setEnabled(actionInfo.m_isEnabled);

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

        menuItem->setQtMenuItem(action);

        if(!actionInfo.m_isSeparator)
        {
            m_menuItems.push_back(menuItem);
            OSLM_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
            ::fwGui::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

            ::fwGuiQt::ActionCallback::sptr qtCallback = ::fwGuiQt::ActionCallback::dynamicCast(callback);
            SLM_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", qtCallback);

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
    QMenu* menu = m_parent->getQtMenu();

    m_menuItems.clear();
    menu->clear();
}

//-----------------------------------------------------------------------------


void MenuLayoutManager::menuItemSetVisible(::fwGui::fwMenuItem::sptr fwMenuItem, bool isVisible)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer = ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction *action = menuItemContainer->getQtMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::menuItemSetEnabled(::fwGui::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer = ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction *action = menuItemContainer->getQtMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void MenuLayoutManager::menuItemSetChecked(::fwGui::fwMenuItem::sptr fwMenuItem, bool isChecked)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer = ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction *action = menuItemContainer->getQtMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



