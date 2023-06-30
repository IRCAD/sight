/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "IHasFiducials.hpp"

namespace sight::data
{

IHasFiducials::IHasFiducials(core::com::Signals& _signals)
{
    _signals(signals::GROUP_ADDED, m_groupAdded);
    _signals(signals::GROUP_REMOVED, m_groupRemoved);
    _signals(signals::POINT_ADDED, m_pointAdded);
    _signals(signals::POINT_REMOVED, m_pointRemoved);
    _signals(signals::POINT_INSERTED, m_pointInserted);
    _signals(signals::POINT_MODIFIED, m_pointModified);
    _signals(signals::GROUP_MODIFIED, m_groupModified);
    _signals(signals::GROUP_RENAMED, m_groupRenamed);
    _signals(signals::POINT_SELECTED, m_pointSelected);
    _signals(signals::POINT_DESELECTED, m_pointDeselected);
}

} // namespace sight::data
