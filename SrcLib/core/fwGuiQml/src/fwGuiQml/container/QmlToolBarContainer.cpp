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

#include "fwGuiQml/container/QmlToolBarContainer.hpp"

#include <QAction>
#include <QMenu>
#include <QToolBar>

namespace fwGuiQml
{
namespace container
{

//-----------------------------------------------------------------------------

QmlToolBarContainer::QmlToolBarContainer(::fwGui::GuiBaseObject::Key key) noexcept
{
}

//-----------------------------------------------------------------------------

QmlToolBarContainer::~QmlToolBarContainer() noexcept
{
}

//-----------------------------------------------------------------------------

void QmlToolBarContainer::clean()
{
    SLM_ASSERT("The Qml toolBar is not yet initialized, cleaning is thus impossible", m_toolBar);

    m_toolBar->clear();
}

//-----------------------------------------------------------------------------

void QmlToolBarContainer::destroyContainer()
{
    SLM_ASSERT("The Qml toolBar is not yet initialized, cleaning is thus impossible", m_toolBar);

    delete m_toolBar;
}

//-----------------------------------------------------------------------------

void QmlToolBarContainer::setQmlToolBar(QToolBar* toolBar)
{
    this->m_toolBar = toolBar;
}

//-----------------------------------------------------------------------------

QToolBar* QmlToolBarContainer::getQmlToolBar()
{
    return this->m_toolBar;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace fwGuiQml
