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

#include "fwGuiQt/builder/MenuBarBuilder.hpp"

#include "fwGuiQt/App.hpp"
#include "fwGuiQt/container/QtContainer.hpp"
#include "fwGuiQt/container/QtMenuBarContainer.hpp"

#include <fwGui/registry/macros.hpp>

#include <QMainWindow>
#include <QMenuBar>

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
    m_parent = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("The parent container is not a QtContainer", m_parent);
    QMainWindow* window = qobject_cast<QMainWindow*> ( m_parent->getQtContainer() );
    if ( !window )
    {
        window = qobject_cast<QMainWindow*> ( m_parent->getQtContainer()->parent() );
    }
    SLM_ASSERT("The parent container must be a QMainWindow", window );
    if (window)
    {
        ::fwGuiQt::container::QtMenuBarContainer::sptr menuBarContainer =
            ::fwGuiQt::container::QtMenuBarContainer::New();
        QMenuBar* menuBar = new QMenuBar(0);
        menuBarContainer->setQtMenuBar(menuBar);
        window->setMenuBar( menuBar );
        m_menuBar = menuBarContainer;

        if(!m_backgroundColor.empty())
        {
            const QString style = QString::fromStdString(
                "QMenuBar, QMenu { background-color: " + m_backgroundColor + ";}");
            menuBar->setStyleSheet(qApp->styleSheet() + style);
        }

    }
}

//-----------------------------------------------------------------------------

void MenuBarBuilder::destroyMenuBar()
{
    SLM_ASSERT("The menu is not initialized", m_menuBar);
    SLM_ASSERT("The parent container is not a QtContainer", m_parent);
    QMainWindow* window = qobject_cast<QMainWindow*> ( m_parent->getQtContainer() );
    if ( !window )
    {
        window = qobject_cast<QMainWindow*> ( m_parent->getQtContainer()->parent() );
    }
    SLM_ASSERT("The parent container must be a QMainWindow", window );
    if (window)
    {
        window->setMenuBar( NULL );
    }
    m_menuBar->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui
