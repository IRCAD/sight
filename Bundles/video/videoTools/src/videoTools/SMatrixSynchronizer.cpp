/************************************************************************
 *
 * Copyright (C) 2014-2016 IRCAD France
 * Copyright (C) 2014-2016 IHU Strasbourg
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

#include "videoTools/SMatrixSynchronizer.hpp"

#include <arData/MatrixTL.hpp>
#include <arData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Object.hpp>
#include <fwTools/fwID.hpp>

fwServicesRegisterMacro(::arServices::ISynchronizer, ::videoTools::SMatrixSynchronizer,
                        ::fwData::TransformationMatrix3D);

// ----------------------------------------------------------------------------

namespace videoTools
{

const ::fwCom::Slots::SlotKeyType SMatrixSynchronizer::s_UPDATE_MATRIX_SLOT = "updateMatrix";

// ----------------------------------------------------------------------------

SMatrixSynchronizer::SMatrixSynchronizer() noexcept : m_lastTimestamp(0)
{
    m_slotUpdateMatrix = ::fwCom::newSlot(&SMatrixSynchronizer::updateMatrix, this);

    ::fwCom::HasSlots::m_slots(s_UPDATE_MATRIX_SLOT, m_slotUpdateMatrix);
    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
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

void SMatrixSynchronizer::updateMatrix(::fwCore::HiResClock::HiResClockType timestamp)
{
    ::fwData::TransformationMatrix3D::sptr matrix3D = this->getInOut< ::fwData::TransformationMatrix3D >("matrix");
    ::arData::MatrixTL::csptr matrixTL              = this->getInput< ::arData::MatrixTL >("TL");

    if (timestamp > m_lastTimestamp)
    {

        ::fwCore::HiResClock::HiResClockType currentTimestamp = matrixTL->getNewerTimestamp();
        CSPTR(::arData::MatrixTL::BufferType) buffer          = matrixTL->getClosestBuffer(currentTimestamp);
        OSLM_ASSERT("Buffer not found with timestamp " << currentTimestamp, buffer);
        m_lastTimestamp = currentTimestamp;

        const float* values = buffer->getElement(0);

        for(unsigned int i = 0; i < 4; ++i)
        {
            for(unsigned int j = 0; j < 4; ++j)
            {
                matrix3D->setCoefficient(i,j,values[i*4+j]);
            }
        }

        auto sig = matrix3D->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------


}  // namespace videoTools
