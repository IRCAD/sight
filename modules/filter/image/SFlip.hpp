/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/filter/image/config.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Slots.hpp>

#include <data/Image.hpp>

#include <service/IFilter.hpp>

namespace sight::module::filter::image
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
       <service type="sight::module::filter::image::SFlip">
           <in key="source" uid="..." autoConnect="true" />
           <out key="target" uid="..." />
       </service>
   @endcode
 * @subsection Input Input
 * - \b source [sight::data::Image]: Image to flip.
 * @subsection Output Output:
 * - \b target [sight::data::Image]: New flipped image.
 */
class MODULE_FILTER_IMAGE_CLASS_API SFlip : public service::IFilter
{
public:

    SIGHT_DECLARE_SERVICE(SFlip, sight::service::IFilter);

    MODULE_FILTER_IMAGE_API static const core::com::Slots::SlotKeyType s_FLIP_AXIS_X_SLOT;
    MODULE_FILTER_IMAGE_API static const core::com::Slots::SlotKeyType s_FLIP_AXIS_Y_SLOT;
    MODULE_FILTER_IMAGE_API static const core::com::Slots::SlotKeyType s_FLIP_AXIS_Z_SLOT;

    /// Constructor, does nothing.
    MODULE_FILTER_IMAGE_API SFlip();

    /// Destructor, does nothing.
    MODULE_FILTER_IMAGE_API ~SFlip();

protected:

    /// Configures the service.
    MODULE_FILTER_IMAGE_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void starting() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void stopping() override;

    /// Apply the flip operator.
    MODULE_FILTER_IMAGE_API void updating() override;

    /**
     * @brief Signal-slot auto-connection proposals
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    MODULE_FILTER_IMAGE_API KeyConnectionsMap getAutoConnections() const override;

private:

    /// Slot: flip the first axis
    void flipAxisX();

    /// Slot: flip the second axis
    void flipAxisY();

    /// Slot: flip the third axis
    void flipAxisZ();

    // Store whether to flip or not one of the 3 axis
    std::array<bool, 3> m_flipAxes {{false, false, false}};

    static constexpr std::string_view s_IMAGE_IN  = "source";
    static constexpr std::string_view s_IMAGE_OUT = "target";

    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_source {this, s_IMAGE_IN, true};
    sight::data::ptr<sight::data::Image, sight::data::Access::out> m_target {this, s_IMAGE_OUT, false};
};

} // namespace sight::module::filter::image
