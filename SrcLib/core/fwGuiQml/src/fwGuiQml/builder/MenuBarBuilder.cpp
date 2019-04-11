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

#include "fwGuiQml/builder/MenuBarBuilder.hpp"

#include "fwGuiQml/container/QmlContainer.hpp"
#include "fwGuiQml/container/QmlMenuBarContainer.hpp"

#include <fwGui/registry/macros.hpp>

fwGuiRegisterMacro(::fwGui::builder::MenuBarBuilder, ::fwGui::builder::IMenuBarBuilder::REGISTRY_KEY);

namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

MenuBarBuilder::MenuBarBuilder(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

MenuBarBuilder::~MenuBarBuilder()
{
}

//-----------------------------------------------------------------------------

void MenuBarBuilder::createMenuBar( ::fwGui::container::fwContainer::sptr parent )
{
    m_parent = ::fwGuiQml::container::QmlContainer::dynamicCast(parent);
    SLM_ASSERT("The parent container is not a QmlContainer", m_parent);
//    QMainWindow* window = qobject_cast<QMainWindow*> ( m_parent->getQmlContainer() );
//    if ( !window )
//    {
//        window = qobject_cast<QMainWindow*> ( m_parent->getQmlContainer()->parent() );
//    }
//    SLM_ASSERT("The parent container must be a QMainWindow", window );
//    if (window)
//    {
//        ::fwGuiQml::container::QmlMenuBarContainer::sptr menuBarContainer =
//            ::fwGuiQml::container::QmlMenuBarContainer::New();
//        QMenuBar* menuBar = new QMenuBar(0);
//        menuBarContainer->setQmlMenuBar(menuBar);
//        window->setMenuBar( menuBar );
//        m_menuBar = menuBarContainer;
//    }
}

//-----------------------------------------------------------------------------

void MenuBarBuilder::destroyMenuBar()
{
    SLM_ASSERT("The menu is not initialized", m_menuBar);
    SLM_ASSERT("The parent container is not a QmlContainer", m_parent);
//    QMainWindow* window = qobject_cast<QMainWindow*> ( m_parent->getQmlContainer() );
//    if ( !window )
//    {
//        window = qobject_cast<QMainWindow*> ( m_parent->getQmlContainer()->parent() );
//    }
//    SLM_ASSERT("The parent container must be a QMainWindow", window );
//    if (window)
//    {
//        window->setMenuBar( NULL );
//    }
    m_menuBar->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui
