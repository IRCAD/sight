/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "data/TimeLine.hpp"

#include <core/com/signal.hxx>

namespace sight::data
{

const core::com::signals::key_t TimeLine::OBJECT_PUSHED_SIG  = "objectPushed";
const core::com::signals::key_t TimeLine::OBJECT_REMOVED_SIG = "objectRemoved";
const core::com::signals::key_t TimeLine::CLEARED_SIG        = "objectCleared";

//------------------------------------------------------------------------------

TimeLine::TimeLine()
{
    new_signal<ObjectPushedSignalType>(OBJECT_PUSHED_SIG);
    new_signal<ObjectRemovedSignalType>(OBJECT_REMOVED_SIG);
    new_signal<ObjectClearedSignalType>(CLEARED_SIG);
}

//------------------------------------------------------------------------------

bool TimeLine::operator==(const TimeLine& other) const noexcept
{
    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool TimeLine::operator!=(const TimeLine& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
