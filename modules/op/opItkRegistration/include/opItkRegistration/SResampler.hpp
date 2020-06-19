/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "opItkRegistration/config.hpp"

#include <fwServices/IOperator.hpp>

namespace opItkRegistration
{

/**
 * @brief Service applying a transform to an image and outputing the resampled image.
 *
 * @section Signals Signals
 * - \b computed() : Signal sent after resampling.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::opITK::SPropagator">
       <in key="imageIn" uid="..." autoConnect="yes" />
       <in key="transform" uid="..." />
       <in key="target" uid="..." />
       <inout key="imageOut" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b imageIn [::fwData::Image]: Image to resample.
 * - \b transform [::fwData::TransformationMatrix3D]: Transform to apply.
 * - \b target [::fwData::Image] (optional): target image defining the size, spacing and origin of the output.
 * @subsection In-Out In-Out
 * - \b imageOut [::fwData::Image]: New resampled image.
 */
class OPITKREGISTRATION_CLASS_API SResampler : public ::fwServices::IOperator
{
public:

    fwCoreServiceMacro(SResampler, ::fwServices::IOperator);

    /// Constructor, does nothing.
    OPITKREGISTRATION_API SResampler();

    /// Destructor, does nothing.
    OPITKREGISTRATION_API ~SResampler();

protected:

    /// Does nothing.
    OPITKREGISTRATION_API virtual void configuring() override;

    /// Does nothing.
    OPITKREGISTRATION_API virtual void starting() override;

    /// Does nothing.
    OPITKREGISTRATION_API virtual void stopping() override;

    /// Apply the transform and resample.
    OPITKREGISTRATION_API virtual void updating() override;

    /**
     * @brief Auto connections
     *
     * - Update service when the image or its buffer is modified.
     * - Update service when the transform matrix is modified.
     * - Update service when the target image is modified.
     */
    OPITKREGISTRATION_API virtual KeyConnectionsMap getAutoConnections() const override;

};

} // namespace opItkRegistration
