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

#include "modules/geometry/__/SSwitchMatrices.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <geometry/data/Matrix4.hpp>

namespace sight::module::geometry
{

const core::com::slots::key_t SSwitchMatrices::SWITCH_SLOT    = "switchMatrix";
const core::com::slots::key_t SSwitchMatrices::SWITCH_TO_SLOT = "switchToMatrix";

// ----------------------------------------------------------------------------

SSwitchMatrices::SSwitchMatrices() noexcept
{
    new_slot(SWITCH_SLOT, &SSwitchMatrices::switchMatrix, this);
    new_slot(SWITCH_TO_SLOT, &SSwitchMatrices::switchToMatrix, this);
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::configuring()
{
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::starting()
{
    this->updating();
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::stopping()
{
}

// ----------------------------------------------------------------------------

service::connections_t SSwitchMatrices::getAutoConnections() const
{
    return {{s_MATRIX_INPUT, data::Object::MODIFIED_SIG, service::slots::UPDATE}};
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::updating()
{
    auto matrix = m_output.lock();

    auto desiredMatrix = m_matrix[m_indexOfDesiredMatrix].lock();
    matrix->shallow_copy(desiredMatrix.get_shared());

    auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::switchMatrix()
{
    ++m_indexOfDesiredMatrix;
    if(m_indexOfDesiredMatrix >= m_matrix.size())
    {
        m_indexOfDesiredMatrix = 0;
    }

    this->updating();
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::switchToMatrix(int index)
{
    if(index >= 0 && static_cast<std::size_t>(index) < m_matrix.size())
    {
        m_indexOfDesiredMatrix = static_cast<std::size_t>(index);
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
