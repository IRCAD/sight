/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "modules/geometry/__/switch_matrices.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/matrix4.hpp>

namespace sight::module::geometry
{

// ----------------------------------------------------------------------------

switch_matrices::switch_matrices() noexcept
{
    new_slot(slots::SWITCH, &switch_matrices::switch_matrix, this);
}

// ----------------------------------------------------------------------------

void switch_matrices::configuring()
{
}

// ----------------------------------------------------------------------------

void switch_matrices::starting()
{
    this->updating();
}

// ----------------------------------------------------------------------------

void switch_matrices::stopping()
{
}

// ----------------------------------------------------------------------------

service::connections_t switch_matrices::auto_connections() const
{
    return {
        {m_current_index, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_matrix, data::object::MODIFIED_SIG, service::slots::UPDATE}
    };
}

// ----------------------------------------------------------------------------

void switch_matrices::updating()
{
    auto matrix = m_output.lock();

    auto current_index = static_cast<std::size_t>(*m_current_index);
    SIGHT_ASSERT("Requested matrix index doesn't exists.", current_index < m_matrix.size());

    auto desired_matrix = m_matrix[current_index].lock();
    matrix->shallow_copy(desired_matrix.get_shared());
    matrix->async_emit(this, data::object::MODIFIED_SIG);
}

// ----------------------------------------------------------------------------

void switch_matrices::switch_matrix()
{
    auto current_index = m_current_index.lock();
    *current_index = *current_index + 1;
    if(static_cast<std::size_t>(*current_index) >= m_matrix.size())
    {
        *current_index = 0;
    }

    this->updating();
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry
