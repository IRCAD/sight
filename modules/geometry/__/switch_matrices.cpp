/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

const core::com::slots::key_t switch_matrices::SWITCH_SLOT    = "switchMatrix";
const core::com::slots::key_t switch_matrices::SWITCH_TO_SLOT = "switchToMatrix";

// ----------------------------------------------------------------------------

switch_matrices::switch_matrices() noexcept
{
    new_slot(SWITCH_SLOT, &switch_matrices::switchMatrix, this);
    new_slot(SWITCH_TO_SLOT, &switch_matrices::switchToMatrix, this);
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
    return {{s_MATRIX_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE}};
}

// ----------------------------------------------------------------------------

void switch_matrices::updating()
{
    auto matrix = m_output.lock();

    auto desired_matrix = m_matrix[m_indexOfDesiredMatrix].lock();
    matrix->shallow_copy(desired_matrix.get_shared());

    auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

// ----------------------------------------------------------------------------

void switch_matrices::switchMatrix()
{
    ++m_indexOfDesiredMatrix;
    if(m_indexOfDesiredMatrix >= m_matrix.size())
    {
        m_indexOfDesiredMatrix = 0;
    }

    this->updating();
}

// ----------------------------------------------------------------------------

void switch_matrices::switchToMatrix(int _index)
{
    if(_index >= 0 && static_cast<std::size_t>(_index) < m_matrix.size())
    {
        m_indexOfDesiredMatrix = static_cast<std::size_t>(_index);
    }
    else
    {
        SIGHT_WARN("Desired index don't exists, switch to first matrix");
        m_indexOfDesiredMatrix = 0;
    }

    this->updating();
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry
