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

#include "ui/qt/container/QtToolBarContainer.hpp"

#include <QAction>
#include <QMenu>
#include <QToolBar>

namespace sight::ui::qt
{
namespace container
{

//-----------------------------------------------------------------------------

QtToolBarContainer::QtToolBarContainer(ui::base::GuiBaseObject::Key key) noexcept
{
}

//-----------------------------------------------------------------------------

QtToolBarContainer::~QtToolBarContainer() noexcept
{
}

//-----------------------------------------------------------------------------

void QtToolBarContainer::clean()
{
    SIGHT_ASSERT("The Qt toolBar is not yet initialized, cleaning is thus impossible", m_toolBar);

    m_toolBar->clear();
}

//-----------------------------------------------------------------------------

void QtToolBarContainer::destroyContainer()
{
    SIGHT_ASSERT("The Qt toolBar is not yet initialized, cleaning is thus impossible", m_toolBar);

    delete m_toolBar;
}

//-----------------------------------------------------------------------------

void QtToolBarContainer::setQtToolBar(QToolBar* toolBar)
{
    this->m_toolBar = toolBar;
}

//-----------------------------------------------------------------------------

QToolBar* QtToolBarContainer::getQtToolBar()
{
    return this->m_toolBar;
}

//-----------------------------------------------------------------------------

} // namespace container
} // namespace sight::ui::qt
