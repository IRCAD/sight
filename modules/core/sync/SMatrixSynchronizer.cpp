/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "SMatrixSynchronizer.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/tools/fwID.hpp>
#include <core/tools/Object.hpp>

#include <data/Image.hpp>
#include <data/Matrix4.hpp>
#include <data/MatrixTL.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/timeline/Buffer.hpp>

#include <service/macros.hpp>

// ----------------------------------------------------------------------------

namespace sight::module::sync
{

const core::com::Slots::SlotKeyType SMatrixSynchronizer::s_UPDATE_MATRIX_SLOT = "updateMatrix";

// ----------------------------------------------------------------------------

SMatrixSynchronizer::SMatrixSynchronizer() noexcept :
    m_lastTimestamp(0)
{
    m_slotUpdateMatrix = core::com::newSlot(&SMatrixSynchronizer::updateMatrix, this);

    core::com::HasSlots::m_slots(s_UPDATE_MATRIX_SLOT, m_slotUpdateMatrix);
    core::com::HasSlots::m_slots.setWorker( m_associatedWorker );
}

// ----------------------------------------------------------------------------

void SMatrixSynchronizer::configuring()
{
}

// ----------------------------------------------------------------------------

void SMatrixSynchronizer::starting()
{
}

// ----------------------------------------------------------------------------

void SMatrixSynchronizer::stopping()
{
}

// ----------------------------------------------------------------------------

void SMatrixSynchronizer::updateMatrix(core::HiResClock::HiResClockType timestamp)
{
    data::Matrix4::sptr matrix3D   = this->getInOut< data::Matrix4 >("matrix");
    data::MatrixTL::csptr matrixTL = this->getInput< data::MatrixTL >("TL");

    if (timestamp > m_lastTimestamp)
    {

        core::HiResClock::HiResClockType currentTimestamp = matrixTL->getNewerTimestamp();
        CSPTR(data::MatrixTL::BufferType) buffer = matrixTL->getClosestBuffer(currentTimestamp);
        SIGHT_ASSERT("Buffer not found with timestamp " << currentTimestamp, buffer);
        m_lastTimestamp = currentTimestamp;

        const float* values = buffer->getElement(0);

        for(unsigned int i = 0; i < 4; ++i)
        {
            for(unsigned int j = 0; j < 4; ++j)
            {
                matrix3D->setCoefficient(i, j, values[i*4+j]);
            }
        }

        auto sig = matrix3D->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

}  // namespace sight::module::sync
