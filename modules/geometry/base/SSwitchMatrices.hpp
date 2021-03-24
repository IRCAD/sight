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

#include "modules/geometry/base/config.hpp"

#include <core/base.hpp>
#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/Slot.hpp>

#include <data/Matrix4.hpp>

#include <service/IController.hpp>

namespace sight::module::geometry::base
{

/**
 * @brief   This service switch between several matrices.
 *
 * @section Slots Slots
 * - \b switchMatrix() : Each time the slot switchMatrix() is called the next matrix given in the configuration is
 * copied on the matrix bound
 * to the service
 * - \b switchToMatrix(int index) : switch to the matrix at the given index
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::geometry::base::SSwitchMatrices" autoConnect="true">
            <in group="matrix">
                <key uid="..." />
                <key uid="..." />
            </in>
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [sight::data::Matrix4]: List of two matrices keys to switch.
 * @subsection In-Out In-Out:
 * - \b output [sight::data::Matrix4]: Output matrix.
 *
 */
class MODULE_GEOMETRY_BASE_CLASS_API SSwitchMatrices : public service::IController
{

public:

    SIGHT_DECLARE_SERVICE(SSwitchMatrices, service::IController)

    /**
     * @brief Constructor.
     */
    MODULE_GEOMETRY_BASE_API SSwitchMatrices() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SSwitchMatrices() noexcept
    {
    }

    MODULE_GEOMETRY_BASE_API static const core::com::Slots::SlotKeyType s_SWITCH_SLOT;
    typedef core::com::Slot< void () > SwitchSlotType;

    MODULE_GEOMETRY_BASE_API static const core::com::Slots::SlotKeyType s_SWITCH_TO_SLOT;
    typedef core::com::Slot<void (size_t)> SwitchToSlotType;

protected:

    /// This method is used to configure the service.
    MODULE_GEOMETRY_BASE_API void configuring() override;

    /// This method is used to initialize the service.
    MODULE_GEOMETRY_BASE_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void stopping() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void updating() override;

    /// Switch to next Matrix
    MODULE_GEOMETRY_BASE_API void switchMatrix();

    /// Switch to Matrix "index"
    MODULE_GEOMETRY_BASE_API void switchToMatrix(size_t index);

    MODULE_GEOMETRY_BASE_API KeyConnectionsMap getAutoConnections() const override;

private:

    size_t m_indexOfDesiredMatrix;
};

} //namespace sight::module::geometry::base
