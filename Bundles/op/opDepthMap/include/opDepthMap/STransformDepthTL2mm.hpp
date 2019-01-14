/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "opDepthMap/config.hpp"

#include <fwServices/IOperator.hpp>

namespace opDepthMap
{

/**
 * @brief This service applies a scale on a depth map buffer extracted from a timeline.
 *
 * The scale is given by the calibration (::arData::CameraSeries). The scaled map is pushed in the output timeline with
 * the same timestamp as the input map.
 *
 * @section Signals Signals
 * - \b computed() : emitted when the compute method is done
 *
 * @section Slots Slots
 * - \b compute(double) : apply the scale on the depth map
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::opDepthMap::STransformDepthTL2mm">
       <in key="cameraSeries" uid="..." />
       <in key="originDepthTL" uid="..." />
       <inout key="scaledDepthTL" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b cameraSeries [::arData::CameraSeries]: camera series containing the scale of the depth map.
 * - \b originDepthTL [::arData::FrameTL]: timeline containing the original depth maps.
 * @subsection In-Out In-Out
 * - \b scaledDepthTL [::arData::FrameTL]: timeline containing the scaled depth maps.
 */
class OPDEPTHMAP_CLASS_API STransformDepthTL2mm : public ::fwServices::IOperator
{
public:
    fwCoreServiceClassDefinitionsMacro((STransformDepthTL2mm)(::fwServices::IOperator))

    /**
     * @brief STransformDepthTL2mm constructor.
     */
    OPDEPTHMAP_API STransformDepthTL2mm();

    /**
     * @brief STransformDepthTL2mm destructor.
     */
    OPDEPTHMAP_API virtual ~STransformDepthTL2mm();

protected:

    /// Do nothing
    OPDEPTHMAP_API virtual void starting() override;

    /// Do nothing
    OPDEPTHMAP_API virtual void stopping() override;

    /// Do nothing
    OPDEPTHMAP_API virtual void configuring() override;

    /// Do nothing
    OPDEPTHMAP_API virtual void updating() override;

    /// Defines the auto-connection between the depth timeline and the 'compute' slot
    OPDEPTHMAP_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:
    /// Apply the scale on the depth map
    void compute(::fwCore::HiResClock::HiResClockType timestamp);

    /// timestamp of the last process
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;
};

} // namespace opDepthMap
