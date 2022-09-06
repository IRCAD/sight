/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/qt/container/QtMenuItemContainer.hpp"

#include <QAction>

namespace sight::ui::qt::container
{

//-----------------------------------------------------------------------------

QtMenuItemContainer::QtMenuItemContainer(ui::base::GuiBaseObject::Key /*key*/) noexcept :
    m_menuItem(nullptr)
{
}

//-----------------------------------------------------------------------------

QtMenuItemContainer::~QtMenuItemContainer() noexcept =
    default;

//-----------------------------------------------------------------------------

void QtMenuItemContainer::clean()
{
    SIGHT_ASSERT("The qtMenuItem is not yet initialized, cleaning is thus impossible", m_menuItem);
}

//-----------------------------------------------------------------------------

void QtMenuItemContainer::destroyContainer()
{
    SIGHT_ASSERT("The qtMenuItem is not yet initialized, cleaning is thus impossible", m_menuItem);
}

//-----------------------------------------------------------------------------

void QtMenuItemContainer::setQtMenuItem(QAction* menuItem)
{
    this->m_menuItem = menuItem;
}

//-----------------------------------------------------------------------------

QAction* QtMenuItemContainer::getQtMenuItem()
{
    return this->m_menuItem;
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::container
