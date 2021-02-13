/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#pragma once

#include "modules/sync/config.hpp"

#include <core/base.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <data/MatrixTL.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <service/ISynchronizer.hpp>

namespace sight::modules::sync
{
/**
 * @brief   SMatrixSynchronizer reads the content of a MatrixTL into a TransformationMatrix3D.
 */
class MODULE_SYNC_CLASS_API SMatrixSynchronizer : public service::ISynchronizer
{

public:

    fwCoreServiceMacro(SMatrixSynchronizer, service::ISynchronizer)

    /**
     * @brief Constructor.
     */
    MODULE_SYNC_API SMatrixSynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_SYNC_API virtual ~SMatrixSynchronizer() noexcept
    {
    }

    /// Slot definition
    MODULE_SYNC_API static const core::com::Slots::SlotKeyType s_UPDATE_MATRIX_SLOT;
    typedef core::com::Slot<void (core::HiResClock::HiResClockType)> UpdateMatrixSlotType;

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @code{.xml}
       <service type="::sight::modules::sync::SMatrixSynchronizer">
            <inout>Matrix</inout>
            <in>MatrixTL</in>
       </service>
     * @endcode
     *
     * @subsection In-Out In-Out
     * - \b TL [sight::data::MatrixTL]:  Defines the uid of the MatrixTL containing the matrices.
     *
     */

    MODULE_SYNC_API void configuring() override;

    /// This method is used to initialize the service.
    MODULE_SYNC_API void starting() override;

    /// Does nothing.
    MODULE_SYNC_API void stopping() override;

    /// Does nothing.
    MODULE_SYNC_API void updating() override
    {
    }

private:

    /// slot to update the TransformationMatrix3D with the timeline buffer
    UpdateMatrixSlotType::sptr m_slotUpdateMatrix;
    /// Last timestamp
    core::HiResClock::HiResClockType m_lastTimestamp;

    /// This function fills the TransformationMatrix3D with the current buffer content of the MatrixTL
    void updateMatrix(core::HiResClock::HiResClockType timestamp);

};

} //namespace sight::modules::sync
