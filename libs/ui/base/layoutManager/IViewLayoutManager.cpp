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

/**
 * @file ui/base/layoutManager/IViewLayoutManager.cpp
 * @brief This file defines the implementation of the base class for managing a layout geometry.
 *
 */

#include "ui/base/layoutManager/IViewLayoutManager.hpp"

namespace sight::ui::base
{

namespace layoutManager
{

//-----------------------------------------------------------------------------

IViewLayoutManager::IViewLayoutManager()
{
}

//-----------------------------------------------------------------------------

IViewLayoutManager::~IViewLayoutManager()
{
}

//-----------------------------------------------------------------------------

void IViewLayoutManager::destroySubViews()
{
    for(ui::base::container::fwContainer::sptr container : m_subViews)
    {
        container->destroyContainer();
    }

    m_subViews.clear();
}

//-----------------------------------------------------------------------------

std::vector<ui::base::container::fwContainer::sptr> IViewLayoutManager::getSubViews()
{
    return this->m_subViews;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager

} // namespace sight::ui::base
