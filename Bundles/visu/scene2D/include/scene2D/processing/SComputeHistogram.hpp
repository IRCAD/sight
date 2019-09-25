/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "scene2D/config.hpp"

#include <fwData/Histogram.hpp>
#include <fwData/Image.hpp>

#include <fwServices/IController.hpp>

namespace scene2D
{

namespace processing
{

/**
 * @brief The SComputeHistogram service computes the histogram of the image.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="computeHistogram" type="::scene2D::processing::SComputeHistogram" autoConnect="yes">
    <inout key="histogram" uid="..." />
    <in key="image" uid="..." />
    <binsWidth>5.0</binsWidth>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b histogram [::fwData::Histogram]: histogram to compute.
 *
 * @subsection In In
 * - \b image [::fwData::Image]: image used to compute histogram.
 *
 * @subsection Configuration Configuration:
 * - \b binsWidth (mandatory): desired bins width used to classified pixel.
 */
class SCENE2D_CLASS_API SComputeHistogram : public ::fwServices::IController
{

public:

    fwCoreServiceMacro(SComputeHistogram, ::fwServices::IController);

    /// Constructor. Does nothing.
    SCENE2D_API SComputeHistogram() noexcept;

    /// Destructor. Does nothing.
    SCENE2D_API virtual ~SComputeHistogram() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    SCENE2D_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    SCENE2D_API void configuring() override;

    /// Starts the service. Calls updating().
    SCENE2D_API void starting() override;

    /// Computes the histogram and notifies Histogram::s_MODIFIED_SIG.
    SCENE2D_API void updating() override;

    /// Swaps the service. Calls updating().
    SCENE2D_API void swapping() override;

    /// Stops the service. Does nothing.
    SCENE2D_API void stopping() override;

private:

    /// Desired bins width used to classified pixel.
    float m_binsWidth;

};
} // namespace processing
} // namespace scene2D
