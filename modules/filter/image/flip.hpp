/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/com/slots.hpp>

#include <data/image.hpp>

#include <service/filter.hpp>

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
       <service type="sight::module::filter::image::flip">
           <in key="source" uid="..." auto_connect="true" />
           <out key="target" uid="..." />
       </service>
   @endcode
 * @subsection Input Input
 * - \b source [sight::data::image]: image to flip.
 * @subsection Output Output:
 * - \b target [sight::data::image]: New flipped image.
 */
class flip : public service::filter
{
public:

    SIGHT_DECLARE_SERVICE(flip, sight::service::filter);

    static const core::com::slots::key_t FLIP_AXIS_X_SLOT;
    static const core::com::slots::key_t FLIP_AXIS_Y_SLOT;
    static const core::com::slots::key_t FLIP_AXIS_Z_SLOT;

    /// Constructor, does nothing.
    flip();

    /// Destructor, does nothing.
    ~flip() override;

protected:

    /// Configures the service.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Does nothing.
    void stopping() override;

    /// Apply the flip operator.
    void updating() override;

    /**
     * @brief Signal-slot auto-connection proposals
     *
     * Connect image::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect image::BUFFER_MODIFIED_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

private:

    /// Slot: flip the first axis
    void flip_axis_x();

    /// Slot: flip the second axis
    void flip_axis_y();

    /// Slot: flip the third axis
    void flip_axis_z();

    // Store whether to flip or not one of the 3 axis
    std::array<bool, 3> m_flip_axes {{false, false, false}};

    static constexpr std::string_view IMAGE_IN  = "source";
    static constexpr std::string_view IMAGE_OUT = "target";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_source {this, IMAGE_IN, true};
    sight::data::ptr<sight::data::image, sight::data::access::out> m_target {this, IMAGE_OUT, false};
};

} // namespace sight::module::filter::image
