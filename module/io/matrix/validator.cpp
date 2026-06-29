/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include "module/io/matrix/validator.hpp"

#include <geometry/data/matrix4.hpp>

namespace sight::module::io::matrix
{

//-----------------------------------------------------------------------------

validator::validator() :
    notifier(has_signals::signals())
{
    new_signal<signals::void_t>(signals::VALID);
    new_signal<signals::void_t>(signals::INVALID);
}

//-----------------------------------------------------------------------------

void validator::starting()
{
}

//-----------------------------------------------------------------------------

void validator::configuring()
{
    sight::service::controller::configuring();
}

//-----------------------------------------------------------------------------

void validator::stopping()
{
}

//-----------------------------------------------------------------------------

void validator::updating()
{
    const auto matrix = m_matrix.lock();

    if(!matrix)
    {
        this->async_emit(signals::INVALID);
        this->notify(sight::service::notification::type::failure, "matrix is empty");
        return;
    }

    const auto is_ortho = sight::geometry::data::is_orthogonal(*matrix);

    if(!is_ortho)
    {
        this->notify(sight::service::notification::type::failure, "matrix is not orthogonal");
        this->async_emit(signals::INVALID);
        return;
    }

    const auto is_homogeneous = sight::geometry::data::is_homogeneous(*matrix);

    if(!is_homogeneous)
    {
        this->notify(sight::service::notification::type::failure, "matrix is not homogeneous");
        this->async_emit(signals::INVALID);
        return;
    }

    this->async_emit(signals::VALID);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::matrix
