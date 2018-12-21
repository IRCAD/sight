/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "opImageFilter/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IOperator.hpp>

namespace opImageFilter
{

/**
 * @brief Service flipping an input image along x or y or z axis.
 *
 * @section Slots Slots
 * - flipAxisX(): flip along X axis
 * - flipAxisY(): flip along Y axis
 * - flipAxisZ(): flip along Z axis

 * @section XML XML Configuration
 *
 * @code{.xml}
       <service type="::opITKRegistration::SFlip">
           <in key="source" uid="..." autoConnect="yes" />
           <out key="target" uid="..." />
       </service>
   @endcode
 * @subsection Input Input
 * - \b imageIn [::fwData::Image]: Image to flip.
 * @subsection Output Output:
 * - \b imageOut [::fwData::Image]: New flipped image.
 */
class OPIMAGEFILTER_CLASS_API SFlip : public ::fwServices::IOperator
{
public:

    fwCoreServiceClassDefinitionsMacro( (SFlip)(::fwServices::IOperator) )

    OPIMAGEFILTER_API static const ::fwCom::Slots::SlotKeyType s_FLIP_AXISX_SLOT;
    OPIMAGEFILTER_API static const ::fwCom::Slots::SlotKeyType s_FLIP_AXISY_SLOT;
    OPIMAGEFILTER_API static const ::fwCom::Slots::SlotKeyType s_FLIP_AXISZ_SLOT;

    /// Constructor, does nothing.
    OPIMAGEFILTER_API SFlip();

    /// Destructor, does nothing.
    OPIMAGEFILTER_API ~SFlip();

protected:

    /// Configures the service.
    OPIMAGEFILTER_API virtual void configuring() override;

    /// Does nothing.
    OPIMAGEFILTER_API virtual void starting() override;

    /// Does nothing.
    OPIMAGEFILTER_API virtual void stopping() override;

    /// Apply the flip operator.
    OPIMAGEFILTER_API virtual void updating() override;

    /**
     * @brief Auto connections
     *
     */
    OPIMAGEFILTER_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Slot: flip the first axis
    void flipAxisX();
    /// Slot: flip the second axis
    void flipAxisY();
    /// Slot: flip the third axis
    void flipAxisZ();

    // Store whether to flip or not one of the 3 axis
    std::array<bool, 3> m_flipAxes {{false, false, false}};
};

} // namespace opImageFilter
