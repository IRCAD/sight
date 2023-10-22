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

#include "data/timeline/base.hpp"

#include <core/com/signal.hxx>

namespace sight::data::timeline
{

//------------------------------------------------------------------------------

base::base()
{
    new_signal<signals::pushed_t>(signals::PUSHED);
    new_signal<signals::removed_t>(signals::REMOVED);
    new_signal<signals::cleared_t>(signals::CLEARED);
}

//------------------------------------------------------------------------------

bool base::operator==(const base& _other) const noexcept
{
    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool base::operator!=(const base& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data::timeline
