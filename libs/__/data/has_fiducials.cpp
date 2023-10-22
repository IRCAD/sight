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
    _signals(signals::GROUP_ADDED, M_GROUP_ADDED);
    _signals(signals::GROUP_REMOVED, M_GROUP_REMOVED);
    _signals(signals::POINT_ADDED, M_POINT_ADDED);
    _signals(signals::POINT_REMOVED, M_POINT_REMOVED);
    _signals(signals::POINT_INSERTED, M_POINT_INSERTED);
    _signals(signals::POINT_MODIFIED, M_POINT_MODIFIED);
    _signals(signals::GROUP_MODIFIED, M_GROUP_MODIFIED);
    _signals(signals::GROUP_RENAMED, M_GROUP_RENAMED);
    _signals(signals::POINT_SELECTED, M_POINT_SELECTED);
    _signals(signals::POINT_DESELECTED, M_POINT_DESELECTED);
}

} // namespace sight::data
