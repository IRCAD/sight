/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/geometry/base/SSwitchMatrices.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <geometry/data/Matrix4.hpp>

namespace sight::module::geometry::base
{

const core::com::Slots::SlotKeyType SSwitchMatrices::s_SWITCH_SLOT    = "switchMatrix";
const core::com::Slots::SlotKeyType SSwitchMatrices::s_SWITCH_TO_SLOT = "switchToMatrix";

// ----------------------------------------------------------------------------

SSwitchMatrices::SSwitchMatrices() noexcept
{
    newSlot(s_SWITCH_SLOT, &SSwitchMatrices::switchMatrix, this);
    newSlot(s_SWITCH_TO_SLOT, &SSwitchMatrices::switchToMatrix, this);
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

service::IService::KeyConnectionsMap SSwitchMatrices::getAutoConnections() const
{
    return {{s_MATRIX_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT}};
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::updating()
{
    auto matrix = m_output.lock();

    auto desiredMatrix = m_matrix[m_indexOfDesiredMatrix].lock();
    matrix->shallowCopy(desiredMatrix.get_shared());

    auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
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

void SSwitchMatrices::switchToMatrix(std::size_t index)
{
    if(index < m_matrix.size())
    {
        m_indexOfDesiredMatrix = index;
    }
    else
    {
        SIGHT_WARN("Desired index don't exists, switch to first matrix");
        m_indexOfDesiredMatrix = 0;
    }

    this->updating();
}

// ----------------------------------------------------------------------------

} // namespace sight::module::geometry::base
