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

#include "has_fiducials.hpp"

namespace sight::data
{

has_fiducials::has_fiducials(core::com::signals& _signals)
{
    _signals(signals::GROUP_ADDED, m_group_added);
    _signals(signals::GROUP_REMOVED, m_group_removed);
    _signals(signals::POINT_ADDED, m_point_added);
    _signals(signals::POINT_REMOVED, m_point_removed);
    _signals(signals::POINT_INSERTED, m_point_inserted);
    _signals(signals::POINT_MODIFIED, m_point_modified);
    _signals(signals::GROUP_MODIFIED, m_group_modified);
    _signals(signals::GROUP_RENAMED, m_group_renamed);
    _signals(signals::POINT_SELECTED, m_point_selected);
    _signals(signals::POINT_DESELECTED, m_point_deselected);
}

} // namespace sight::data
