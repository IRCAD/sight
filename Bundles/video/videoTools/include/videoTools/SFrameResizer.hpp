/************************************************************************
 *
 * Copyright (C) 2015-2018 IRCAD France
 * Copyright (C) 2015-2018 IHU Strasbourg
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

#include "videoTools/config.hpp"

#include <arData/FrameTL.hpp>

#include <fwServices/IOperator.hpp>

namespace videoTools
{

/**
 * @brief   Operator that resizes buffer coming from an input arData::FrameTL and pushes them into an output timeline.
 *          This service uses OpenCV to perform the resize processing.
 * @deprecated This service will be removed in 19.0.
 */
class VIDEOTOOLS_CLASS_API SFrameResizer : public ::fwServices::IOperator
{

public:

    fwCoreServiceClassDefinitionsMacro( (SFrameResizer)(::fwServices::IOperator) );

    /// Constructor.
    VIDEOTOOLS_API SFrameResizer() noexcept;

    /// Destructor. Does nothing
    VIDEOTOOLS_API virtual ~SFrameResizer() noexcept;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SFrameResizerInstance" impl="::videoTools::SFrameResizer" type="::fwServices::IOperator">
             <input>frameTL</input>
             <output>resizedFrameTL</output>
             <ratio x="50" y="50" />
             <interpolation>linear</interpolation>
        </service>
       @endcode
     * - \b input : key of the timeline used as input.
     * - \b output : key of the timeline used as input.
     * - \b ratio : ratio (x,y) of the resize operation as a percentage.
     * - \b interpolation(optional) : method to use amongst : nearest(default), linear, area, cubic, lanczos4.
     */
    VIDEOTOOLS_API virtual void configuring() override;

    /// Grab timelines.
    VIDEOTOOLS_API virtual void starting() override;

    /// Stopping method.
    VIDEOTOOLS_API virtual void stopping() override;

    /// Called when a new buffer is pushed inside the input timeline. A new buffer containing the
    /// resied image is pushed inside the output timeline.
    VIDEOTOOLS_API virtual void updating() override;

private:

    /// Frame timeline key
    std::string m_inputTLKey;

    /// Frame timeline key
    std::string m_outputTLKey;

    /// Frame timeline
    ::arData::FrameTL::sptr m_inputTL;

    /// Frame timeline
    ::arData::FrameTL::sptr m_outputTL;

    /// Resize ratio
    float m_ratio[2];

    /// Interpolation method - as defined in OpenCV
    int m_interpolationMethod;

    /// Connections
    ::fwCom::helper::SigSlotConnection m_connections;
};

} // videoTools
