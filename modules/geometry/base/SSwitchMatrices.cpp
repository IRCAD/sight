/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>

#include <geometry/data/Matrix4.hpp>

#include <service/macros.hpp>

namespace sight::module::geometry::base
{

const core::com::Slots::SlotKeyType SSwitchMatrices::s_SWITCH_SLOT    = "switchMatrix";
const core::com::Slots::SlotKeyType SSwitchMatrices::s_SWITCH_TO_SLOT = "switchToMatrix";

const service::IService::KeyType s_MATRIX_OUTPUT = "output";
const service::IService::KeyType s_MATRIX_INPUT  = "matrix";

// ----------------------------------------------------------------------------

SSwitchMatrices::SSwitchMatrices() noexcept :
    m_indexOfDesiredMatrix(0)
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
    service::IService::KeyConnectionsMap connections;
    connections.push(s_MATRIX_INPUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::updating()
{
    data::Matrix4::sptr matrix = this->getInOut<data::Matrix4>(s_MATRIX_OUTPUT);

    auto desiredMatrix = this->getInput<data::Matrix4>(s_MATRIX_INPUT, m_indexOfDesiredMatrix);

    matrix->shallowCopy(desiredMatrix);

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
    if(m_indexOfDesiredMatrix >= this->getKeyGroupSize(s_MATRIX_INPUT))
    {
        m_indexOfDesiredMatrix = 0;
    }

    this->updating();
}

// ----------------------------------------------------------------------------

void SSwitchMatrices::switchToMatrix(size_t index)
{
    if(index < this->getKeyGroupSize(s_MATRIX_INPUT))
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
