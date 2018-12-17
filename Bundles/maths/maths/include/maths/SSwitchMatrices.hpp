/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
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

#pragma once

#include "maths/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IController.hpp>

namespace maths
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
        <service uid="..." type="::maths::SSwitchMatrices" autoConnect="yes">
            <in group="matrix">
                <key uid="..." />
                <key uid="..." />
            </in>
            <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [::fwData::TransformationMatrix3D]: List of two matrices keys to switch.
 * @subsection In-Out In-Out:
 * - \b output [::fwData::TransformationMatrix3D]: Output matrix.
 *
 */
class MATHS_CLASS_API SSwitchMatrices : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro((SSwitchMatrices)(::fwServices::IController))

    /**
     * @brief Constructor.
     */
    MATHS_API SSwitchMatrices() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SSwitchMatrices() noexcept
    {
    }

    MATHS_API static const ::fwCom::Slots::SlotKeyType s_SWITCH_SLOT;
    typedef ::fwCom::Slot< void () > SwitchSlotType;

    MATHS_API static const ::fwCom::Slots::SlotKeyType s_SWITCH_TO_SLOT;
    typedef ::fwCom::Slot<void (size_t)> SwitchToSlotType;

protected:

    /// This method is used to configure the service.
    MATHS_API void configuring() override;

    /// This method is used to initialize the service.
    MATHS_API void starting() override;

    /// Does nothing.
    MATHS_API void stopping() override;

    /// Does nothing.
    MATHS_API void updating() override;

    /// Switch to next Matrix
    MATHS_API void switchMatrix();

    /// Switch to Matrix "index"
    MATHS_API void switchToMatrix(size_t index);

    MATHS_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    size_t m_indexOfDesiredMatrix;
};

} //namespace maths
